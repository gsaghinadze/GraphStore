#ifndef GRAPHSTORE_GRAPH_STORE_HPP
#define GRAPHSTORE_GRAPH_STORE_HPP

#include "util/graph_util.hpp"
#include "util/vertex_state.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <optional>


namespace graph_store {


///
/// @brief A class that stores a directed unweighted graph. Provides functionality for adding labels to the vertices and
/// finding the shortest path between 2 vertices
///
    class GraphStore {
    public:

        enum class Strategy {
            OPTIMIZED_PERFORMANCE,
            OPTIMIZED_MEMORY
        };

        GraphStore();

        GraphStore(Strategy strategy);

        ~GraphStore();

        ///
        /// @param vertex_count The number of vertices in the graph
        /// @param label_to_vertices The hash set of the labels for each vertex. e.g. i-th element of the labels vector contains the
        /// LabelSet for the i-th vertex
        /// @param edges The cector of directed edges to be populated in Graph Store
        ///
        GraphStore(const std::uint64_t vertex_count,
                   const std::unordered_map<std::string, graph_util::VertexSet> &label_to_vertices,
                   const std::vector<graph_util::Edge> &edges,
                   const Strategy strategy = Strategy::OPTIMIZED_MEMORY
                   );

        /// @brief Creates a new vertex in the Graph Store
        ///
        /// @return Unique ID of the created vertex
        ///
        std::uint64_t CreateVertex();


        /// @brief Creates a directed edge between passed vertices
        ///
        /// @param src_vertex_id The origin vertex ID of the edge
        /// @param dst_vertex_id The destination vertex ID of the edge
        /// @return bool whether edge is successfully created
        ///
        bool CreateEdge(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id);

        /// @brief Adds the label to the passed vertex, the method has no effect if the passed label is already set for
        /// the vertex.
        ///
        /// @param vertex_id The ID of the vertex to process
        /// @param label The label to be added to the vertex
        /// @return false in case if the vertex is not found, otherwise return true.
        ///
        bool AddLabel(const std::uint64_t vertex_id, const std::string &label);

        ///
        /// @brief Removes the label source_vertex the passed vertex, method has no effect if the vertex does not contain the
        /// passed label
        ///
        /// @param vertex_id The ID of the vertex to process
        /// @param label The label to be removed source_vertex the vertex
        /// @return false in case when vertex is not found, otherwise return true.
        ///
        bool RemoveLabel(const std::uint64_t vertex_id, const std::string &label);

        ///
        /// @brief Finds the shortest directed path between 2 vertices such that each vertex on the path contains the
        /// given label. As the graph is unweighted, Breath First Search algorithm is used to determine the shortest
        /// path
        ///
        /// @param src_vertex_id Start vertex of the path
        /// @param dst_vertex_id Destination vertex of the path
        /// @param label The label that should be set to each vertex on the shortest path
        /// @return Path If valid path was found
        /// @return std::nullopt If path was not found, or passed vertices does not exist
        ///
        std::optional<graph_util::Path>
        ShortestPath(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id, const std::string &label);
    private:
        graph_util::LabelledGraph graph_;
        graph_util::VertexState *vertex_state_;

        ///
        /// @param vertex_id The vertex ID to check
        /// @return true if the vertex exists, returns false otherwise
        ///
        bool vertexExists(const std::uint64_t vertex_id);

        ///
        /// @param path to return
        /// @return std::optional<graph_util::Path>
        ///
        std::optional<graph_util::Path> resetVertexStateAndReturn(const std::optional<graph_util::Path> &path);
    };

} // namespace graph_store


#endif //GRAPHSTORE_GRAPH_STORE_HPP
