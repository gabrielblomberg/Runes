#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <tuple>
#include <future>
#include <thread>

#include "utility/Time.h"

template<typename Message>
class BroadcastTopic
{
public:

    using Callback = std::function<void(const Message &)>;

    class SyncSubscription
    {
    public:

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

        friend class Topic;

        SyncSubscription(BroadcastTopic<Message> *topic, std::size_t id)
            : m_topic(topic)
            , m_id(id)
        {}

        inline void push(Message&& message)
        {
            std::unique_lock lock(m_mutex);
            m_queue.push_back(std::move(message));
        }

        BroadcastTopic<Message> *m_topic;
        std::size_t m_id;
        std::mutex m_mutex;
        std::deque<Message> m_queue;
    };

    class AsyncSubscription
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
        BroadcastTopic<Message> *m_topic;
        std::size_t m_id;
    };

    BroadcastTopic(std::stop_token stop)
        : m_stop(stop)
    {
        m_thread = std::jthread(BroadcastTopic::worker, this);
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

    SyncSubscription subscribe_sync()
    {
        std::unique_lock lock(m_mutex);
        std::size_t id = m_next_subscription_id++;
        SyncSubscription subscription {this, id};
        m_sync_subs.emplace(id, &subscription);
        return subscription;
    }

    AsyncSubscription subscribe_async(Callback&& callback)
    {
        std::unique_lock lock(m_mutex);
        std::size_t id = m_next_subscription_id++;
        m_async_subs.emplace(id, callback);
        return Subscription(this, id);
    }

protected:

    void unsubscribe(std::size_t id)
    {
        std::unique_lock lock(m_mutex);

        auto sub = m_async_subs.find(id);
        if (sub != m_async_subs.end()) {
            m_async_subs.erase(sub);
            return
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
            buffer.push(message);

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

        for (auto [id, callback] : this->m_subscriptions)
            callback(m_message);
    }

    Message m_message;
};

template<typename Request, typename Response>
class EndpointTopic
{
public:

    using Callback = std::function<Response(const Request&)>;

    EndpointTopic(Callback &&callback = {})
        : m_callback(callback)
    {}

    void register_endpoint(Callback&& callback)
    {
        if (!m_callback)
            m_callback = callback
    }

    std::optional<Response> request(Request&& request, Time::Duration timeout)
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
