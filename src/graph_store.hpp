#ifndef GRAPHSTORE_GRAPH_STORE_HPP
#define GRAPHSTORE_GRAPH_STORE_HPP

#include "graph_util.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <optional>


namespace graph_store {

    using Label = std::string;
    using VertexSet = std::unordered_set<std::uint64_t>;
    using VertexVector = std::vector<std::uint64_t>;

    struct Edge {
        std::uint64_t from;
        std::uint64_t to;
    };

    struct Path {
        std::uint64_t length;
        VertexVector vertices;

        bool operator == (const Path &other) const {
            return length == other.length && vertices == other.vertices;
        }
    };

///
/// @brief A class that stores a directed unweighted graph. Provides functionality for adding labels to the vertices and
/// finding the shortest path between 2 vertices
///
    class GraphStore {
    public:

        GraphStore();

        ///
        /// @param vertex_count The
        /// @param labels The hash set of the labels for each vertex. e.g. i-th element of the labels vector contains the
        /// LabelSet for the i-th vertex
        /// @param edges The cector of directed edges to be populated in Graph Store
        ///
        GraphStore(const std::uint64_t vertex_count, const std::unordered_map<Label, VertexSet> &label_to_vertices,
                   const std::vector<Edge> &edges);

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
        bool AddLabel(const std::uint64_t vertex_id, const Label& label);

        ///
        /// @brief Removes the label from the passed vertex, method has no effect if the vertex does not contain the
        /// passed label
        ///
        /// @param vertex_id The ID of the vertex to process
        /// @param label The label to be removed from the vertex
        /// @return false in case when vertex is not found, otherwise return true.
        ///
        bool RemoveLabel(const std::uint64_t vertex_id, const Label& label);

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
        std::optional<Path>
        ShortestPath(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id, const Label &label);

    private:
        std::vector<VertexVector> neighbours_;
        std::unordered_map<Label, VertexSet> label_to_vertices_;

        ///
        /// @param vertex_id The vertex ID to check
        /// @return true if the vertex exists, returns false otherwise
        ///
        bool vertexExists(const std::uint64_t vertex_id);

        ///
        /// @brief Method determines the Path from the source vertex to the destination vertex for the provided parents map
        ///
        /// @param src_vertex_id The source vertex
        /// @param dst_vertex_id The destination vertex
        /// @param parent map from the vertex to it's parent according to the BFS traversal
        /// @return Path from the source vertex t the destination vertex
        ///
        Path findPath(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id,
                      const std::unordered_map<std::uint64_t, std::uint64_t>& parent);
        ///
        /// @brief Recursively determines the path from the current vertex towards the source vertex
        ///
        /// @param curr_vertex_id current vertex
        /// @param src_vertex_id source vertex
        /// @param parent arent map from the vertex to it's parent according to the BFS traversal
        /// @param path[out] The path that is populated by the recursive method
        ///
        void findPathRec(std::uint64_t curr_vertex_id, std::uint64_t src_vertex_id,
                         const std::unordered_map<std::uint64_t, std::uint64_t> &parent, VertexVector *path);
    };

} // namespace graph_store


#endif //GRAPHSTORE_GRAPH_STORE_HPP
