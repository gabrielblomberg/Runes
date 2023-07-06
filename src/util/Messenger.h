#pragma once

#include <mutex>
#include <tuple>
#include <deque>
#include <functional>

#include "util/TypeList.h"

/**
 * @brief A class responsible for being an intermediary between code publishing
 * messages and code subscribed to them.
 * 
 * The topic types must not be aliases or else the message type is duped and
 * the messenger will use the same stream for both channels. Each type should
 * be a struct.
 * 
 * @tparam Topics A type list of the types of each message
 */
template<typename Topics>
class Messenger
{
public:

    /**
     * @brief Construct a new Messenger object
     * 
     * @param threads The number of threads to handle messages with.
     */
    Messenger(std::size_t threads = 4);

    /**
     * @brief Subscribe to a topic.
     * 
     * @tparam Topic The topic to subscribe to.
     * @param function The function to callback on to receive data.
     * 
     * @returns An integer identifier of the subscription.
     */
    template<std::size_t Topic>
    int subscribe(
        std::function<void(const TypeList::Get<Topics, Topic> &)> function
    );

    /**
     * @brief Publish data to a topic.
     * 
     * @tparam Topic The topic to publish data to.
     * @param data The data to publish to all subscribers.
     */
    template<std::size_t Topic>
    void publish(TypeList::Get<Topics, Topic> &&data);

    /**
     * @brief Publish data to a topic.
     * 
     * @tparam Topic The topic to publish data to.
     * @param args The data to publish to all subscribers.
     */
    template<std::size_t Topic, typename... Args>
    inline void publish(Args&&... args) {
        publish<Topic>(
            TypeList::Get<Topics, Topic>(
                std::forward<Args>(args)...
            )
        );
    }

private:

    /**
     * @brief Class responsible for owning a working thread.
     */
    class Worker
    {
    public:

        Worker();

        /**
         * @brief Checks if the worker is ready to receive another task.
         */
        bool Ready();

    private:

        /// Thread executing subscription functions.
        std::jthread m_thread;
    };

    /**
     * @brief Meta function that transform a message type to a channel of those
     * messages.
     */
    template<typename T>
    struct MakeChannel {
        using type = std::pair<std::mutex, std::deque<T>>;
    };

    /**
     * @brief Type denoting the queues of topic messages.
     */
    using Channels = TypeList::TupleOf<TypeList::Transform<Topics, MakeChannel>>;

    /**
     * @brief Meta function that transform a message type to a collection of
     * callbacks on that message.
     */
    template<typename T>
    struct MakeSubscription {
        using type = std::vector<std::function<void(const T&)>>;
    };

    /**
     * @brief Type denoting the subscriptions to each topic.
     */
    using Subscriptions = TypeList::TupleOf<TypeList::Transform<Topics, MakeSubscription>>;

    /// Queues of messages to be processed.
    Channels m_channels;

    /// Threads handling messages.
    std::vector<Worker> m_workers;

    /// The number of topics created by the messenger.
    std::array<unsigned, TypeList::Size<Topics>> m_counters;

    /// The subscriptions to each topic.
    Subscriptions m_subscriptions;
};

template<typename Topics>
Messenger<Topics>::Messenger(std::size_t threads)
    : m_workers()
    , m_counters()
    , m_subscriptions()
{}

template<typename Topics>
template<std::size_t Topic>
int Messenger<Topics>::subscribe(
    std::function<void(const TypeList::Get<Topics, Topic> &)> function
) {
    // Lock the vector of callbacks for updating.
    std::scoped_lock<std::mutex> lock(std::get<Topic>(m_channels).first);

    std::get<Topic>(m_subscriptions).push_back(function);
    m_counters[Topic]++;
    return m_counters[Topic];
}

template<typename Topics>
template<std::size_t Topic>
void Messenger<Topics>::publish(TypeList::Get<Topics, Topic> &&data)
{
    for (auto &callback : std::get<Topic>(m_subscriptions))
        callback(data);
}
