#include "graph_store.hpp"

#include <queue>
#include <stdexcept>

namespace graph_store {

    GraphStore::GraphStore() {
    }

    GraphStore::GraphStore(const std::uint64_t vertex_count,
                           const std::unordered_map<Label, VertexSet> &label_to_vertices,
                           const std::vector<Edge> &edges) {
        for (int i = 0; i < vertex_count; i++) {
            CreateVertex();
        }

        // Populate labels.
        for (const auto &[label, vertex_set] : label_to_vertices) {
            for (const auto vertex : vertex_set) {
                if (!AddLabel(vertex , label)) {
                    throw std::invalid_argument("Failed to populate labels.");
                }
            }
        }

        // Populate edges.
        for (const auto edge : edges) {
            if (!CreateEdge(edge.from , edge.to)) {
                throw std::invalid_argument("Failed to populate edges.");
            }
        }
    }

    std::uint64_t GraphStore::CreateVertex() {
        int id = neighbours_.size();
        neighbours_.push_back(std::vector<std::uint64_t>());
        return id;
    }

    bool GraphStore::CreateEdge(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id) {
        if (!vertexExists(src_vertex_id) || !vertexExists(dst_vertex_id)) {
            return false;
        }
        neighbours_[src_vertex_id].push_back(dst_vertex_id);
        return true;
    }

    bool GraphStore::AddLabel(const std::uint64_t vertex_id, const Label& label) {
        if (!vertexExists(vertex_id)) {
            return false;
        }
        label_to_vertices_[label].insert(vertex_id);
        return true;
    }

    bool GraphStore::RemoveLabel(const std::uint64_t vertex_id, const Label& label) {
        if (!vertexExists(vertex_id)) {
            return false;
        }

        auto vertices_it = label_to_vertices_.find(label);
        if (vertices_it != label_to_vertices_.end()) {
            vertices_it->second.erase(vertex_id);
        }

        return true;
    }

    std::optional<Path>
    GraphStore::ShortestPath(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id,
                             const Label &label) {

        // Return if one of both vertices do not exist.
        if (!vertexExists(src_vertex_id) || !vertexExists(dst_vertex_id)) {
            return std::nullopt;
        }

        const auto labeled_vertices_pair = label_to_vertices_.find(label);

        // If the label does not exists in the map, path cannot be found and we can immediately return.
        if (labeled_vertices_pair == label_to_vertices_.end()) {
            return std::nullopt;
        }

        const auto &valid_vertices = labeled_vertices_pair->second;

        // if the source or destination vertices do not have the specified label, labelled path does not exists between them.
        if (valid_vertices.count(src_vertex_id) == 0 || valid_vertices.count(dst_vertex_id) == 0) {
            return std::nullopt;
        }

        // Parents map, parent[i] is the parent vertex ID for the vertex with ID=i
        std::unordered_map<std::uint64_t, std::uint64_t> parent;

        // Distances map, distances[i] is the distance from the source vertex to the vertex with ID=i.
        // If the verted with ID=i is not present in the distances map, this means that BFS could not find the path from
        // the source vertex to i-th vertex, or BFS algorithm exited before reaching the i-th vertex.
        std::unordered_map<std::uint64_t, std::uint64_t> distances;
        distances[src_vertex_id] = 0;

        // Queue for Breadth First Search.
        std::queue<std::uint64_t> vertex_queue;
        vertex_queue.push(src_vertex_id);

        // If the source and the destination are the same, skip entire while loop.
        bool reached_dst_vertex = (src_vertex_id == dst_vertex_id);

        while (!vertex_queue.empty() && !reached_dst_vertex) {
            int curr_vertex = vertex_queue.front();
            vertex_queue.pop();

            for (const int neighbour: neighbours_[curr_vertex]) {
                if (valid_vertices.count(neighbour) == 0) {
                    continue;
                }

                // TODO: optimize, keys are searched multiple times in the map.
                if (distances.find(neighbour) == distances.end() || distances[neighbour] > distances[curr_vertex] + 1) {
                    distances[neighbour] = distances[curr_vertex] + 1;
                    parent[neighbour] = curr_vertex;
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
            return std::nullopt;
        }

        return findPath(src_vertex_id, dst_vertex_id, parent);
    }

    inline bool GraphStore::vertexExists(const std::uint64_t vertex_id) {
        return vertex_id < neighbours_.size();
    }

    Path GraphStore::findPath(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id,
                              const std::unordered_map<std::uint64_t, std::uint64_t> &parent) {
        Path path;
        findPathRec(dst_vertex_id, src_vertex_id, parent, &path.vertices);

        path.length = path.vertices.size() - 1;
        return path;
    }

    void GraphStore::findPathRec(std::uint64_t curr_vertex_id, std::uint64_t src_vertex_id,
                                 const std::unordered_map<std::uint64_t, std::uint64_t> &parent,
                                 VertexVector *path) {
        if (curr_vertex_id != src_vertex_id) {
            findPathRec(parent.at(curr_vertex_id), src_vertex_id, parent, path);
        }
        path->push_back(curr_vertex_id);
    }
} // namespace graph_store