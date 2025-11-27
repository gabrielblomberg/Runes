#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <tuple>
#include <future>
#include <thread>
#include "utility/TypeList.h"

#include "utility/Time.h"

class BaseSubscription {};

using Subscription = std::unique_ptr<BaseSubscription>;
using Subscriptions = std::vector<Subscription>;

template<typename T>
class MemoryTopic;

template<typename Message>
class BroadcastTopic
{
public:

    using Callback = std::function<void(const Message &)>;

    class SyncSubscription : public BaseSubscription
    {
    public:

        SyncSubscription(SyncSubscription &&sub)
            : m_topic(std::move(sub.m_topic))
            , m_id(sub.m_id)
            , m_mutex(std::move(sub.m_mutex))
            , m_queue()
        {
            std::unique_lock lock(sub.m_mutex);
            m_topic = sub.m_topic;
            m_id = sub.m_id;
            m_queue = std::move(sub.m_queue);
            
            sub.m_topic = nullptr;
            sub.m_id = -1;
        }

        ~SyncSubscription()
        {
            m_topic.unsubscribe(m_id);
        }

        inline std::optional<Message> get()
        {
            std::unique_lock lock(m_mutex);
            if (m_queue.empty())
                return std::nullopt;

            std::optional<Message> message = std::move(m_queue.front());
            m_queue.pop_front();
            return message;
        }

    private:

        friend class BroadcastTopic<Message>;
        friend class MemoryTopic<Message>;

        SyncSubscription(BroadcastTopic<Message> *topic, std::size_t id)
            : m_topic(topic)
            , m_id(id)
        {}

        inline void push(Message message)
        {
            std::unique_lock lock(m_mutex);
            m_queue.push_back(std::move(message));
        }

        BroadcastTopic<Message> *m_topic;
        std::size_t m_id;
        std::mutex m_mutex;
        std::deque<Message> m_queue;
    };

    class AsyncSubscription : public BaseSubscription
    {
    public:

        inline void unsubscribe()
        {
            m_topic->unsubscribe(m_id);
        }

        ~AsyncSubscription()
        {
            unsubscribe();
        }

    private:

        friend class BroadcastTopic<Message>;
        friend class MemoryTopic<Message>;

        AsyncSubscription(BroadcastTopic<Message> *topic, std::size_t id)
            : m_topic(topic)
            , m_id(id)
        {}

        BroadcastTopic<Message> *m_topic;
        std::size_t m_id;
    };

    BroadcastTopic(std::stop_token stop)
        : m_stop(stop)
    {
        m_thread = std::jthread(&BroadcastTopic::worker, this);
    }

    void publish(Message&& message)
    {
        {
            std::unique_lock lock(m_mutex);
            m_queue.emplace_back(std::move(message));
        }
        m_condition.notify_all();
    }

    template<typename... Args>
    void publish(Args&&... args)
    {
        return publish(Message{args...});
    }

    Subscription subscribe_sync()
    {
        std::unique_lock lock(m_mutex);
        std::size_t id = m_next_subscription_id++;
        auto subscription = std::unique_ptr<SyncSubscription>(new SyncSubscription(this, id));
        m_sync_subs.emplace(id, subscription.get());
        return subscription;
    }

    Subscription subscribe_async(Callback&& callback)
    {
        std::unique_lock lock(m_mutex);
        std::size_t id = m_next_subscription_id++;
        m_async_subs.emplace(id, callback);
        auto subscription = std::unique_ptr<AsyncSubscription>(new AsyncSubscription(this, id));
        return subscription;
    }

protected:

    void unsubscribe(std::size_t id)
    {
        std::unique_lock lock(m_mutex);

        auto sub = m_async_subs.find(id);
        if (sub != m_async_subs.end()) {
            m_async_subs.erase(sub);
            return;
        }

        auto buffer = m_sync_subs.find(id);
        if (buffer != m_sync_subs.end())
            m_sync_subs.erase(buffer);
    }

    void worker()
    {
        while (!m_stop.stop_requested())
            work();
    }

    virtual void work()
    {
        std::unique_lock lock(m_mutex);
        while (m_queue.empty()) {
            m_condition.wait(lock, m_stop, [this]{ return !m_queue.empty(); });
        }

        if (!m_stop.stop_requested())
            return;

        Message message = std::move(m_queue.front());
        m_queue.pop_front();

        for (auto &buffer : m_sync_subs)
            buffer.second->push(message);

        for (auto &[id, callback] : m_async_subs)
            callback(message);
    }

    std::stop_token m_stop;
    std::mutex m_mutex;
    std::condition_variable_any m_condition;
    std::deque<Message> m_queue;
    std::atomic_size_t m_next_subscription_id;
    std::unordered_map<std::size_t, SyncSubscription*> m_sync_subs;
    std::unordered_map<std::size_t, Callback> m_async_subs;
    std::jthread m_thread;
};

template<typename Message>
class MemoryTopic : public BroadcastTopic<Message>
{
public:

    using BroadcastTopic<Message>::BroadcastTopic;
    using BroadcastTopic<Message>::SyncSubscription;
    using BroadcastTopic<Message>::AsyncSubscription;

    inline Message get() {
        std::unique_lock lock(this->m_mutex);
        return m_message;
    }

private:

    void work() override {
        std::unique_lock lock(this->m_mutex);
        while (this->m_queue.empty()) {
            this->m_condition.wait(lock, this->m_stop, [this]{ return !this->m_queue.empty(); });
        }

        if (!this->m_stop.stop_requested())
            return;

        m_message = std::move(this->m_queue.front());
        this->m_queue.pop_front();

        for (auto &buffer : this->m_sync_subs)
            buffer.second->push(m_message);

        for (auto &[id, callback] : this->m_async_subs)
            callback(m_message);
    }

    Message m_message;
};

template<typename Request, typename Response>
class RequestTopic
{
public:

    class AsyncSubscription : public BaseSubscription
    {
    public:

        ~AsyncSubscription()
        {
            m_topic->unsubscribe();
        }

    private:

        AsyncSubscription(RequestTopic<Request, Response> *topic)
            : m_topic(topic)
        {}

        friend class RequestTopic<Request, Response>;

        RequestTopic<Request, Response> *m_topic;
    };

    using Callback = std::function<Response(const Request&)>;

    RequestTopic(std::stop_token stop)
        : m_stop(stop)
    {}

    Subscription subscribe(Callback&& callback)
    {
        std::unique_lock lock(m_mutex);
        assert(m_callback && "already subscribed to request topic");

        m_callback = callback;
        return std::unique_ptr<AsyncSubscription>(new AsyncSubscription(this));
    }

    std::optional<Response> request_sync(Request&& request, Time::Duration timeout)
    {
        if (!m_callback)
            return std::nullopt;

        std::promise<Response> promise;
        std::future<Response> future = promise.get_future();

        {
            std::unique_lock lock(m_mutex);
            m_queue.emplace(request, promise);
        }

        m_condition.notify_all();

        if (future.wait_for(timeout) == std::future_status::ready)
            return std::move(future.get());

        return std::nullopt;
    }

private:

    void unsubscribe()
    {
        m_callback = nullptr;
    }

    void thread()
    {
        while (!m_stop.stop_requested())
            work();
    }

    void work()
    {
        std::unique_lock lock(m_mutex);
        while (m_queue.empty()) {
            m_condition.wait(lock, m_stop, [this]{ return !m_queue.empty(); });
        }

        if (m_stop.stop_requested())
            return;

        auto [request, promise] = std::move(m_queue.front());
        m_queue.pop_front();

        promise.set_value(std::move(m_callback(request)));
    }

    std::stop_token m_stop;
    std::mutex m_mutex;
    std::condition_variable_any m_condition;
    std::deque<std::tuple<Request, std::promise<Response>>> m_queue;
    Callback m_callback;
};

/**
 * @brief Messenger of topics.
 * @tparam Topic The topics.
 */
template<typename BroadcastTopics, typename MemoryTopics, typename RequestTopics>
class Messenger
{
public:

    template<typename... Args>
    Messenger(std::stop_token stop, Args&& ...args)
        : m_broadcast_topics(
            [stop]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::tuple{((void)Is, stop)...};
            }(std::make_index_sequence<TypeList::Size<BroadcastTopics>>{})
        ),
        m_memory_topics(
            [stop]<std::size_t... Is>(std::index_sequence<Is...>, auto&& values) {
                return std::tuple{((void)Is, stop, )...};
            }(std::make_index_sequence<TypeList::Size<MemoryTopics>>{}, std::forward<Args>(args)...)
        ),
        m_request_topics(
            [stop]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::tuple{((void)Is, stop)...};
            }(std::make_index_sequence<TypeList::Size<RequestTopics>>{})
        )
    {}

    /**
     * @brief Construct a new Messenger object
     * 
     * @param stop A stop source to use if provided.
     * @param threads The number of threads to handle messages with.
     */
    Messenger(std::stop_token stop);

    /**
     * @brief Subscribe to a topic.
     * 
     * @tparam Topic The topic to subscribe to.
     * @param function The function to callback on to receive data.
     * 
     * @returns An integer identifier of the subscription.
     */
    template<typename Topic, typename Callback>
    auto subscribe(Callback &&function)
    {
        const constexpr auto Index = TypeList::Index<Topics, Topic>;
        return std::get<Index>(m_topics).subscribe(std::forward(function));
    }

    /**
     * @brief Publish data to a topic.
     * 
     * @tparam Topic The topic to publish data to.
     * @param data The data to publish to all subscribers.
     */
    template<typename Topic, typename Message>
    void publish(Message &&data)
    {
        const constexpr auto Index = TypeList::Index<Topics, Topic>;
        std::get<Index>(m_topics).publish(std::forward(data));
    }

    /**
     * @brief Publish data to a topic.
     * 
     * @tparam Topic The topic to publish data to.
     * @param args The data to publish to all subscribers.
     */
    template<typename Topic, typename... Args>
    inline void publish(Args&&... args)
    {
        const constexpr auto Index = TypeList::Index<Topics, Topic>;
        std::get<Index>(m_topics).publish(std::forward(args)...);
    }

    /**
     * @brief Read memory values from memory topics directly.
     */
    template<typename Topic>
    inline auto get()
    {
        const constexpr auto Index = TypeList::Index<Topics, Topic>;
        return std::get<Index>(m_topics).get();
    }

private:

    TypeList::TupleOf<BroadcastTopics> m_broadcast_topics;

    TypeList::TupleOf<MemoryTopics> m_memory_topics;

    TypeList::TupleOf<RequestTopics> m_request_topics;
};
