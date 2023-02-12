//
// Created by giorgi on 2/12/23.
//

#include "vertex_state.hpp"
#include <limits>
#include <functional>

namespace graph_util {
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

    Path OptimizedMemoryVertexState::FindPath(std::uint64_t src_vertex_id, std::uint64_t dst_vertex_id) {
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

    void OptimizedMemoryVertexState::Reset() {
        distances_.clear();
        parent_.clear();
    }

} // graph_util