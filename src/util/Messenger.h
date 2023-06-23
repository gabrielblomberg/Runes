#pragma once

#include <array>
#include <memory>
#include <string>
#include <unordered_map>

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
     */
    Messenger();

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
     * @brief Type denoting the queues of topic messages.
     */
    using Streams = TypeList::Tuple<TypeList::List<Topics>>;

    /**
     * @brief Type denoting the subscriptions to each topic.
     */
    using Subscriptions = TypeList::Tuple<
        TypeList::Vector<TypeList::Function<Topics, void>>
    >;

    /// The number of topics created by the messenger.
    std::array<unsigned, TypeList::Size<Topics>> m_counters;

    /// The queues of messages on each topic.
    Streams m_topics;

    /// The subscriptions to each topic.
    Subscriptions m_subscriptions;
};

template<typename Topics>
Messenger<Topics>::Messenger()
    : m_counters()
    , m_topics()
    , m_subscriptions()
{}

template<typename Topics>
template<std::size_t Topic>
int Messenger<Topics>::subscribe(
    std::function<void(const TypeList::Get<Topics, Topic> &)> function
) {
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
