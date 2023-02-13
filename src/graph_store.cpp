#include "graph_store.hpp"

#include <queue>
#include <stdexcept>

namespace graph_store {

    GraphStore::GraphStore() : GraphStore(Strategy::OPTIMIZED_PERFORMANCE) {
        // Use optimized performance VertexState by default.
    }

    GraphStore::GraphStore(const Strategy strategy) {
        if (strategy == Strategy::OPTIMIZED_MEMORY) {
            vertex_state_ = new graph_util::OptimizedMemoryVertexState;
        } else if (strategy == Strategy::OPTIMIZED_PERFORMANCE) {
            vertex_state_ = new graph_util::OptimizedPerformanceVertexState;
        }
    }

    GraphStore::GraphStore(const std::uint64_t vertex_count,
                           const std::unordered_map<graph_util::Label, graph_util::VertexSet> &label_to_vertices,
                           const std::vector<graph_util::Edge> &edges,
                           const Strategy strategy) : GraphStore(strategy) {

        for (int i = 0; i < vertex_count; i++) {
            CreateVertex();
        }

        // Populate labels.
        for (const auto &[label, vertex_set]: label_to_vertices) {
            for (const auto vertex: vertex_set) {
                if (!AddLabel(vertex, label)) {
                    throw std::invalid_argument("Failed to populate labels.");
                }
            }
        }

        // Populate edges.
        for (const auto edge: edges) {
            if (!CreateEdge(edge.source_vertex, edge.destination_vertex)) {
                throw std::invalid_argument("Failed to populate edges.");
            }
        }
    }

    GraphStore::~GraphStore() {
        delete vertex_state_;
    }

    std::uint64_t GraphStore::CreateVertex() {
        std::uint64_t id = graph_.neighbours.size();
        graph_.neighbours.emplace_back();
        vertex_state_->ProcessVertexAddition();
        return id;
    }

    bool GraphStore::CreateEdge(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id) {
        if (!vertexExists(src_vertex_id) || !vertexExists(dst_vertex_id)) {
            return false;
        }
        graph_.neighbours[src_vertex_id].push_back(dst_vertex_id);
        return true;
    }

    bool GraphStore::AddLabel(const std::uint64_t vertex_id, const graph_util::Label &label) {
        if (!vertexExists(vertex_id)) {
            return false;
        }
        graph_.label_to_vertices[label].insert(vertex_id);
        return true;
    }

    bool GraphStore::RemoveLabel(const std::uint64_t vertex_id, const graph_util::Label &label) {
        if (!vertexExists(vertex_id)) {
            return false;
        }

        auto vertices_it = graph_.label_to_vertices.find(label);
        if (vertices_it != graph_.label_to_vertices.end()) {
            vertices_it->second.erase(vertex_id);
        }

        return true;
    }

    std::optional<graph_util::Path>
    GraphStore::ShortestPath(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id,
                             const graph_util::Label &label) {
        // Return if one or both vertices do not exist.
        if (!vertexExists(src_vertex_id) || !vertexExists(dst_vertex_id)) {
            return resetVertexStateAndReturn(std::nullopt);
        }

        const auto labeled_vertices_pair = graph_.label_to_vertices.find(label);

        // If the label is not set to any vertex, we can immediately return.
        if (labeled_vertices_pair == graph_.label_to_vertices.end()) {
            return resetVertexStateAndReturn(std::nullopt);
        }

        const auto &valid_vertices = labeled_vertices_pair->second;

        // if the source or destination vertices do not have the specified label, labelled path does not exist between them.
        if (valid_vertices.count(src_vertex_id) == 0 || valid_vertices.count(dst_vertex_id) == 0) {
            return resetVertexStateAndReturn(std::nullopt);
        }

        vertex_state_->SetDistance(src_vertex_id, 0);

        // Queue for Breadth First Search.
        std::queue<std::uint64_t> vertex_queue;
        vertex_queue.push(src_vertex_id);

        // If the source and the destination are the same, skip entire while loop.
        bool reached_dst_vertex = (src_vertex_id == dst_vertex_id);

        while (!vertex_queue.empty() && !reached_dst_vertex) {
            std::uint64_t curr_vertex = vertex_queue.front();
            vertex_queue.pop();

            for (const std::uint64_t neighbour: graph_.neighbours[curr_vertex]) {
                if (valid_vertices.count(neighbour) == 0) {
                    continue;
                }

                std::uint64_t distance_to_curr = vertex_state_->GetDistance(curr_vertex);
                std::uint64_t distance_to_neighbour = vertex_state_->GetDistance(neighbour);


                if (distance_to_neighbour > distance_to_curr + 1) {
                    vertex_state_->SetDistance(neighbour, distance_to_curr + 1);
                    vertex_state_->SetParent(neighbour, curr_vertex);
                    vertex_queue.push(neighbour);
                }

                if (neighbour == dst_vertex_id) {
                    // If we reached the destination vertex, we can terminate BFS algorithm, because the shortest path
                    // is already found for the destination vertex.
                    reached_dst_vertex = true;
                    break;
                }
            }
        }

        if (!reached_dst_vertex) {
            return resetVertexStateAndReturn(std::nullopt);
        }

        auto path = vertex_state_->FindPath(src_vertex_id, dst_vertex_id);
        vertex_state_->Reset();
        return path;
    }

    bool GraphStore::vertexExists(const std::uint64_t vertex_id) const {
        return vertex_id < graph_.neighbours.size();
    }

    std::optional<graph_util::Path> GraphStore::resetVertexStateAndReturn(const std::optional<graph_util::Path> &path) {
        vertex_state_->Reset();
        return path;
    }

} // namespace graph_store