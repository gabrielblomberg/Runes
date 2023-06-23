#pragma once

#include "util/Hexagon.h"
#include "util/Graph.h"

namespace Hexagon {

template<typename VertexType, typename EdgeType = void>
class HexagonalGraph : public Graph<Hexagon<int>, VertexType, EdgeType>
{
public:

    /**
     * @brief The type of graph the HexagonalGraph inherits from.
     */
    using GraphType = Graph<Hexagon<int>, VertexType, EdgeType>;

    /**
     * @brief Add a vertex. Also adds edges to all neighboring hexagons.
     * 
     * @param key The vertex in the graph to add.
     * @param value The data to store at the vertex.
     * 
     * @returns If the vertex was successfully added when the vertex was not
     * already occupied.
     */
    GraphType::Iterator AddVertex(const Hexagon<int> &key, const VertexType &&value);
};

template<typename VertexType, typename EdgeType>
HexagonalGraph<VertexType, EdgeType>::GraphType::Iterator
HexagonalGraph<VertexType, EdgeType>::AddVertex(
    const Hexagon<int> &key, const VertexType &&value
) {
    auto it = GraphType::AddVertex(key, std::move(value));
    return it;

    // Add neighbors.
}

} // namespace Hexagon
