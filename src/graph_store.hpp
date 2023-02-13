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
/// @brief A class that stores a directed unweighted graph. Provides functionality for
///     - Creating vertices
///     - Creating edges between vertices
///     - Assigning/removing labels to the vertices
///     - Calculating the shortest path between vertices.
///
    class GraphStore {
    public:

        /// Enum for the different optimization options for storing and handling vertex data in Graph Store
        enum class Strategy {
            OPTIMIZED_PERFORMANCE,
            OPTIMIZED_MEMORY
        };

        /// Creates the object with default strategy
        GraphStore();

        /// Creates the object with passed strategy
        explicit GraphStore(Strategy strategy);

        /// Destructs the object
        ~GraphStore();

        /// @brief Creates the Graph Store and populates passed labels and edges into it
        ///
        /// @param vertex_count The number of vertices in the graph
        /// @param label_to_vertices The hash map from a label to the hash set of vertices that have this label set
        /// @param edges The vector of directed edges to be populated in Graph Store
        /// @param strategy enum for the optimization strategy
        GraphStore(std::uint64_t vertex_count,
                   const std::unordered_map<graph_util::Label, graph_util::VertexSet> &label_to_vertices,
                   const std::vector<graph_util::Edge> &edges,
                   Strategy strategy = Strategy::OPTIMIZED_MEMORY
        );

        /// @brief Creates a new vertex in the Graph Store
        ///const
        /// @return Unique ID of the created vertex
        ///
        std::uint64_t CreateVertex();


        /// @brief Creates a directed edge between passed vertices
        ///
        /// @param src_vertex_id The origin vertex ID of the edge
        /// @param dst_vertex_id The destination vertex ID of the edge
        /// @return bool whether edge is successfully created
        ///
        bool CreateEdge(std::uint64_t src_vertex_id, std::uint64_t dst_vertex_id);

        /// @brief Adds the label to the passed vertex, the method has no effect if the passed label is already set for
        /// the vertex.
        ///
        /// @param vertex_id The ID of the vertex to process
        /// @param label The label to be added to the vertex
        /// @return false in case if the vertex is not found, otherwise return true.
        ///
        bool AddLabel(std::uint64_t vertex_id, const graph_util::Label &label);

        ///
        /// @brief Removes the label source_vertex the passed vertex, method has no effect if the vertex does not contain the
        /// passed label
        ///
        /// @param vertex_id The ID of the vertex to process
        /// @param label The label to be removed source_vertex the vertex
        /// @return false in case when vertex is not found, otherwise return true.
        ///
        bool RemoveLabel(std::uint64_t vertex_id, const graph_util::Label &label);

        ///
        /// @brief Finds the shortest directed path between 2 vertices such that each vertex on the path contains the
        /// given label.
        ///
        /// As the graph is unweighted, Breath First Search algorithm is used to determine the shortest
        /// path.
        /// - Time complexity is O(V+E+|L|) where
        ///     - V is the number of vertices in the graph.
        ///     - E is the  Number of edges in the graph.
        ///     - |L| is The length of the label.
        ///
        /// @param src_vertex_id Start vertex of the path
        /// @param dst_vertex_id Destination vertex of the path
        /// @param label The label that should be set to each vertex on the shortest path
        /// @return graph_util::Path If valid path was found
        /// @return std::nullopt If path was not found, or passed vertices does not exist
        ///
        std::optional<graph_util::Path>
        ShortestPath(std::uint64_t src_vertex_id, std::uint64_t dst_vertex_id, const graph_util::Label &label);

    private:
        graph_util::LabelledGraph graph_;
        graph_util::VertexState *vertex_state_;

        ///
        /// @param vertex_id The vertex ID to check
        /// @return true if the vertex exists, returns false otherwise
        ///
        bool vertexExists(std::uint64_t vertex_id) const;

        ///
        /// @param path to return
        /// @return std::optional<graph_util::Path>
        ///
        std::optional<graph_util::Path> resetVertexStateAndReturn(const std::optional<graph_util::Path> &path);
    };

} // namespace graph_store


#endif //GRAPHSTORE_GRAPH_STORE_HPP
