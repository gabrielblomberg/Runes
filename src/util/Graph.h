#pragma once

// Node Edge Graph

/**
 * @brief A hexagonal graph.
 * 
 * @tparam T The type of the key used to index the graph.
 * @tparam Vertex The type stored at each hexagonal location.
 * @tparam Edge The type stored at each edge.
 */
template<typename T, typename VertexType, typename EdgeType = void>
class Graph
{
public:

    // Forward declare NodeType for EdgeType::vertex;
    struct Vertex;

    /**
     * @brief An edge without any additional data.
     */
    struct BareEdge {

        /// Pointer to the vertex on the other side of the edge.
        std::weak_ptr<Vertex> vertex;
    };

    /**
     * @brief An edge with additional data.
     */
    struct FullEdge {

        /// Data contained at this edge.
        EdgeType data;

        /// Pointer to the vertex on the other side of the edge.
        std::weak_ptr<Vertex> vertex;
    };

    /**
     * @brief An edge stores data associated to the edge, and a weak pointer to
     * the node on the other side of the edge.
     */
    using Edge = std::conditional_t<std::is_void_v<EdgeType>, BareEdge, FullEdge>;

    /**
     * @brief The type of a node stored in the map, a vertex and a sequence of
     * edges.
     */
    struct Vertex {

        /// Data contained in this vertex.
        VertexType data;

        /// The edges that begin at this vertex.
        std::map<T, std::shared_ptr<Edge>> edges;
    };

    /// The map stores the nodes at each vertex.
    using Map = std::unordered_map<T, std::shared_ptr<Vertex>>;

    /**
     * @brief An iterator over the graph.
     * 
     * This iterator doesn't conform to standard c++ iterators since it has
     * no types and no operator++(), and so is treated as just another class
     * denoted by the capitalisation.
     */
    class Iterator
    {
    public:

        /**
         * @brief Move the iterator to another vertex. The iterator becomes
         * end() if no outgoing edge from this vertex to that one exists.
         * 
         * @param vertex The vertex to move to. 
         */
        void Move(const T &&to);

        /**
         * @brief Get the previously traversed edge.
         * @return A pointer to the previously traversed edge.
         */
        inline const Edge &GetEdge() const {
            return *m_edge.lock();
        };

        /**
         * @brief Get the previously traversed edge.
         * @return A pointer to the previously traversed edge.
         */
        inline const Vertex &GetVertex() const {
            return *m_vertex.lock();
        };

        /**
         * @brief Check if the iterator points to a nonexistant vertex.
         * @returns If this iterator points to a non existant vertex.
         */
        inline bool HasEdge() const {
            return m_edge.expired();
        }

        /**
         * @brief Check if the iterator points to a nonexistant vertex.
         * @returns If this iterator points to a non existant vertex.
         */
        inline bool HasVertex() const {
            return m_vertex.expired();
        }

    private:

        friend class Graph;

        /**
         * @brief Create a new iterator for the graph, starting 
         * 
         * @param vertex A shared pointer to the vertex to iterate at.
         * @param edge A shared pointer to the edge to iterate at.
         */
        Iterator(std::shared_ptr<Vertex> vertex, std::shared_ptr<Edge> edge);

        /// Vertex being pointed to.
        std::weak_ptr<Vertex> m_vertex;

        /// Previous edge being pointed to.
        std::weak_ptr<Edge> m_edge;
    };

    Graph();

    // Mutation.

    /**
     * @brief Add a vertex.
     * 
     * @param key The vertex in the graph to add.
     * @param value The data to store at the vertex.
     * 
     * @returns If the vertex was successfully added when the vertex was not
     * already occupied.
     */
    Iterator AddVertex(const T &key, const VertexType &&value);

    /**
     * @brief Remove a vertex.
     * 
     * @param key The key of the vertex to remove.
     * @returns If the vertex was successfully deleted when a vertex exists
     * to remove.
     */
    bool RemoveVertex(const T &&key);

    /**
     * @brief Add an edge between two verticies.
     * 
     * @param first The first vertex in the edge.
     * @param second The second vertex in the edge.
     * @param edge The data to store at the edge.
     * 
     * @returns If the edge was successfully added when both verticies exist and
     * the edge was not already occupied.
     */
    template<typename E>
    std::enable_if_t<std::is_same_v<E, EdgeType> && !std::is_void_v<E>, Iterator>
    AddEdge(const T &&first, const T &&second, const Edge &&edge);

    /**
     * @brief Add an edge between two verticies.
     * 
     * @param first The first vertex in the edge.
     * @param second The second vertex in the edge.
     * 
     * @returns If the edge was successfully added when both hexagons exist and
     * the edge was not already occupied.
     */
    template<typename E>
    std::enable_if_t<std::is_same_v<E, EdgeType> && std::is_void_v<E>, Iterator>
    AddEdge(const T &&first, const T &&second);

    /**
     * @brief Removes an edge between two verticies if it exists.
     * 
     * @param first The first vertex on the edge.
     * @param second The second vertex on the edge.
     * 
     * @returns If the edge was successfully removed when the first vertex
     * and the edge exists.
     */
    bool RemoveEdge(const T &&first, const T &&second);

    /**
     * @brief Get an iterator to a vertex.
     * 
     * @param key The key of the vertex to find.
     * @returns An iterator to the vertex..
     */
    Iterator At(const T &&key);

    /**
     * @brief Get an iterator to an edge.
     * 
     * @param first The first vertex of the edge.
     * @param second The second vertex of the edge.
     * 
     * @returns An iterator to the edge at the second vertex.
     */
    Iterator At(const T &&first, const T &&second);

    // Querying.

    /**
     * @brief Get the total number of verticies.
     * @returns The total number of verticies.
     */
    std::size_t TotalVerticies() const;

    /**
     * @brief Get the total number of edges.
     * @return The total number of edges.
     */
    std::size_t TotalEdges() const;

    /**
     * @brief Get the number of outgoing edges from a vertex.
     * 
     * @param key The key of the vertex to get outgoing edges from.
     * @return The number of outgoing edges.
     */
    std::size_t OutDegree(const T &&key);

    /**
     * @brief Get the number of incoming edges from a vertex.
     * 
     * @param key The key of the vertex to get incoming edges from.
     * @return The number of incoming edges.
     */
    std::size_t InDegree(const T &&key);

private:

    /// The graph data structure.
    Map m_graph;
};

template<typename T, typename VertexType, typename EdgeType>
Graph<T, VertexType, EdgeType>::Iterator::Iterator(
        std::shared_ptr<Vertex> vertex,
        std::shared_ptr<Edge> edge
  ) : m_vertex(vertex)
    , m_edge(edge)
{}

template<typename T, typename VertexType, typename EdgeType>
void Graph<T, VertexType, EdgeType>::Iterator::Move(const T &&to)
 {
    auto next = m_vertex->edges.find(to);

    if (next == m_vertex->edges.end()) {
        m_vertex.reset();
        m_edge.reset();
        return;
    }

    m_edge = next->second;

    if (m_edge->vertex.expired()) {
        m_vertex.reset();
        return;
    }

    m_vertex = m_edge.lock();
}

template<typename T, typename VertexType, typename EdgeType>
Graph<T, VertexType, EdgeType>::Graph()
    : m_graph()
{}

template<typename T, typename VertexType, typename EdgeType>
Graph<T, VertexType, EdgeType>::Iterator
Graph<T, VertexType, EdgeType>::AddVertex(
    const T &key,
    const VertexType &&value
) {
    auto [it, success] = m_graph.emplace(
        key,
        std::make_shared<Vertex>(
            value, std::map<T, std::shared_ptr<Edge>>()
        )
    );

    if (success)
        return Iterator(it->second, nullptr);

    return Iterator(nullptr, nullptr);
}

template<typename T, typename VertexType, typename EdgeType>
Graph<T, VertexType, EdgeType>::Iterator
Graph<T, VertexType, EdgeType>::At(const T &&key)
{
    // Find the first vertex.
    auto it = m_graph.find(key);
    if (it == m_graph.end())
        return Iterator(nullptr, nullptr);

    return Iterator(it->second, nullptr);
}
