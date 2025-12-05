#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <vector>

#include "utility/TypeList.h"

template<typename Messages>
class MultiQueue
{
public:

    template<typename T>
    class Queue
    {
    public:

        std::deque<T>& queue() { return m_queue; }

        inline T pop()
        {
            T value = m_queue.front();
            m_queue.pop_front();
            return value;
        }

        void unhandle() { m_event_queue->unhandle_event_queue(m_id); }

        ~Queue() { unhandle(); }

    private:

        friend class Queue<Messages>;

        Queue(MultiQueue<Messages>* multi_queue)
            : m_queue(multi_queue)
        {}

        MultiQueue<Messages>* m_event_queue;

        std::size_t m_id;

        std::deque<T> m_queue;
    };

    template<std::size_t MessageIndex, typename... Args>
    void push_event(Args&&... args)
    {
        push_event<MessageIndex>({args...});
    }

    /**
     * @brief Publish an event.
     */
    template<std::size_t MessageIndex>
    void push_event(const TypeList::Get<Messages, MessageIndex>& event)
    {
        for (auto& handler : std::get<MessageIndex>(m_events))
            handler.push(event);
    }

    /**
     * @brief Make a new queue for an event.
     */
    template<std::size_t MessageIndex>
    Queue<TypeList::Get<Messages, MessageIndex>> handle_event()
    {
        std::unique_lock lock(m_mutex);
        auto queue = std::make_shared<Queue<TypeList::Get<Messages, MessageIndex>>>();
        std::get<MessageIndex>(m_events).emplace_back(queue);
        return queue;
    }

private:

    void unhandle_event_queue(std::size_t id)
    {
        std::apply(
            [id](auto&... handlers) {
                (std::erase_if(handlers, [id](auto& handler) { return handler->m_id == id; }), ...);
            },
            m_events
        );
    }

    template<typename T>
    struct MakeQueue {
        using type = std::vector<std::shared_ptr<Queue<T>>>;
    };

    TypeList::Map<MakeQueue, Messages> m_events;

    std::mutex m_mutex;
};
