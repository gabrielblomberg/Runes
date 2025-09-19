#pragma once

#include <thread>
#include <array>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
#include <tuple>

#include "utility/TypeList.h"

template<typename Topic>
struct IsBroadcastTopic {
    static const constexpr bool value = requires {
        typename Topic::Message;
    };
    constexpr operator bool() { return value; };
};

template<typename Topic>
struct IsRequestTopic {
    static const constexpr bool value = requires {
        typename Topic::Request;
        typename Topic::Response;
    };
    constexpr operator bool() { return value; };
};

template<typename Topic>
struct IsMemoryTopic {
    static const constexpr bool value = requires {
        typename Topic::Message;
        {Topic::DEFAULT} -> std::same_as<Topic::Message>; 
    };
    constexpr operator bool() { return value; };
};

/**
 * @brief Broadcast topics emit messages which can be subscribed to.
 */
template<typename Topic>
requires requires { IsBroadcastTopic<Topic>; }
struct BroadcastTopic {};

/**
 * @brief Request topics make a request to a component, which responds with
 * data.
 */
template<typename Topic>
requires requires { IsRequestTopic<Topic>; }
struct RequestTopic {};

/**
 * @brief Memory topics store cached values which can be subscribed to on
 * update, and have a default value.
 */
template<typename Topic>
requires requires { IsMemoryTopic<Topic>; }
struct MemoryTopic {};

/**
 * @brief Messenger of topics.
 * @tparam Topic The topics.
 */
template<typename Topics>
class Messenger
{
public:

    using BroadcastTopics = TypeList::Filter<Topics, IsBroadcastTopic>;
    using MemoryTopics = TypeList::Filter<Topics, IsMemoryTopic>;
    using RequestTopics = TypeList::Filter<Topics, IsRequestTopic>;

    using SubscriptionTopics = TypeList::Concatenate<BroadcastTopics, MemoryTopics>;

    /**
     * @brief Construct a new Messenger object
     * 
     * @param stop A stop source to use if provided.
     * @param threads The number of threads to handle messages with.
     */
    Messenger(
        std::optional<std::stop_source> stop = std::nullopt,
        std::size_t threads = 4
    );

    /**
     * @brief Stops the threads.
     */
    ~Messenger();

    /**
     * @brief Subscribe to a topic.
     * 
     * @tparam Topic The topic to subscribe to.
     * @param function The function to callback on to receive data.
     * 
     * @returns An integer identifier of the subscription.
     */
    template<typename Topic>
    void subscribe(
        std::function<void(const TypeList::Find<SubscriptionTopics, Topic>::Message &)> &&function
    ) {
        // Lock the vector of callbacks for updating.
        const constexpr auto Index = TypeList::Index<Topics, Topic>;
        auto &channel = std::get<Topic>(Index)>(m_channels);

        std::scoped_lock lock(channel.mutex);
        channel.callbacks.push_back(
            [function](void *message){
                function(*static_cast<Topic::Message*>(message));
            }
        );

        // If it's a memory topic, immediately call the function with the
        // currently cached value.
        if constexpr ( requires {IsMemoryTopic<Topic>} ) {
            channel.callbacks.back()(static_cast<Topic::Message*>(&m_memory[Index]));
        }
    }

    /**
     * @brief Publish data to a topic.
     * 
     * @tparam Topic The topic to publish data to.
     * @param data The data to publish to all subscribers.
     */
    template<typename Topic>
    void publish(TypeList::Find<SubscriptionTopics, Topic>::Message &&data)
    {
        std::scoped_lock lock(m_mutex);
        m_queue.emplace_back(std::make_pair(
                TypeList::Index<SubscriptionTopics, Topic>,
                std::make_shared<TypeList::Find<SubscriptionTopics, Topic>::Message>(data)
            ));
        }

        m_condition.notify_all();
    }

    /**
     * @brief Publish data to a topic.
     * 
     * @tparam Topic The topic to publish data to.
     * @param args The data to publish to all subscribers.
     */
    template<typename Topic, typename... Args>
    inline void publish(Args&&... args) {
        publish<Topic>(
            TypeList::Find<SubscriptionTopics, Topic>::Message(std::forward<Args>(args)...)
        );
    }

    /**
     * @brief Read memory values from memory topics directly.
     */
    template<typename Topic>
    inline TypeList::Find<MemoryTopics, Topic>::Message get()
    {
        std::unique_lock lock(m_mutex);
        const constexpr auto Index = TypeList::Index<SubscriptionTopics, Topic>;
        return std::get<Index>(m_memory);
    }

private:

    template<typename T>
    struct GetValue {
        using value = T::Message;
    };

    /**
     * @brief Channel type.
     */
    struct Subscriptions
    {
        // Mutex protecting concurrent calling back of call backs.
        std::mutex mutex;

        // Callbacks on each message.
        std::vector<std::function<void(void*)>> callbacks;
    };

    /**
     * @brief Thread of each worker processing messages.
     * @param stop A stop token to stop the worker thread.
     */
    void worker(std::stop_token stop);

    /// Channels for each topic.
    std::array<Subscriptions, TypeList::Size<SubscriptionTopics>> m_channels;

    /// Values of memory topics.
    TypeList::TupleOf<TypeList::Map<GetValue, MemoryTopics>> m_memory;

    /// Queues of messages to be processed.
    std::deque<std::pair<std::size_t, std::shared_ptr<void>>> m_queue;

    /// Mutex protecting concurrent access to m_queue and condition variable.
    std::mutex m_mutex;

    /// Condition workers wait on.
    std::condition_variable_any m_condition;

    /// Threads handling messages.
    std::vector<std::jthread> m_workers;

    /// Source for stopping the messaging.
    std::stop_source m_stop_source;
};

template<typename Topics>
Messenger<Topics>::Messenger(
    std::optional<std::stop_source> stop,
    std::size_t threads
) {
    if (stop) {
        m_stop_source = stop.value();
    }

    for (std::size_t i = 0; i < threads; i++) {
        m_workers.push_back(
            std::jthread(&Messenger::worker, this, m_stop_source.get_token())
        );
    }
}

template<typename Topics>
Messenger<Topics>::~Messenger()
{
    m_stop_source.request_stop();
}

template<typename Topics>
void Messenger<Topics>::worker(std::stop_token stop)
{
    using namespace std::chrono_literals;

    while (!stop.stop_requested()) {

        // Pointer to the message when found.
        std::shared_ptr<void> message {nullptr};

        // The topic the message belongs to.
        std::size_t topic {0};

        // Lock on the channel to ensure no two threads are calling back on two
        // messages at the same time and potentially out of order.
        std::unique_lock<std::mutex> channel_lock {};

        // If a message was found and needs to be processed. Prevents busy loop
        // checking for messages in queue when no channel is able to be locked.
        bool found = false;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            while (m_queue.empty()) {

                // Wait until the stop is signalled or a message exists.
                m_condition.wait(lock, stop, [&]{ return !m_queue.empty(); });

                // Stop when requested.
                if (stop.stop_requested()) {
                    return;
                }
            }

            // Find next available message.
            for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {

                // If another thread has the topic lock then it will get this me
                channel_lock = std::unique_lock(
                    m_channels[it->first].mutex,
                    std::try_to_lock
                );

                if (!channel_lock)
                    continue;

                // Remove the message from the queue.
                std::tie(topic, message) = *it;
                m_queue.erase(it);

                // A message is ready to be processed.
                found = true;
                break;
            }
        }

        // If no message was found to process then wait for a bit to prevent
        // busy loop and continue.
        if (!found) {
            std::this_thread::sleep_for(1ms);
            continue;
        }

        for (const auto &function : m_channels[topic].callbacks) {
            if (stop.stop_requested())
                return;

            function(message.get());
        }
    }
}
