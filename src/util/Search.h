#include <unordered_set>
#include <queue>
#include <optional>
#include <vector>
#include <memory>

/**
 * @brief A search strategy.
 * 
 * @tparam State The type of each
 * @tparam Compare The comparator that orders search nodes in the frontier. If
 * this is void, then a standard deque is used without insertion ordering.
 */
template<typename State, typename Compare = void>
class Search {
public:

    /**
     * @brief A node in the search tree that contains data pertaining to the
     * search strategy.
     * 
     * Examples of additional values stored in the search node include weights,
     * state transitions and costs.
     */
    struct Node {

        Node(State state, const std::shared_ptr<Node> &predecessor)
            : state(state)
            , predecessor(predecessor)
        {}

        /// The state of the node.
        State state;

        /// The previous search node visited to get to this node.
        std::shared_ptr<Node> predecessor;

        virtual ~Node() = default;
    };

    /**
     * @brief Function that checks if a state is the goal state.
     */
    using Checker = std::function<bool(const State&)>;

    /**
     * @brief Function that returns all the subsequent states from a state.
     */
    using Successors = std::function<std::vector<State>(const State &)>;

    /**
     * @brief Create a new search problem
     * 
     * @param initial The initial state.
     * @param successor A function that takes a state and returns all subsequent
     * states.
     * @param is_goal A function that takes a state and returns if that state is
     * the goal state.
     */
    Search(State initial, Successors successor, Checker is_goal);

    /**
     * @brief Perform the search operation.
     * 
     * @return A sequence of states to the goal or std::nullopt if no solution
     * exists.
     */
    std::optional<std::vector<std::shared_ptr<Node>>> perform();

    /**
     * @brief Get all the states visited during the search.
     */
    inline const std::unordered_set<State> &visited() {
        return m_visited;
    }

protected:

    /**
     * @brief Checks if the search frontier is empty.
     * @return If the search frontier is empty.
     */
    virtual bool frontier_empty();

    /**
     * @brief Push a search node into the frontier.
     */
    virtual void frontier_push(std::shared_ptr<Node> search_node);

    /**
     * @brief Pop the next search node from the frontier.
     * @returns The next node to check and search.
     */
    virtual std::shared_ptr<Node> frontier_pop();

    /**
     * @brief Make a node.
     * 
     * Used by derived classes to construct their own node types and cast them
     * to a node.
     * 
     * @param state The state of the node.
     * @param previous The previous node.
     * 
     * @return A shared pointer to a base class node.
     */
    virtual std::shared_ptr<Node> make_node(
        const State &state,
        const std::shared_ptr<Node> &previous
    );

    /**
     * @brief Get all the successor search nodes from a state.
     */
    std::vector<std::shared_ptr<Node>>
    successors(std::shared_ptr<Node> &node);

    /// Function that returns the subsequent states of a state.
    Successors m_successors;

    /// Function that checks if a state is the goal state.
    Checker m_is_goal;

    /// The frontier of vertices to search through. A deque if not comparing
    /// search states or a priority queue if search states can be compared.
    std::conditional_t<
        std::is_void_v<Compare>,
        std::deque<std::shared_ptr<Node>>,
        std::priority_queue<
            std::shared_ptr<Node>,
            std::vector<std::shared_ptr<Node>>,
            Compare
        >
    > m_frontier;

    // The set of visited search nodes that should not be revisited.
    std::unordered_set<State> m_visited;
};

template<typename State, typename Compare>
Search<State, Compare>::Search(
        State initial,
        Successors successors,
        Checker is_goal
  ) : m_successors(successors)
    , m_is_goal(is_goal)
    , m_frontier({make_node(initial, nullptr)})
    , m_visited()
{}

template<typename State, typename Compare>
bool Search<State, Compare>::frontier_empty()
{
    return m_frontier.empty();
}

template<typename State, typename Compare>
void Search<State, Compare>::frontier_push(std::shared_ptr<Node> search_node)
{
    if constexpr (std::is_void_v<Compare>) {
        m_frontier.push_back(search_node);
    }
    else {
        m_frontier.push(search_node);
    }
};

template<typename State, typename Compare>
std::shared_ptr<typename Search<State, Compare>::Node>
Search<State, Compare>::frontier_pop()
{
    auto node = std::move(m_frontier.front());
    m_frontier.pop_front();
    return std::move(node);
}

template<typename State, typename Compare>
std::shared_ptr<typename Search<State, Compare>::Node>
Search<State, Compare>::make_node(
    const State &state,
    const std::shared_ptr<Node> &previous
) {
    return std::make_shared<Node>(state, previous);
}

template<typename State, typename Compare>
std::vector<std::shared_ptr<typename Search<State, Compare>::Node>>
Search<State, Compare>::successors(std::shared_ptr<Node> &node)
{
    // The states that are immediately reachable from state.
    std::vector<State> next_states = m_successors(node->state);

    // Construct the search nodes of each new state.
    std::vector<std::shared_ptr<Node>> nodes {};
    nodes.reserve(next_states.size());

    for (const State &next : next_states) {
        nodes.push_back(make_node(next, node));
    }

    return std::move(nodes);
}

template<typename State, typename Compare>
std::optional<std::vector<std::shared_ptr<typename Search<State, Compare>::Node>>>
Search<State, Compare>::perform()
{
    while (!m_frontier.empty()) {
        std::shared_ptr<typename Search<State, void>::Node> node = frontier_pop();

        // If its the not the goal then mark this node visited and add its
        // subsequent search nodes to the frontier.
        if (!m_is_goal(node->state)) {
            m_visited.emplace(node->state);

            for (auto &next : successors(node))
                if (m_visited.find(next->state) == m_visited.end())
                    frontier_push(next);

            continue;
        }

        // Found the goal, traverse backwards to generate the sequence of
        // search nodes visited to get to the goal
        std::vector<std::shared_ptr<Node>> trace {};
        for (; node; node = node->predecessor)
            trace.push_back(node);

        // For memory saving.
        m_frontier.clear();
        m_visited.clear();

        // Convert to start to finish ordering.
        std::reverse(trace.begin(), trace.end());
        return std::move(trace);
    }

    return std::nullopt;
}

/**
 * @brief Depth first search. Search tree traversed breadth first.
 */
template<typename State>
using BFS = Search<State, void>;

/**
 * @brief Depth first search. Search tree traversed depth first.
 */
template<typename State>
class DFS : public Search<State, void>
{
public:

    using Search<State, void>::Node;
    using Search<State, void>::Search;

private:

    /**
     * @brief Pop the newest search node added to the frontier.
     * @returns The newest node on the frontier.
     */
    std::shared_ptr<typename Search<State, void>::Node> frontier_pop() override;

    using Search<State, void>::m_frontier;
};

template<typename State>
std::shared_ptr<typename Search<State, void>::Node>
DFS<State>::frontier_pop()
{
    auto node = std::move(m_frontier.back());
    m_frontier.pop_back();
    return std::move(node);
}

/**
 * @brief Iteratively deepening depth first search. 
 */
template<typename State>
class IDDFS : public Search<State, void>
{
public:

    /**
     * @brief An IDDFS node keeps track of its height in the search tree.
     */
    struct Node : public Search<State, void>::Node {

        Node(
            State state,
            std::shared_ptr<typename Search<State, void>::Node> previous,
            int k
          ) : Search<State, void>::Node(state, previous)
            , k(k)
        {}

        /// The height of the search node in the search tree.
        int k;
    };

    IDDFS(
            State initial,
            Search<State, void>::Successors successor,
            Search<State, void>::Checker is_goal
      ) : Search<State, void>(initial, successor, is_goal)
        , m_initial_state(initial)
        , m_cutoff(3)
        , m_deepen(false)
        , m_depths()
    {}

private:

    std::shared_ptr<typename Search<State, void>::Node> make_node(
        const State &state,
        const std::shared_ptr<typename Search<State, void>::Node> &previous
    ) override;

    bool frontier_empty() override;

    void frontier_push(
        std::shared_ptr<typename Search<State, void>::Node> search_node
    ) override;

    /**
     * @brief Pop the newest search node added to the frontier.
     * @returns The newest node on the frontier.
     */
    std::shared_ptr<typename Search<State, void>::Node> frontier_pop() override;

    using Search<State, void>::m_frontier;
    using Search<State, void>::m_visited;

    /// Cache of the initial state for restarts.
    State m_initial_state;

    /// The cutoff of tree depth before restarting the search.
    int m_cutoff;

    /// Whether to restart the search.
    bool m_deepen;

    /// Record of the visited state depths.
    std::unordered_map<State, int> m_depths;
};

template<typename State>
std::shared_ptr<typename Search<State, void>::Node>
IDDFS<State>::make_node(
    const State &state,
    const std::shared_ptr<typename Search<State, void>::Node> &previous
) {
    const Node *node = (Node*)previous.get();
    return std::make_shared<Node>(
        state,
        previous,
        previous ? node->k + 1 : 0
    );
};

template<typename State>
bool IDDFS<State>::frontier_empty()
{
    if (m_frontier.empty())
        return false;

    if (m_deepen) {
        m_deepen = false;
        m_frontier.clear();
        m_visited.clear();
        m_depths.clear();
        m_frontier.push_back(make_node(m_initial_state, nullptr));
        m_cutoff += 1;
        return false;
    }

    return true;
}

template<typename State>
void IDDFS<State>::frontier_push(
    std::shared_ptr<typename Search<State, void>::Node> search_node
) {
    const Node *node = dynamic_cast<Node*>(search_node.get());

    // If the search depth has been reached then deepen.
    if (node->k > m_cutoff) {
        m_deepen = true;
        return;
    }

    // We should push the node to the frontier if this search node has a
    // shallower depth than the last search node for this state.
    auto previous_depth = m_depths.find(node->state);
    if (previous_depth != m_depths.end() && node->k >= previous_depth->second)
        return;

    m_depths[node->state] = node->k;

    return Search<State, void>::frontier_push(search_node);
}

template<typename State>
std::shared_ptr<typename Search<State, void>::Node>
IDDFS<State>::frontier_pop()
{
    auto node = std::move(m_frontier.back());
    m_frontier.pop_back();
    return std::move(node);
}
