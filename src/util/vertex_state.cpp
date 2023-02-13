#include "vertex_state.hpp"
#include <limits>
#include <functional>

namespace graph_util {

    Path VertexState::FindPath(std::uint64_t src_vertex_id, std::uint64_t dst_vertex_id) {
        std::function<void(std::uint64_t, std::uint64_t, Path *)> findPathRec;
        findPathRec = [this, &findPathRec](std::uint64_t curr_vertex_id, std::uint64_t src_vertex_id, Path *path) {
            if (curr_vertex_id != src_vertex_id) {
                findPathRec(this->GetParent(curr_vertex_id), src_vertex_id, path);
            }
            path->vertices.push_back(curr_vertex_id);
        };

        Path path;
        findPathRec(dst_vertex_id, src_vertex_id, &path);
        path.length = path.vertices.size() - 1;
        return path;
    }

    // Do nothing by default
    void VertexState::ProcessVertexAddition() {

    }

    std::uint64_t OptimizedMemoryVertexState::GetDistance(std::uint64_t vertex_id) {
        const auto it = distances_.find(vertex_id);
        if (it != distances_.end()) {
            return it->second;
        }

        return std::numeric_limits<std::uint64_t>::max();
    }

    bool OptimizedMemoryVertexState::SetDistance(std::uint64_t vertex_id, std::uint64_t value) {
        distances_[vertex_id] = value;
        return true;
    }

    std::uint64_t OptimizedMemoryVertexState::GetParent(std::uint64_t vertex_id) {
        const auto it = parent_.find(vertex_id);
        if (it != parent_.end()) {
            return it->second;
        }

        return std::numeric_limits<std::uint64_t>::max();
    }

    bool OptimizedMemoryVertexState::SetParent(std::uint64_t vertex_id, std::uint64_t parent_vertex_id) {
        parent_[vertex_id] = parent_vertex_id;
        return true;
    }


    void OptimizedMemoryVertexState::Reset() {
        distances_.clear();
        parent_.clear();
    }


    std::uint64_t OptimizedPerformanceVertexState::GetDistance(std::uint64_t vertex_id) {
        return distances_[vertex_id];
    }

    bool OptimizedPerformanceVertexState::SetDistance(std::uint64_t vertex_id, std::uint64_t value) {
        distances_[vertex_id] = value;
        affected_vertices_.push_back(vertex_id);
        return true;
    }

    std::uint64_t OptimizedPerformanceVertexState::GetParent(std::uint64_t vertex_id) {
        return parent_[vertex_id];
    }

    bool OptimizedPerformanceVertexState::SetParent(std::uint64_t vertex_id, std::uint64_t parent_vertex_id) {
        parent_[vertex_id] = parent_vertex_id;
        return true;
    }

    void OptimizedPerformanceVertexState::Reset() {
        for (const auto vertex: affected_vertices_) {
            parent_[vertex] = 0;
            distances_[vertex] = std::numeric_limits<std::uint64_t>::max();
        }
        affected_vertices_.clear();
    }

    void OptimizedPerformanceVertexState::ProcessVertexAddition() {
        parent_.push_back(0);
        distances_.push_back(std::numeric_limits<std::uint64_t>::max());
    }


} // namespace graph_util