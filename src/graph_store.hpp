#ifndef GRAPHSTORE_GRAPH_STORE_HPP
#define GRAPHSTORE_GRAPH_STORE_HPP

#include "graph_util.hpp"
#include <vector>
#include <cstdint>
#include <string>
#include <optional>


namespace graph_store {

///
/// @brief A class that stores directed unweighted graph. Provides functionality to add labels to the vertices and find
/// the shortest path between 2 vertices
    class GraphStore {
    public:

        GraphStore();

        /// @brief Creates a new vertex in Graph Store
        ///
        /// @return Unique ID of the created vertex
        ///
        std::uint64_t CreateVertex();


        /// @brief Creates directed edge between passed vertices
        ///
        /// @param src_vertex_id The origin vertex ID of the edge
        /// @param dst_vertex_id The destination vertex ID of the edge
        /// @return Returns whether edge is successfully created
        ///
        bool CreateEdge(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id);

        /// @brief Adds label to the passed vertex, method has no effect if vertex already has the passed label set
        ///
        /// @param vertex_id The ID of the vertex to process
        /// @param label The label will be added to the vertex
        /// @return false in case if vertex is not found, otherwise returns true.
        ///
        bool AddLabel(std::uint64_t vertex_id, std::string label);

        ///
        /// @brief Removes the label from the passed vertex, method has no effect if vertex does not have the passed label
        ///
        /// @param vertex_id The ID of the vertex to process
        /// @param label The label will be removed from the vertex
        /// @return false in case when vertex is not found, otherwise returns true.
        ///
        bool RemoveLabel(std::uint64_t vertex_id, std::string label);

        ///
        /// @brief Finds the shortest directed path between 2 vertices such that every vertex on such path contains the
        /// passed label
        ///
        /// @param src_vertex_id Start vertex of the path
        /// @param dst_vertex_id Destination vertex of the path
        /// @param label
        /// @return graph_util::Path If valid path was found
        /// @return std::nullopt If path was not found, or passed vertices does not exist
        ///
        std::optional<graph_util::Path>
        ShortestPath(std::uint64_t src_vertex_id, std::uint64_t dst_vertex_id, std::string label);

    private:
    };

} // namespace graph_store


#endif //GRAPHSTORE_GRAPH_STORE_HPP
