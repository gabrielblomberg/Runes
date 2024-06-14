#pragma once

#include <array>
#include <type_traits>
#include <tuple>
#include <ranges>
#include <unordered_map>
#include <memory>

/**
 * @brief A hexagonal graph.
 * 
 * @tparam T The type of the key used to index the graph.
 * @tparam VertexType The type stored at each hexagonal location.
 * @tparam EdgeType The type stored at each edge.
 */
template<typename T, typename VertexType = void, typename EdgeType = void>
class Graph
{
public:

    // Forward declare types for conditional templating.
    struct UnweightedVertex;
    struct WeightedVertex;
    struct UnweightedEdge;
    struct WeightedEdge;

    /**
     * @brief The type representing a vertex on a graph.
     */
    using Vertex = std::conditional_t<std::is_void_v<VertexType>, UnweightedVertex, WeightedVertex>;

    /**
     * @brief The type representing an edge between two vertices.
     */
    using Edge = std::conditional_t<std::is_void_v<EdgeType>, UnweightedEdge, WeightedEdge>;

    /**
     * @brief The type containing vertex instances, each containing a mapping of
     * edges.
     */
    using Map = std::unordered_map<T, std::shared_ptr<Vertex>>;

    /**
     * @brief A vertex defined only by its edges and the key to the vertex.
     */
    struct UnweightedVertex {

        /// The edges that begin at this vertex.
        std::unordered_map<T, std::shared_ptr<Edge>> edges;
    };

    /**
     * @brief A vertex that contains some type.
     */
    struct WeightedVertex {

        /// The edges that begin at this vertex.
        std::unordered_map<T, std::shared_ptr<Edge>> edges;

        /// Data contained in this vertex.
        VertexType data;
    };

    /**
     * @brief An edge without any additional data.
     */
    struct UnweightedEdge {

        /// Pointer to the vertex on the other side of the edge.
        std::weak_ptr<Vertex> vertex;
    };

    /**
     * @brief An edge with additional data.
     */
    struct WeightedEdge {

        /// Pointer to the vertex on the other side of the edge.
        std::weak_ptr<Vertex> vertex;

        /// Data contained at this edge.
        EdgeType data;
    };

    /**
     * @brief An iterator over the graph.
     * 
     * This iterator doesn't conform to standard c++ iterators since it has
     * no types and no operator++(), and so is treated as just another class.
     */
    class Iterator
    {
    public:

        /**
         * @brief move the iterator to another vertex. The iterator becomes
         * end() if no outgoing edge from this vertex to that one exists.
         * 
         * @param vertex The vertex to move to. 
         */
        void move(const T &to);

        /**
         * @brief Get the key of vertex, if the vertex exists.
         * @return The key of the vertex.
         */
        inline T key() const {
            return m_key;
        }

        /**
         * @brief Get the previously traversed edge.
         * @return A pointer to the previously traversed edge.
         */
        inline const Edge &edge() const {
            return *m_edge.lock();
        };

        /**
         * @brief Get the previously traversed edge.
         * @return A pointer to the previously traversed edge.
         */
        inline Vertex &vertex() const {
            return *m_vertex.lock();
        };

        /**
         * @brief Check if the iterator has a key.
         * @returns If the iterator has a key.
         */
        inline bool has_key() const {
            return has_vertex();
        }

        /**
         * @brief Check if the iterator points to a nonexistant vertex.
         * @returns If this iterator points to a non existant vertex.
         */
        inline bool has_edge() const {
            return !m_edge.expired();
        }

        /**
         * @brief Check if the iterator points to a nonexistant vertex.
         * @returns If this iterator points to a non existant vertex.
         */
        inline bool has_vertex() const {
            return !m_vertex.expired();
        }

        /**
         * @brief Check if two iterators are equal.
         * 
         * @param other The other iterator.
         * @returns If the two iterators are equal.
         */
        inline bool operator==(const Iterator &other) const {
            bool equal = has_vertex();
            equal &= other.has_vertex();
            equal &= m_key == other.m_key;
            equal &= m_vertex.lock() == other.m_vertex.lock();
            return equal;
            // return (
            //     has_vertex() &&
            //     other.has_vertex() &&
            //     m_key == other.m_key &&
            //     m_vertex.lock() == other.m_vertex.lock()
            // );
        }

    private:

        friend class Graph;
    
        /**
         * @brief Create a new iterator for the graph, starting 
         * 
         * @param key The key of the vertex.
         * @param vertex The vertex being pointed to.
         * @param edge The edge previously traversed edge to get to the vertex.
         */
        Iterator(T key, std::shared_ptr<Vertex> vertex, std::shared_ptr<Edge> edge);

        // The key of the vertex, if it is valid.
        T m_key;

        /// Vertex being pointed to.
        std::weak_ptr<Vertex> m_vertex;

        /// Previous edge being pointed to.
        std::weak_ptr<Edge> m_edge;
    };

    Graph();

    // Access

    /**
     * @brief Get an iterator to a vertex.
     * 
     * @param key The key of the vertex to find.
     * @returns An iterator to the vertex..
     */
    Iterator at(const T &key);

    /**
     * @brief Get an iterator to an edge.
     * 
     * @param first The first vertex of the edge.
     * @param second The second vertex of the edge.
     * 
     * @returns An iterator to the edge at the second vertex.
     */
    Iterator at(const T &first, const T &second);

    /**
     * @brief Get an iterator to a vertex in the graph.
     * 
     * The iterator may point to any vertex.
     * 
     * To iterate over the whole graph, use edges() and verticies().
     * 
     * @returns An iterator to a vertex in the graph.
     */
    inline Iterator begin() {
        if (m_graph.empty())
            return end();
        return at(m_graph.begin()->first);
    }

    /**
     * @brief The placeholder iterator for the end of the graph.
     * 
     * @return Iterator 
     */
    inline Iterator end() {
        return Iterator(T{}, nullptr, nullptr);
    }

    // Mutation.

    /**
     * @brief Add a vertex.
     * 
     * For vertices that contain an element, the element must be default
     * constructable.
     * 
     * @param key The vertex in the graph to add.
     * @returns An iterator to the new vertex or end() if it failed to add, and
     * a flag if the vertex was successfully added.
     */
    std::pair<Iterator, bool> add_vertex(const T &key);

    /**
     * @brief Add a vertex that contains an element.
     * 
     * @param key The vertex in the graph to add.
     * @param value The data to store at the vertex.
     * 
     * @returns An iterator to the new vertex or end() if it failed to add, and
     * a flag if the vertex was successfully added.
     */
    template<typename V>
    std::enable_if_t<
        std::is_same_v<V, VertexType> && !std::is_void_v<V>,
        std::pair<Iterator, bool>
    >
    add_vertex(const T &key, const V &value);

    /**
     * @brief Remove a vertex.
     * 
     * @param key The key of the vertex to remove.
     * @returns If the vertex was successfully deleted when a vertex exists
     * to remove.
     */
    bool remove_vertex(const T &key);

    /**
     * @brief Add an edge between two vertices.
     * 
     * @param first The first vertex in the edge.
     * @param second The second vertex in the edge.
     * 
     * @returns If the edge was successfully added when both hexagons exist and
     * the edge was not already occupied.
     */
    std::pair<Iterator, bool> add_edge(const T &first, const T &second);

    /**
     * @brief Add an edge between two vertices.
     * 
     * An edge was successfully added when both vertices exist and
     * the edge was not already occupied.
     * 
     * @param first The first vertex in the edge.
     * @param second The second vertex in the edge.
     * @param edge The data to store at the edge.
     * 
     * @returns An iterator to the edge if successful or end() on failure.
     */
    template<typename E>
    std::enable_if_t<
        std::is_same_v<E, EdgeType> && !std::is_void_v<E>,
        std::pair<Iterator, bool>
    >
    add_edge(const T &first, const T &second, const E &edge);

    /**
     * @brief Removes an edge between two vertices if it exists.
     * 
     * @param first The first vertex on the edge.
     * @param second The second vertex on the edge.
     * 
     * @returns If the edge was successfully removed when the first vertex
     * and the edge exists.
     */
    bool remove_edge(const T &first, const T &second);

    // Querying.

    /**
     * @brief Get all the keys of the graph.
     */
    inline auto keys() {
        return m_graph |
            std::views::transform(
                [](std::pair<const T, std::shared_ptr<Vertex>> &pair) {
                    return pair.first;
                }
            );
    }

    /**
     * @brief Get all the (key, vertex) of the graph.
     */
    inline auto vertices() {
        return m_graph |
            std::views::transform(
                [](std::pair<const T, std::shared_ptr<Vertex>> &pair) {
                    return pair;
                }
            );
    }

    /**
     * @brief Get all the (key1, key2, edge) in the graph.
     */
    inline auto edges() {
        // Oh my goodness. This should be using std::views::zip.
        using TupleType = std::tuple<T, T, std::shared_ptr<Edge>>;
        return m_graph |
            std::views::transform(
                [](std::pair<const T, std::shared_ptr<Vertex>> &pair) {
                    return pair.second->edges | std::views::transform(
                        [&pair](std::pair<const T, std::shared_ptr<Edge>> &edge) {
                            return std::array<TupleType, 1>{
                                std::make_tuple(pair.first, edge.first, edge.second)
                            };
                        }
                    ) | std::views::join;
                }
            ) |
            std::views::join;
    }

    /**
     * @brief Check if the map contains a vertex at the provided key.
     * 
     * @param key The key of the vertex.
     * @returns If the graph contains a vertex at the key.
     */
    inline bool contains_vertex(T key) const {
        return m_graph.contains(key);
    }

    /**
     * @brief Check if the map contains an edge between the provided keys.
     * 
     * @param first The first vertex of the edge.
     * @param second The second vertex of the edge.
     * 
     * @returns If an edge exists between the verticies.
     */
    inline bool contains_edge(T first, T second) const {
        auto it = m_graph.find(first);
        if (it == m_graph.end())
            return false;
        return it->edges.contains(second);
    }

    /**
     * @brief Get the total number of vertices.
     * @returns The total number of vertices.
     */
    inline std::size_t total_verticies() const {
        return m_graph.size();
    }

    /**
     * @brief Get the total number of edges.
     * @return The total number of edges.
     */
    inline std::size_t total_edges() const {
        std::size_t n = 0;
        for (const auto &[key, vertex] : m_graph)
            n += vertex->edges.size();
        return n;
    }

    /**
     * @brief Get the number of outgoing edges from a vertex.
     * 
     * Complexity O(1).
     * 
     * @param key The key of the vertex to get outgoing edges from.
     * @return The number of outgoing edges.
     */
    inline std::size_t out_degree(const T &key) {
        auto it = m_graph.find(key);
        if (it == m_graph.end())
            return 0;
        return it->second->edges.size();
    }

    /**
     * @brief Get the number of incoming edges from a vertex.
     * 
     * Complexity O(n + m). Could precalculate on every added edge.
     * 
     * @param key The key of the vertex to get incoming edges from.
     * @return The number of incoming edges.
     */
    inline std::size_t in_degree(const T &key) {
        auto it = m_graph.find(key);
        if (it == m_graph.end())
            return 0;

        std::size_t n = 0;

        for (const auto &[k1, vertex] : m_graph)
            for (const auto [k2, edge] : *vertex.edges)
                n += edge->contains(key);

        return n;
    }

private:

    /// The graph data structure.
    Map m_graph;
};

template<typename T, typename VertexType, typename EdgeType>
Graph<T, VertexType, EdgeType>::Iterator::Iterator(
        T key,
        std::shared_ptr<Vertex> vertex,
        std::shared_ptr<Edge> edge
  ) : m_key(key)
    , m_vertex(vertex)
    , m_edge(edge)
{}

template<typename T, typename VertexType, typename EdgeType>
void Graph<T, VertexType, EdgeType>::Iterator::move(const T &to)
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
Graph<T, VertexType, EdgeType>::at(const T &key)
{
    // Find the first vertex.
    auto it = m_graph.find(key);
    if (it == m_graph.end())
        return end();

    return Iterator(key, it->second, nullptr);
}

template<typename T, typename VertexType, typename EdgeType>
Graph<T, VertexType, EdgeType>::Iterator
Graph<T, VertexType, EdgeType>::at(const T &from, const T &to)
{
    // Find the first vertex.
    auto v1 = m_graph.find(from);
    if (v1 == m_graph.end())
        return end();

    auto &edges = v1->second->edges;

    // Get the edge to the second vertex.
    auto edge = edges.find(to);
    if (edge == edges.end())
        return end();

    return Iterator(to, edge->vertex, edge);
}

template<typename T, typename VertexType, typename EdgeType>
std::pair<typename Graph<T, VertexType, EdgeType>::Iterator, bool>
Graph<T, VertexType, EdgeType>::add_vertex(const T &key)
{
    bool success = false;
    decltype(Map::iterator) it;

    if constexpr (std::is_void_v<VertexType>) {
        std::tie(it, success) = m_graph.emplace(
            key,
            std::make_shared<Vertex>(
                std::unordered_map<T, std::shared_ptr<Edge>>()
            )
        );
    }
    else {
        std::tie(it, success) = m_graph.emplace(
            key,
            std::make_shared<Vertex>(
                std::unordered_map<T, std::shared_ptr<Edge>>(), VertexType()
            )
        );
    }

    if (success)
        return std::make_pair(Iterator(key, it->second, nullptr), true);

    return std::make_pair(end(), false);
}

template<typename T, typename VertexType, typename EdgeType>
template<typename V>
std::enable_if_t<
    std::is_same_v<V, VertexType> && !std::is_void_v<V>,
    std::pair<typename Graph<T, VertexType, EdgeType>::Iterator, bool>
>
Graph<T, VertexType, EdgeType>::add_vertex(const T &key, const V &value)
{
    // Add the vertex to the graph.
    auto [it, success] = m_graph.emplace(
        key,
        std::make_shared<Vertex>(
            std::unordered_map<T, std::shared_ptr<Edge>>(), value
        )
    );

    if (success)
        return std::make_pair(Iterator(key, it->second, nullptr), true);

    return std::make_pair(end(), false);
}

template<typename T, typename VertexType, typename EdgeType>
std::pair<typename Graph<T, VertexType, EdgeType>::Iterator, bool>
Graph<T, VertexType, EdgeType>::add_edge(const T &first, const T &second)
{
    auto v1 = m_graph.find(first);
    if (v1 == m_graph.end())
        return std::make_pair(end(), false);

    auto v2 = m_graph.find(second);
    if (v2 == m_graph.end())
        return std::make_pair(end(), false);

    using UnweightedEdge = Graph<T, VertexType, EdgeType>::UnweightedEdge;
    using WeightedEdge = Graph<T, VertexType, EdgeType>::WeightedEdge;

    if constexpr (std::is_void_v<EdgeType>) {
        v1->second->edges.emplace(
            second,
            std::make_shared<UnweightedEdge>(v2->second)
        );
    }
    else {
        v1->second->edges.emplace(
            second,
            std::make_shared<WeightedEdge>(v2->second, EdgeType())
        ); 
    }

    return std::make_pair(end(), true);
}

template<typename T, typename VertexType, typename EdgeType>
template<typename E>
std::enable_if_t<
    std::is_same_v<E, EdgeType> && !std::is_void_v<E>,
    std::pair<typename Graph<T, VertexType, EdgeType>::Iterator, bool>
>
Graph<T, VertexType, EdgeType>::add_edge(
    const T &first,
    const T &second,
    const E &data
) {
    // Find the first vertex.
    auto a = m_graph.find(first);
    if (a == m_graph.end())
        return std::make_pair(end(), false);

    // Find the second vertex.
    auto b = m_graph.find(second);
    if (b == m_graph.end())
        return std::make_pair(end(), false);

    // Create the edge to add to the first vertex edges.
    auto edge = std::make_shared<Graph<T, VertexType, EdgeType>::WeightedEdge>(
        data, b->second
    );

    // Add the edge to the first vertex edges.
    auto [_, success] = a->second.emplace(edge);
    if (!success)
        return std::make_pair(end(), false);

    return std::make_pair(Iterator(second, b->second, edge), true);
}

template<typename T, typename VertexType, typename EdgeType>
bool Graph<T, VertexType, EdgeType>::remove_vertex(const T &key)
{
    auto vertex = m_graph.find(key);
    if (vertex == m_graph.end())
        return false;

    // Erase the vertex.
    m_graph.erase(vertex);

    // Erase all edges to the vertex.
    for (auto [k, v] : m_graph) {
        auto it = v->edges.find(key);
        if (it != v->edges.end())
            v->edges.erase(it);
    }

    return true;
}

template<typename T, typename VertexType, typename EdgeType>
bool Graph<T, VertexType, EdgeType>::remove_edge(
    const T &first,
    const T &second
) {
    auto a = m_graph.find(first);
    if (a == m_graph.end())
        return false;

    auto &edges = a->second->edges;

    auto b = edges.find(second);
    if (b == edges.end())
        return false;

    edges.erase(b);
    return true;
}
