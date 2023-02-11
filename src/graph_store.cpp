#include "graph_store.hpp"

namespace graph_store {

    GraphStore::GraphStore() {
        // TODO: implement
    }

    std::uint64_t GraphStore::CreateVertex() {
        // TODO: implement
        return 0;
    }

    bool GraphStore::CreateEdge(const std::uint64_t src_vertex_id, const std::uint64_t dst_vertex_id) {
        // TODO: implement
        return false;
    }

    bool GraphStore::AddLabel(std::uint64_t vertex_id, std::string label) {
        // TODO: implement
        return false;
    }

    bool GraphStore::RemoveLabel(std::uint64_t vertex_id, std::string label) {
        // TODO: implement
        return false;
    }

    std::optional<graph_util::Path>
    GraphStore::ShortestPath(std::uint64_t src_vertex_id, std::uint64_t dst_vertex_id, std::string label) {
        // TODO: implement
        return std::nullopt;
    }
} // namespace graph_store