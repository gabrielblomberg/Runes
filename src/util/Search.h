#include <unordered_set>
#include <queue>
#include <optional>
#include <vector>
#include <memory>

/**
 * @brief A node in the search tree that contains data pertaining to the
 * search strategy.
 * 
 * Examples of additional values stored in the search node include weights,
 * state transitions and costs.
 */
template<typename State>
struct SearchNode {

    SearchNode() = default;

    SearchNode(State state, SearchNode *parent)
        : state(state)
        , parent(parent)
    {}

    SearchNode(State state)
        : state(state)
        , parent(nullptr)
    {}

    /// The state of the node.
    State state;

    /// The parent search node visited to get to this node.
    SearchNode *parent;

    virtual ~SearchNode() = default;
};

// Hash of a search state for storing in a mapping.
template<>
template<typename State>
struct std::hash<SearchNode<State>>
{
    std::size_t operator()(const SearchNode<State> &node) const {
        return std::hash<State>{}(node.state);
    }
};

/**
 * @brief A search strategy.
 * 
 * @tparam Node the derived class of Search.
 * @tparam State The type of each
 * @tparam Compare The comparator that orders search nodes in the frontier. If
 * this is void, then a standard deque is used without insertion ordering.
 */
template<typename Node, typename State, typename Compare = void>
class Search {
public:

    /**
     * @brief Function that checks if a state is the goal state.
     */
    using Checker = std::function<bool(const State&)>;

    /**
     * @brief Function that returns all the subsequent states from a state.
     */
    using Successors = std::function<std::vector<State>(const State &)>;

    /// The type of the frontier, a deque or a priority queue.
    using Frontier = std::conditional_t<
        std::is_void_v<Compare>,
        std::deque<Node*>,
        std::priority_queue<Node*, std::vector<Node*>, std::function<bool(const Node*, const Node*)>>
    >;

    /// Hash type to hash a node pointer using the node hash function.
    struct Hash {
        std::size_t operator()(const Node* const &node) const {
            return std::hash<Node>{}(*node);
        }
    };

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
    std::optional<std::vector<std::unique_ptr<Node>>> perform();

    /**
     * @brief Get all the states visited during the search.
     */
    inline const std::unordered_set<Node> &visited() {
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
     * 
     * @param node The node to push onto the frontier.
     * @returns If the node was successfully pushed onto the frontier.
     */
    virtual bool frontier_push(Node *node);

    /**
     * @brief Pop the next search node from the frontier.
     * @returns The next node to check and search.
     */
    virtual Node *frontier_pop();

    /**
     * @brief Get all the successor search nodes from a state.
     */
    std::vector<std::unique_ptr<Node>>
    successors(Node *node);

    /// Function that returns the subsequent states of a state.
    Successors m_successors;

    /// Function that checks if a state is the goal state.
    Checker m_is_goal;

    // All the nodes in the search tree.
    std::vector<std::unique_ptr<Node>> m_nodes;

    /// The frontier of vertices to search through. A deque if not comparing
    /// search states or a priority queue if search states can be compared.
    Frontier m_frontier;

    // The set of visited search nodes that should not be revisited.
    std::unordered_set<Node*, Hash> m_visited;
};

template<typename Node, typename State, typename Compare>
Search<Node, State, Compare>::Search(
        State initial,
        Successors successors,
        Checker is_goal
  ) : m_successors(successors)
    , m_is_goal(is_goal)
    , m_nodes()
    , m_frontier()
    , m_visited()
{
    if constexpr (!std::is_void_v<Compare>) {
        m_frontier = Frontier(
            [](const Node *l, const Node *r){ return Compare{}(*l, *r); }
        );
    }

    m_nodes.push_back(std::make_unique<Node>(initial));

    if constexpr (std::is_void_v<Compare>) {
        m_frontier.push_back(m_nodes.back().get());
    }
    else {
        m_frontier.push(m_nodes.back().get());
    }
}

template<typename Node, typename State, typename Compare>
bool Search<Node, State, Compare>::frontier_empty()
{
    return m_frontier.empty();
}

template<typename Node, typename State, typename Compare>
bool Search<Node, State, Compare>::frontier_push(Node *node)
{
    if constexpr (std::is_void_v<Compare>) {
        m_frontier.push_back(node);
    }
    else {
        m_frontier.push(node);
    }

    return true;
};

template<typename Node, typename State, typename Compare>
Node *Search<Node, State, Compare>::frontier_pop()
{
    Node *node;

    if constexpr (std::is_void_v<Compare>) {
        node = m_frontier.front();
        m_frontier.pop_front();
    }
    else {
        node = m_frontier.top();
        m_frontier.pop();
    }

    return node;
}

template<typename Node, typename State, typename Compare>
std::vector<std::unique_ptr<Node>>
Search<Node, State, Compare>::successors(Node *node)
{
    // The states that are immediately reachable from state.
    std::vector<State> next_states = m_successors(node->state);

    // Construct the search nodes of each new state.
    std::vector<std::unique_ptr<Node>> nodes {};
    nodes.reserve(next_states.size());

    for (const State &next : next_states)
        nodes.push_back(std::make_unique<Node>(next, node));

    return std::move(nodes);
}

template<typename Node, typename State, typename Compare>
std::optional<std::vector<std::unique_ptr<Node>>>
Search<Node, State, Compare>::perform()
{
    while (!frontier_empty()) {
        Node *node = frontier_pop();

        // If its the not the goal then mark this node visited and add its
        // subsequent search nodes to the frontier.
        if (!m_is_goal(node->state)) {
            m_visited.emplace(node);

            for (auto &next : successors(node)) {
                auto pointer = next.get();

                // Only push search states that have not been visited.
                if (m_visited.find(pointer) == m_visited.end()) {

                    // If the node was pushed onto the frontier then maintain
                    // the unique pointer to it.
                    if (frontier_push(pointer))
                        m_nodes.push_back(std::move(next));
                }
            }

            continue;
        }

        // Found the goal, traverse backwards to generate the sequence of
        // search nodes visited to get to the goal
        std::vector<std::unique_ptr<Node>> trace {};
        for (; node; node = node->parent)
            trace.push_back(std::make_unique<Node>(*node));

        // For memory saving.
        m_nodes.clear();
        m_visited.clear();

        if constexpr (!std::is_void_v<Compare>) {
            m_frontier = Frontier(
                [](const Node *l, const Node *r){ return Compare{}(*l, *r); }
            );
        }
        else {
            m_frontier.clear();
        }

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
class BFS : public Search<SearchNode<State>, State, void>
{
    using Node = SearchNode<State>;
    using Search<Node, State, void>::Search;
};

/**
 * @brief Depth first search. Search tree traversed depth first.
 */
template<typename State>
class DFS : public Search<SearchNode<State>, State, void>
{
public:

    using Node = SearchNode<State>;
    using Search<Node, State, void>::Search;

private:

    /**
     * @brief Pop the newest search node added to the frontier.
     * @returns The newest node on the frontier.
     */
    Node *frontier_pop() override;

    using Search<Node, State, void>::m_frontier;
};

template<typename State>
typename DFS<State>::Node* DFS<State>::frontier_pop()
{
    auto node = m_frontier.back();
    m_frontier.pop_back();
    return node;
}

/**
 * @brief An IDDFS node keeps track of its height in the search tree.
 */
template<typename State>
struct IDDFSNode {

    IDDFSNode() = default;

    IDDFSNode(
            State state,
            IDDFSNode<State> *parent
      ) : state(state)
        , parent(parent)
        , k(parent->k + 1)
    {}

    IDDFSNode(State state)
        : state(state)
        , parent(nullptr)
        , k(0)
    {}

    State state;

    IDDFSNode<State> *parent;

    /// The height of the search node in the search tree.
    int k;
};

// Hash of a search state for storing in a mapping.
template<>
template<typename State>
struct std::hash<IDDFSNode<State>>
{
    std::size_t operator()(const IDDFSNode<State> &node) const {
        return std::hash<State>{}(node.state) ^ (std::hash<int>{}(node.k) << 1);
    }
};

/**
 * @brief Iteratively deepening depth first search. 
 */
template<typename State>
class IDDFS : public Search<IDDFSNode<State>, State, void>
{
public:

    using Node = IDDFSNode<State>;

    IDDFS(
            State initial,
            Search<Node, State, void>::Successors successor,
            Search<Node, State, void>::Checker is_goal
      ) : Search<Node, State, void>(initial, successor, is_goal)
        , m_initial_state(initial)
        , m_maximum_search_depth(3)
        , m_increase_search_depth(false)
    {}

private:

    /**
     * @brief Checks if the frontier is empty, and restarts the algorithm if
     * the cutoff is reached and the frontier is empty.
     * 
     * @returns If the frontier is empty.
     */
    bool frontier_empty() override;

    /**
     * @brief Pushes a node to the frontier only if it does not exceed the
     * search depth limit.
     * 
     * @param node The node to push to the frontier. 
     * @returns If the node does not exceed the maximum search depth.
     */
    bool frontier_push(Node *node) override;

    /**
     * @brief Pop the newest search node added to the frontier.
     * @returns The newest node on the frontier.
     */
    Node *frontier_pop() override;

    using Frontier = Search<Node, State, void>::Frontier;
    using Search<Node, State, void>::m_nodes;
    using Search<Node, State, void>::m_frontier;
    using Search<Node, State, void>::m_visited;

    /// Cache of the initial state for restarts.
    State m_initial_state;

    /// The cutoff of tree depth before restarting the search.
    int m_maximum_search_depth;

    /// Whether to restart the search.
    bool m_increase_search_depth;
};

template<typename State>
bool IDDFS<State>::frontier_empty()
{
    if (!m_frontier.empty())
        return false;

    // If the frontier is empty and the depth limit was reached, restart with
    // a deeper index.
    if (m_increase_search_depth) {
        m_increase_search_depth = false;

        // Clear old search tree.
        m_nodes.clear();
        m_visited.clear();
        m_frontier = Frontier();

        // Instantiate the new search tree.
        m_nodes.push_back(std::move(std::make_unique<Node>(m_initial_state)));
        m_frontier.push_back(m_nodes.back().get());

        m_maximum_search_depth += 1;
        return false;
    }

    return true;
}

template<typename State>
bool IDDFS<State>::frontier_push(Node *node)
{
    // If the search depth has been reached then increase the maximum tree depth
    // once the frontier becomes empty.
    if (node->k > m_maximum_search_depth) {
        m_increase_search_depth = true;
        return false;
    }

    return Search<Node, State, void>::frontier_push(node);
}

template<typename State>
typename IDDFS<State>::Node *IDDFS<State>::frontier_pop()
{
    auto node = m_frontier.back();
    m_frontier.pop_back();
    return node;
}

/**
 * @brief An IDDFS node keeps track of its height in the search tree.
 */
template<typename State>
struct UCSNode {

    UCSNode() = default;

    UCSNode(
            State state,
            UCSNode<State> *parent
      ) : state(state)
        , parent(parent)
        , cost(parent->cost + state.cost)
    {}

    UCSNode(State state)
        : state(state)
        , parent(nullptr)
        , cost(0)
    {}

    /**
     * @brief Ordering of UCS nodes is done base on the cost.
     */
    auto operator<=>(const UCSNode<State> &other) const {
        return cost <=> other.cost;
    };

    /// The state of the search node.
    State state;

    /// The parent search node.
    UCSNode<State> *parent;

    /// The total cost to get to this search node.
    int cost;
};

// Hash of a search state for storing in a mapping.
template<>
template<typename State>
struct std::hash<UCSNode<State>>
{
    std::size_t operator()(const UCSNode<State> &node) const {
        return std::hash<State>{}(node.state) ^ (std::hash<int>{}(node.cost) << 1);
    }
};

/**
 * @brief Uniform cost search.
 */
template<typename State, typename Compare = std::greater<>>
class UCS : public Search<UCSNode<State>, State, Compare>
{
public:
    using Node = UCSNode<State>;
    using Search<Node, State, Compare>::Search;
};
