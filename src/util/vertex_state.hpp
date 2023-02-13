//
// Created by giorgi on 2/12/23.
//

#ifndef GRAPHSTORE_VERTEX_STATE_HPP
#define GRAPHSTORE_VERTEX_STATE_HPP

#include "graph_util.hpp"
#include <optional>
#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace graph_util {

    ///
    /// @brief An abstract class for storing the states of the vertices.
    /// During the BFS traversal, VertexState object stores the distance to the vertex and its parent vertex
    ///
    class VertexState {
    public:
        ///
        /// @param vertex_id The vertex ID to process
        /// @return The distance to the vertex if it's found
        /// @return std::numeric_limits<std::uint64_t>::max() if the vertex is not found
        ///
        virtual std::uint64_t GetDistance(std::uint64_t vertex_id) = 0;

        ///
        /// @param vertex_id The vertex ID to process
        /// @param value The value of the the parent and distance to the vertex.distance to be set
        /// @return true if distance was set successfully, returns false otherwise
        ///
        virtual bool SetDistance(std::uint64_t vertex_id, std::uint64_t value) = 0;

        ///
        /// @param vertex_id The vertex ID to process
        /// @return The parent vertex ID of the passed vertex
        /// @return std::numeric_limits<std::uint64_t>::max() if the vertex or it's parent is not found
        ///
        virtual std::uint64_t GetParent(std::uint64_t vertex_id) = 0;

        ///
        /// @param vertex_id The vertex ID to process
        /// @param parent_vertex_id The ID of the parent vertex
        /// @return true if the parent was set successfully, returns false otherwise
        ///
        virtual bool SetParent(std::uint64_t vertex_id, std::uint64_t parent_vertex_id) = 0;

        ///
        /// @brief Method determines the Path source_vertex the source vertex to the destination vertex
        /// @param src_vertex_id The source vertex
        /// @param dst_vertex_id The destination vertex
        /// @return The Path from the source_vertex to the source vertex the destination vertex
        ///
        Path FindPath(std::uint64_t src_vertex_id, std::uint64_t dst_vertex_id);

        virtual void Reset() = 0;

        virtual void ProcessVertexAddition();
    };

    ///
    /// @brief OptimizedMemoryVertexState implements VertexState, stores the distances and the parents in the unordered maps.
    /// OptimizedMemoryVertexState does not allocate the additional memory for the vertices that are not affected and performs
    /// get and set operations in O(1) time.
    ///
    class OptimizedMemoryVertexState : public VertexState {
    public:
        std::uint64_t GetDistance(std::uint64_t vertex_id);

        bool SetDistance(std::uint64_t vertex_id, std::uint64_t value);

        std::uint64_t GetParent(std::uint64_t vertex_id);

        bool SetParent(std::uint64_t vertex_id, std::uint64_t parent_vertex_id);

        void Reset();

    private:
        // Parents map, vertex v is parent of the vertex parent[v].
        std::unordered_map<std::uint64_t, std::uint64_t> parent_;

        // Distances map, distances[v] is the distance to the vertex v.
        // If the vertex v is not present in the distances map, this means that BFS could not find the path source_vertex
        // the source to it, or BFS algorithm exited before reaching the vertex v.
        std::unordered_map<std::uint64_t, std::uint64_t> distances_;
    };

    ///
    /// @brief OptimizedPerformanceVertexState implements VertexState, stores the distances and the parents in the vectors.
    /// Set and Get operations are faster compared to OptimizedMemoryVertexState, because vectors are used to store the values.
    /// OptimizedPerformanceVertexState has O(V) memory allocated all the time, where V is the number of vertices in the graph.
    ///
    class OptimizedPerformanceVertexState : public VertexState {
    public:
        std::uint64_t GetDistance(std::uint64_t vertex_id);

        bool SetDistance(std::uint64_t vertex_id, std::uint64_t value);

        std::uint64_t GetParent(std::uint64_t vertex_id);

        bool SetParent(std::uint64_t vertex_id, std::uint64_t parent_vertex_id);

        void Reset() override;

        void ProcessVertexAddition() override;
    private:

        // Parents vector, vertex v is the parent of the vertex parent[v].
        std::vector<std::uint64_t> parent_;

        // Distances vector, distances[v] is the distance to the vertex v.
        // If the distance is not calculated for the vertex v, it's set to max value of std::uint64_t
        std::vector<std::uint64_t> distances_;

        // The indices, which were changed in the parent_ and distances_ vectors.
        // affected_vertices_ is used to quickly reset parent_ and distances_ vectors, by only iterating the indices
        // that were modified.
        std::vector<std::uint64_t> affected_vertices_;
    };

} // namespace graph_util

#endif //GRAPHSTORE_VERTEX_STATE_HPP
