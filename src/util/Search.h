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

    /**
     * @brief Create a new search node.
     * 
     * @param state The state of the search node.
     * @param parent The previous node from which this node was derived, or
     * nullptr if there were no parents.
     */
    SearchNode(State state, SearchNode *parent = nullptr)
        : state(state)
        , parent(parent)
    {}

    /// The state of the node.
    State state;

    /// Pointer to the parent node.
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
 * @tparam State The type of state being searched through.
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
        std::priority_queue<
            Node*,
            std::vector<Node*>,
            std::function<bool(const Node*, const Node*)>
        >
    >;

    /// Hash a node pointer using the node hash function. Used in the set of
    /// visited nodes.
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
     * @brief Make a node from a state and its parent.
     * 
     * Overridden to implement functionality when creating a node.
     * 
     * @param state The state of the new node.
     * @param parent The previous node from which this node was derived, or
     * nullptr if there were no parents.
     * 
     * @return The node.
     */
    virtual std::unique_ptr<Node> make_node(
        const State &state,
        Node *parent = nullptr
    ) = 0;

    /// TODO: The above should not be pure virtual, but should have the
    /// following default definition. Adding a default somehow hides all the
    /// overriden definitions.
    ///
    /// virtual std::unique_ptr<Node> make_node(
    ///     const State &state,
    ///     Node *parent = nullptr
    /// ) {
    ///     return std::make_unique<Node>(state, parent);
    /// };

    /**
     * @brief Clear all data structures for restarting the search or reducing
     * memory usage.
     */
    void clear();

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

    /// The initial state.
    State m_initial;

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
  ) : m_initial(initial)
    , m_successors(successors)
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
}

template<typename Node, typename State, typename Compare>
void Search<Node, State, Compare>::clear()
{
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
}

template<typename Node, typename State, typename Compare>
bool Search<Node, State, Compare>::frontier_empty()
{
    return m_frontier.empty();
}

template<typename Node, typename State, typename Compare>
bool Search<Node, State, Compare>::frontier_push(Node *node)
{
    // Only push to the frontier if the search node has not been visited yet.
    if (m_visited.find(node) != m_visited.end())
        return false;

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
std::optional<std::vector<std::unique_ptr<Node>>>
Search<Node, State, Compare>::perform()
{
    // Initialisation that cannot be done in the constructor.
    m_nodes.push_back(make_node(m_initial));

    if constexpr (std::is_void_v<Compare>) {
        m_frontier.push_back(m_nodes.back().get());
    }
    else {
        m_frontier.push(m_nodes.back().get());
    }

    // The current node being visited.
    Node *node = nullptr;

    // If the goal node was found.
    bool found = false;

    while (!frontier_empty()) {
        node = frontier_pop();

        // If its the not the goal then mark this node visited and add its
        // subsequent search nodes to the frontier.
        if ((found = m_is_goal(node->state)))
            break;

        m_visited.emplace(node);

        // Get the next node of each state.
        for (const State &next_state : m_successors(node->state)) {
            auto next_node = make_node(next_state, node);

            // If the node was pushed onto the frontier then maintain
            // the unique pointer to it.
            if (frontier_push(next_node.get()))
                m_nodes.push_back(std::move(next_node));
        }
    }

    if (!found)
        return std::nullopt;

    // Traverse search tree backwards to get visited states / transitions.
    std::vector<std::unique_ptr<Node>> trace {};
    for (; node; node = node->parent)
        trace.push_back(std::make_unique<Node>(*node));

    // Convert to start to finish ordering.
    std::reverse(trace.begin(), trace.end());

    clear();
    return std::move(trace);
}

/**
 * @brief Depth first search. Search tree traversed breadth first.
 */
template<typename State>
class BFS : public Search<SearchNode<State>, State, void>
{
    using Node = SearchNode<State>;
    using Search<Node, State, void>::Search;

private:

    /// @TODO: Remove in favour of virtual default.
    std::unique_ptr<Node> make_node(
        const State &state,
        Node *parent = nullptr
    ) override;
};

/// @TODO: Remove in favour of virtual default.
template<typename State>
std::unique_ptr<SearchNode<State>> BFS<State>::make_node(
    const State &state,
    Node *parent
) {
    return std::make_unique<Node>(state, parent);
}

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

    /// @TODO: Remove in favour of virtual default.
    std::unique_ptr<Node> make_node(
        const State &state,
        Node *parent = nullptr
    ) override;

    /**
     * @brief Pop the newest search node added to the frontier.
     * @returns The newest node on the frontier.
     */
    Node *frontier_pop() override;

    using Search<Node, State, void>::m_frontier;
};

/// @TODO: Remove in favour of virtual default.
template<typename State>
std::unique_ptr<SearchNode<State>> DFS<State>::make_node(
    const State &state,
    Node *parent
) {
    return std::make_unique<Node>(state, parent);
}

template<typename State>
typename DFS<State>::Node* DFS<State>::frontier_pop()
{
    // Pop the most recently pushed search node.
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

    /**
     * @brief Create a new IDDFS search node.
     * 
     * The search tree depth of this node is the parent depth + 1 or 0 if there
     * was no parent.
     * 
     * @param state The state of the search node.
     * @param parent The previous node from which this node was derived, or
     * nullptr if there were no parents.
     */
    IDDFSNode(
            State state,
            IDDFSNode<State> *parent = nullptr
      ) : state(state)
        , parent(parent)
        , k(parent ? parent->k + 1 : 0)
    {}

    /**
     * @brief AN IDDFS node is equal if the states are equal and the depth of
     * the search nodes are equal.
     */
    auto operator==(const IDDFSNode<State> &other) {
        return state == other.state && k == other.k;
    }

    /// The state of the search node.
    State state;

    /// Pointer to the parent node.
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

    /**
     * @brief Create a new IDDFS search.
     * 
     * @param initial The initial state.
     * @param successor A function that takes a state and returns all subsequent
     * states.
     * @param is_goal A function that takes a state and returns if that state is
     * the goal state.
     */
    IDDFS(
            State initial,
            Search<Node, State, void>::Successors successor,
            Search<Node, State, void>::Checker is_goal
      ) : Search<Node, State, void>(initial, successor, is_goal)
        , m_maximum_search_depth(3)
        , m_increase_search_depth(false)
    {}

private:

    using Search<Node, State, void>::make_node;
    using Search<Node, State, void>::clear;

    /// @TODO: Remove in favour of virtual default.
    std::unique_ptr<Node> make_node(
        const State &state,
        Node *parent = nullptr
    ) override;

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

    using Search<Node, State, void>::m_initial;
    using Search<Node, State, void>::m_nodes;
    using Search<Node, State, void>::m_frontier;
    using Search<Node, State, void>::m_visited;

    /// The cutoff of tree depth before restarting the search.
    int m_maximum_search_depth;

    /// Whether to restart the search.
    bool m_increase_search_depth;
};

/// @todo: Remove in favour of virtual default
template<typename State>
std::unique_ptr<IDDFSNode<State>> IDDFS<State>::make_node(
    const State &state,
    Node *parent
) {
    return std::make_unique<Node>(state, parent);
}

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
        clear();

        // Instantiate the new search tree.
        m_nodes.push_back(make_node(m_initial));
        m_frontier.push_back(m_nodes.back().get());

        m_maximum_search_depth += 1;
        return false;
    }

    return true;
}

template<typename State>
bool IDDFS<State>::frontier_push(Node *node)
{
    if (m_visited.find(node) != m_visited.end())
        return false;

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
template<typename State, typename Cost>
struct UCSNode {

    UCSNode() = default;

    UCSNode(
            State state,
            UCSNode<State, Cost> *parent = nullptr
      ) : state(state)
        , parent(parent)
        , cost(parent ? parent->cost + state.cost : 0)
    {}

    /**
     * @brief Ordering of UCS nodes is done base on the cost.
     */
    auto operator<=>(const UCSNode<State, Cost> &other) const {
        return cost <=> other.cost;
    };

    /**
     * @brief Search nodes are equivalent if they have the same state in UCS.
     */
    auto operator==(const UCSNode<State, Cost> &other) const {
        return state == other.state;
    };

    /// The state of the search node.
    State state;

    /// The parent search node.
    UCSNode<State, Cost> *parent;

    /// The total cost to get to this search node.
    Cost cost;
};

// Hash of a search state for storing in a mapping.
template<>
template<typename State, typename Cost>
struct std::hash<UCSNode<State, Cost>>
{
    std::size_t operator()(const UCSNode<State, Cost> &node) const {
        return std::hash<State>{}(node.state) ^ (std::hash<Cost>{}(node.cost) << 1);
    }
};

/**
 * @brief Uniform cost search.
 */
template<typename State, typename Cost = decltype(State::cost), typename Compare = std::greater<>>
class UCS : public Search<UCSNode<State, Cost>, State, Compare>
{
public:
    using Node = UCSNode<State, Cost>;
    using Search<Node, State, Compare>::Search;

private:

    /// @todo: Remove in favour of virtual default.
    std::unique_ptr<Node> make_node(
        const State &state,
        Node *parent = nullptr
    ) override;
};

/// @todo: Remove in favour of virtual default
template<typename State, typename Cost, typename Compare>
std::unique_ptr<UCSNode<State, Cost>> UCS<State, Cost, Compare>::make_node(
    const State &state,
    Node *parent
) {
    return std::make_unique<Node>(state, parent);
}

template<typename State, typename Cost>
struct AStarNode {

    AStarNode() = default;

    /**
     * @brief Create a new AStar search node.
     * 
     * @param state The state of the search node.
     * @param remaining The remaining cost from the state.
     * @param parent The previous node from which this node was derived, or
     * nullptr if there were no parents.
     */
    AStarNode(
            State state,
            Cost remaining,
            AStarNode<State, Cost> *parent = nullptr
      ) : state(state)
        , parent(parent)
        , cost(parent ? parent->cost + state.cost : 0)
    {
        total = cost + remaining;
    }

    /**
     * @brief Ordering of UCS nodes is done base on the cost.
     */
    auto operator<=>(const AStarNode<State, Cost> &other) const {
        return total <=> other.total;
    };

    /**
     * @brief AStar states are equal if their states are equal.
     */
    auto operator==(const AStarNode<State, Cost> &other) {
        return state == other.state;
    }

    /// The state of the search node.
    State state;

    /// The parent search node.
    AStarNode<State, Cost> *parent;

    /// The cumulative total cost so far.
    Cost cost;

    /// The predicted remaining cost from this state to the goal.
    Cost total;
};

// Hash of a search state for storing in a mapping.
template<>
template<typename State, typename Cost>
struct std::hash<AStarNode<State, Cost>>
{
    std::size_t operator()(const AStarNode<State, Cost> &node) const {
        return (
            std::hash<State>{}(node.state) ^
            std::hash<Cost>{}(node.cost) << 1 ^
            std::hash<Cost>{}(node.total) << 2
        );
    }
};

/**
 * @brief A* search.
 */
template<typename State, typename Cost = decltype(State::cost), typename Compare = std::greater<>>
class AStar : public Search<AStarNode<State, Cost>, State, Compare>
{
public:

    using Node = AStarNode<State, Cost>;

    /// The heuristic function that takes a state and returns the estimated
    /// cost to the goal
    using Heuristic = std::function<Cost(const State&)>;

    /**
     * @brief Construct a new AStar object
     * 
     * @param initial The initial state.
     * @param successor A function that takes a state and returns all subsequent
     * states.
     * @param is_goal A function that takes a state and returns if that state is
     * the goal state.
     * @param heuristic A function that predicts the remaining cost to the goal
     * from a given state.
     */
    AStar(
            State initial,
            Search<Node, State, Compare>::Successors successor,
            Search<Node, State, Compare>::Checker is_goal,
            Heuristic heuristic
      ) : Search<Node, State, Compare>(initial, successor, is_goal)
        , m_heuristic(heuristic)
    {}

protected:

    /**
     * @brief Make a node using the heuristic of the state of the node state.
     * 
     * @param state The state of the node.
     * @param parent The previous node from which this node was derived, or
     * nullptr if there were no parents.
     * 
     * @returns The node.
     */
    std::unique_ptr<Node> make_node(const State &state, Node *parent) override;

    /**
     * @brief Pushes a node only if the state has not been visited yet or the
     * search cost for that node is less.
     * 
     * @param node The node to push to the frontier.
     * @returns If the node was pushed to the frontier successfully.
     */
    bool frontier_push(Node *node) override;

    using Search<Node, State, Compare>::m_visited;

    /// A function that predicts the remaining cost to the goal from a given
    /// state.
    Heuristic m_heuristic;
};

template<typename State, typename Cost, typename Compare>
std::unique_ptr<typename AStar<State, Cost, Compare>::Node>
AStar<State, Cost, Compare>::make_node(const State &state, Node *parent)
{
    return std::make_unique<Node>(state, m_heuristic(state), parent);
};

template<typename State, typename Cost, typename Compare>
bool AStar<State, Cost, Compare>::frontier_push(Node *node)
{
    // If the state has not been visited then push to the queue.
    auto previous = m_visited.find(node);
    if (previous == m_visited.end())
        return Search<Node, State, Compare>::frontier_push(node);

    // Only push nodes that have better total costs than the same previously
    // visited state.
    if ((*previous)->total <= node->total)
        return false;
    
    return Search<Node, State, Compare>::frontier_push(node);
}
