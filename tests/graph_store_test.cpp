#include <gtest/gtest.h>
#include "graph_store.hpp"
#include "util/graph_util.hpp"

constexpr std::uint64_t inf = 1e+9;
using adj_matrix = std::vector<std::vector<std::uint64_t>>;


// Floyd-Warshall algorithm to calculate the shortest paths for all pairs of vertices.
adj_matrix FloydWarshall(const std::int64_t vertex_count, const std::vector<graph_util::Edge> &edges) {
    adj_matrix dis(vertex_count , std::vector<std::uint64_t>(vertex_count , inf));

    for (const auto& edge : edges) {
        dis[edge.source_vertex][edge.destination_vertex] = 1;
    }

    for (auto i = 0; i < vertex_count; ++i) {
        dis[i][i] = 0;
    }

    for (auto k = 0; k < vertex_count; ++k) {
        for (auto i = 0; i < vertex_count; ++i) {
            for (auto j = 0; j < vertex_count; ++j) {
                dis[i][j] = std::min(dis[i][j], dis[i][k] + dis[k][j]);
            }
        }
    }
    return dis;
}

std::vector<graph_util::Edge> GenerateRandomGraph(std::int64_t vertex_count , std::int64_t edge_count) {
    std::vector<graph_util::Edge> edges;

    for (auto i = 0; i < edge_count; ++i) {
        graph_util::Edge edge;
        // multiple edges are allowed according to the Graph Store implementation.
        edge.source_vertex = rand() % vertex_count;
        edge.destination_vertex = rand() % vertex_count;
        edges.push_back(edge);
    }

    return edges;
}


// TODO maybe refactor tests to test single scenario per one test
TEST(GraphStoreTest, OneVertex) {
    std::string label = "testLabel";
    graph_store::GraphStore gs;
    auto id = gs.CreateVertex();

    // Path to itself does not exist because vertex has no label.
    ASSERT_FALSE(gs.ShortestPath(id, id, label).has_value());

    // Add label.
    EXPECT_TRUE(gs.AddLabel(id , label));

    auto path = gs.ShortestPath(id, id, label);
    ASSERT_TRUE(path.has_value());
    graph_util::Path want = {id, {id}};
    ASSERT_EQ(path.value(), want);

    // Add label again, should no-op.
    EXPECT_TRUE(gs.AddLabel(id , label));
    ASSERT_TRUE(gs.ShortestPath(id, id, label).has_value());

    // label was added twice, make sure that one removal completely removes it source_vertex the vertex.
    EXPECT_TRUE(gs.RemoveLabel(id, label));
    ASSERT_FALSE(gs.ShortestPath(id, id, label).has_value());
}

TEST(GraphStoreTest, LinearGraph) {
    // linear graph: [ 0 -> 1 -> 2 ], all the vertices have the same label
    std::string label = "testLabel";
    graph_store::GraphStore gs(3, {{label, {0, 1, 2}}}, {{0, 1},
                                                          {1, 2},
                                                          });

    ASSERT_FALSE(gs.ShortestPath(1, 0, label).has_value());
    ASSERT_FALSE(gs.ShortestPath(2, 1, label).has_value());
    ASSERT_FALSE(gs.ShortestPath(2, 0, label).has_value());

    {
        auto path = gs.ShortestPath(0, 1, label);
        ASSERT_TRUE(path.has_value());
        graph_util::Path want = {1, {0, 1}};
        ASSERT_EQ(path.value(), want);
    }

    {
        auto path = gs.ShortestPath(0, 2, label);
        ASSERT_TRUE(path.has_value());
        graph_util::Path want = {2, {0, 1 , 2}};
        ASSERT_EQ(path.value(), want);
    }

    {
        auto path = gs.ShortestPath(1, 2, label);
        ASSERT_TRUE(path.has_value());
        graph_util::Path want = {1, {1, 2}};
        ASSERT_EQ(path.value(), want);
    }

    {
        // remove the label of the vertex 1 and verify that path 0->2 does not exist.
        gs.RemoveLabel(1 , label);
        auto path = gs.ShortestPath(1, 2, label);
        ASSERT_FALSE(gs.ShortestPath(0, 2, label).has_value());
    }
}

TEST(GraphStoreTest, RandomGraphsWithOneLabel) {
    for (auto i = 0; i < 100; ++i) {
        std::uint64_t vertex_count = rand() % 50 + 2;
        std::uint64_t edge_count = rand() % (vertex_count * (vertex_count - 1) / 2);

        std::string label = "testLabel";
        graph_util::VertexSet vertices;
        for (auto i = 0; i < vertex_count; ++i) vertices.insert(i);

        auto edges = GenerateRandomGraph(vertex_count , edge_count);
        auto want_dis_matrix = FloydWarshall(vertex_count, edges);

        graph_store::GraphStore gs(vertex_count , {{label, vertices}} , edges);
        adj_matrix got_dis_matrix(vertex_count , std::vector<std::uint64_t>(vertex_count , inf));

        for (auto i = 0; i < vertex_count; ++i) {
            for (auto j = 0; j < vertex_count; ++j) {
                auto path = gs.ShortestPath(i,  j, label);

                if (path.has_value()) {
                    got_dis_matrix[i][j] = path->length;
                }
            }
        }

        ASSERT_EQ(want_dis_matrix, got_dis_matrix);
    }

    //TODO more tests e.g. max tests, test multiple labels
}
