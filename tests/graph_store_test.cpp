#include <gtest/gtest.h>
#include "graph_store.hpp"
#include "util/graph_util.hpp"

#include <chrono>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

constexpr std::uint64_t inf = 1e+9;
using adj_matrix = std::vector<std::vector<std::uint64_t>>;


// Floyd-Warshall algorithm to calculate the shortest paths for all pairs of vertices.
adj_matrix FloydWarshall(const std::uint64_t vertex_count, const std::vector<graph_util::Edge> &edges) {
    adj_matrix dis(vertex_count, std::vector<std::uint64_t>(vertex_count, inf));

    for (const auto &edge: edges) {
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

std::vector<graph_util::Edge> GenerateRandomGraph(std::uint64_t vertex_count, std::uint64_t edge_count) {
    std::vector<graph_util::Edge> edges;

    for (auto i = 0; i < edge_count; ++i) {
        graph_util::Edge edge{0,0};
        // Multiple edges are allowed according to the Graph Store implementation.
        edge.source_vertex = std::rand() % vertex_count;
        edge.destination_vertex = std::rand() % vertex_count;
        edges.push_back(edge);
    }

    return edges;
}

struct GraphStoreTestParam {
    std::uint64_t src_vertex_id;
    std::uint64_t dst_vertex_id;
    std::string label;
    std::vector<std::pair<std::string, std::pair<std::uint64_t, std::uint64_t>>> actions;
    graph_util::Path want;
};

std::vector<GraphStoreTestParam> params = {
        // Path does not exist, no actions.
        {0, 3, "3", {},                               {}},
        {3, 0, "1", {},                               {}},
        {3, 0, "2", {},                               {}},
        {3, 0, "3", {},                               {}},
        {1, 0, "1", {},                               {}},
        {2, 0, "1", {},                               {}},
        {2, 3, "3", {},                               {}},

        // Path exists, no additional actions.
        {0, 3, "1", {},                               {2, {0, 1, 3}}},
        {0, 3, "2", {},                               {2, {0, 2, 3}}},
        {0, 1, "1", {},                               {1, {0, 1}}},
        {0, 2, "2", {},                               {1, {0, 2}}},

        // Add label "3" to vertex 2.
        {0, 3, "3", {{"AddLabel",     {2, 3}}},       {2, {0, 2, 3}}},

        // Remove label "1" from vertex 1, path should no longer exist.
        {0, 3, "1", {{"RemoveLabel",  {1, 1}}},       {}},

        // New vertex creation scenarios.
        {0, 4, "1", {{"CreateVertex", {}}},           {}},

        // Add vertex, Add edge, no label.
        {0, 4, "1", {{"CreateVertex", {}},
                            {"CreateEdge", {3, 4}}
                    },                                {}},
        // Add vertex, add label, create edge.
        {0, 4, "1", {{"CreateVertex", {}},
                            {"CreateEdge", {3, 4}},
                            {"AddLabel", {4, 1}}
                    },                                {3, {0, 1, 3, 4}}},

        {0, 4, "1", {{"CreateVertex", {}},
                            {"CreateEdge", {3, 4}},
                            {"AddLabel", {4, 1}},
                            {"RemoveLabel", {4, 1}}}, {}},

};


class GraphStorePTest : public ::testing::TestWithParam<GraphStoreTestParam> {
};

INSTANTIATE_TEST_SUITE_P(GraphStoreTestSuite, GraphStorePTest, ::testing::ValuesIn(params));

TEST_P(GraphStorePTest, TestShortestPathOptimizedMemory) {
    graph_store::GraphStore gs(4, {{"1", {0, 1, 3}},
                                   {"2", {0, 2, 3}},
                                   {"3", {0, 3}}},
                               {{0, 1},
                                {0, 2},
                                {1, 3},
                                {2, 3}}, graph_store::GraphStore::Strategy::OPTIMIZED_MEMORY);

    auto param = GetParam();

    for (const auto &[action, action_val]: param.actions) {
        if (action == "AddLabel") {
            gs.AddLabel(action_val.first, std::to_string(action_val.second));
        } else if (action == "RemoveLabel") {
            gs.RemoveLabel(action_val.first, std::to_string(action_val.second));
        } else if (action == "CreateEdge") {
            gs.CreateEdge(action_val.first, action_val.second);
        } else if (action == "CreateVertex") {
            gs.CreateVertex();
        }
    }

    graph_util::Path empty_path = {0, {}};
    EXPECT_EQ(gs.ShortestPath(param.src_vertex_id, param.dst_vertex_id, param.label).value_or(empty_path), param.want);
}

// Duplicated code compared to previous test. I could not find the good solution to run the same test code for different
// GraphStore configurations.
TEST_P(GraphStorePTest, TestShortestPathOptimizedPerformance) {
    graph_store::GraphStore gs(4, {{"1", {0, 1, 3}},
                                   {"2", {0, 2, 3}},
                                   {"3", {0, 3}}},
                               {{0, 1},
                                {0, 2},
                                {1, 3},
                                {2, 3}}, graph_store::GraphStore::Strategy::OPTIMIZED_PERFORMANCE);

    auto param = GetParam();

    for (const auto &[action, action_val]: param.actions) {
        if (action == "AddLabel") {
            gs.AddLabel(action_val.first, std::to_string(action_val.second));
        } else if (action == "RemoveLabel") {
            gs.RemoveLabel(action_val.first, std::to_string(action_val.second));
        } else if (action == "CreateEdge") {
            gs.CreateEdge(action_val.first, action_val.second);
        } else if (action == "CreateVertex") {
            gs.CreateVertex();
        }
    }

    graph_util::Path empty_path = {0, {}};
    EXPECT_EQ(gs.ShortestPath(param.src_vertex_id, param.dst_vertex_id, param.label).value_or(empty_path), param.want);
}

class GraphStoreTestWithDifferentStrategies : public ::testing::TestWithParam<graph_store::GraphStore::Strategy> {
};

INSTANTIATE_TEST_SUITE_P(GraphStoreTestSuite, GraphStoreTestWithDifferentStrategies,
                         ::testing::Values(graph_store::GraphStore::Strategy::OPTIMIZED_PERFORMANCE,
                                           graph_store::GraphStore::Strategy::OPTIMIZED_MEMORY));


TEST_P(GraphStoreTestWithDifferentStrategies, SingleVertexNoLabel) {
    graph_store::GraphStore gs(GetParam());
    auto id = gs.CreateVertex();

    std::string label = "testLabel";
    // Path to itself does not exist because vertex has no label.
    ASSERT_FALSE(gs.ShortestPath(id, id, label).has_value());
}

TEST_P(GraphStoreTestWithDifferentStrategies, SingleVertexWithLabel) {
    graph_store::GraphStore gs(GetParam());
    std::string label = "testLabel";
    auto id = gs.CreateVertex();

    // Add label.
    EXPECT_TRUE(gs.AddLabel(id, label));

    auto path = gs.ShortestPath(id, id, label);
    ASSERT_TRUE(path.has_value());
    graph_util::Path want = {id, {id}};
    ASSERT_EQ(path.value(), want);
}

TEST_P(GraphStoreTestWithDifferentStrategies, SameLabelMultipleTimes) {
    graph_store::GraphStore gs(GetParam());
    std::string label = "testLabel";
    auto id = gs.CreateVertex();

    // Add label.
    EXPECT_TRUE(gs.AddLabel(id, label));
    // Add label again, should be no-op.
    EXPECT_TRUE(gs.AddLabel(id, label));
    // Label was added twice, removal should completely remove it from the vertex.
    EXPECT_TRUE(gs.RemoveLabel(id, label));

    ASSERT_FALSE(gs.ShortestPath(id, id, label).has_value());
}

TEST_P(GraphStoreTestWithDifferentStrategies, EmptyTwoVertexGraph) {
    graph_store::GraphStore gs(GetParam());
    std::string label = "testLabel";
    auto id1 = gs.CreateVertex();
    auto id2 = gs.CreateVertex();

    ASSERT_FALSE(gs.ShortestPath(id1, id2, label).has_value());
}

TEST_P(GraphStoreTestWithDifferentStrategies, TwoVertexGraphNoEdges) {
    graph_store::GraphStore gs(GetParam());
    std::string label = "testLabel";
    auto id1 = gs.CreateVertex();
    auto id2 = gs.CreateVertex();
    gs.AddLabel(id1, label);
    gs.AddLabel(id2, label);

    ASSERT_FALSE(gs.ShortestPath(id1, id2, label).has_value());
    ASSERT_FALSE(gs.ShortestPath(id2, id1, label).has_value());
    ASSERT_TRUE(gs.ShortestPath(id1, id1, label).has_value());
    ASSERT_TRUE(gs.ShortestPath(id2, id2, label).has_value());
}

TEST_P(GraphStoreTestWithDifferentStrategies, TwoVertexGraphWithSingleEdge) {
    graph_store::GraphStore gs(GetParam());
    std::string label = "testLabel";
    auto id1 = gs.CreateVertex();
    auto id2 = gs.CreateVertex();
    gs.AddLabel(id1, label);
    gs.AddLabel(id2, label);
    gs.CreateEdge(id1, id2);

    auto path = gs.ShortestPath(id1, id2, label);
    ASSERT_TRUE(path.has_value());
    graph_util::Path want = {1, {id1, id2}};
    ASSERT_EQ(path.value(), want);
}

TEST_P(GraphStoreTestWithDifferentStrategies, TwoVertexGraphWithMultipleLabels) {

    std::string label1 = "testLabel1";
    std::string label2 = "testLabel2";

    graph_store::GraphStore gs(2, {{label1, {0, 1}},
                                   {label2, {0, 1}}}, {{0, 1}}, GetParam());

    {
        auto path = gs.ShortestPath(0, 1, label1);
        ASSERT_TRUE(path.has_value());
        graph_util::Path want = {1, {0, 1}};
        ASSERT_EQ(path.value(), want);
    }

    {
        auto path = gs.ShortestPath(0, 1, label2);
        ASSERT_TRUE(path.has_value());
        graph_util::Path want = {1, {0, 1}};
        ASSERT_EQ(path.value(), want);
    }
}

TEST_P(GraphStoreTestWithDifferentStrategies, RandomGraphsWithOneLabel) {
    std::uint64_t graph_count = 100;
    while (graph_count--) {
        std::uint64_t vertex_count = std::rand() % 50 + 2;
        std::uint64_t edge_count = std::rand() % (vertex_count * (vertex_count - 1) / 2);

        std::string label = "testLabel";
        graph_util::VertexSet vertices;
        for (auto i = 0; i < vertex_count; ++i) vertices.insert(i);

        auto edges = GenerateRandomGraph(vertex_count, edge_count);
        auto want_dis_matrix = FloydWarshall(vertex_count, edges);

        graph_store::GraphStore gs(vertex_count, {{label, vertices}}, edges, GetParam());
        adj_matrix got_dis_matrix(vertex_count, std::vector<std::uint64_t>(vertex_count, inf));

        for (auto i = 0; i < vertex_count; ++i) {
            for (auto j = 0; j < vertex_count; ++j) {
                auto path = gs.ShortestPath(i, j, label);

                if (path.has_value()) {
                    got_dis_matrix[i][j] = path->length;
                }
            }
        }

        ASSERT_EQ(got_dis_matrix, want_dis_matrix);
    }
}

TEST_P(GraphStoreTestWithDifferentStrategies, RandomGraphsWithMultipleLabels) {
    std::uint64_t graph_count = 100;
    while (graph_count--){
        std::uint64_t vertex_count = rand() % 30 + 2;
        std::uint64_t edge_count = rand() % (vertex_count * (vertex_count - 1) / 2);

        auto edges = GenerateRandomGraph(vertex_count, edge_count);


        std::unordered_map<std::string, graph_util::VertexSet> label_to_vertices;
        graph_util::VertexSet vertices;
        std::vector<std::uint64_t> vertex_vector;
        for (auto i = 0; i < vertex_count; ++i) {
            vertex_vector.push_back(i);
        }

        const uint64_t label_count = 10;
        // Generate random vertex sets for 10 different labels.
        for (auto i = 0; i < label_count; ++i) {
            std::uint64_t num_vertices = std::rand() % vertex_count + 1;
            std::random_shuffle(vertex_vector.begin(), vertex_vector.end());
            std::string label = std::to_string(i);
            label_to_vertices[label] = graph_util::VertexSet(vertex_vector.begin(),
                                                             vertex_vector.begin() + std::int64_t(num_vertices));
        }

        graph_store::GraphStore gs(vertex_count, label_to_vertices, edges, GetParam());

        // Calculate the shortest paths between all pairs of vertices for each label.
        for (auto l = 0; l < label_count; ++l) {
            std::string label = std::to_string(l);
            adj_matrix got_dis_matrix(vertex_count, std::vector<std::uint64_t>(vertex_count, inf));

            std::vector<graph_util::Edge> labelled_edges;
            for (const auto &edge: edges) {
                if (label_to_vertices[label].count(edge.source_vertex) &&
                    label_to_vertices[label].count(edge.destination_vertex)) {
                    labelled_edges.push_back(edge);
                }
            }
            auto want_dis_matrix = FloydWarshall(vertex_count, labelled_edges);

            for (auto i = 0; i < vertex_count; ++i) {
                if (label_to_vertices[label].count(i) == 0) {
                    want_dis_matrix[i][i] = inf;
                } else {
                    want_dis_matrix[i][i] = 0;
                }
            }

            for (auto i = 0; i < vertex_count; ++i) {
                for (auto j = 0; j < vertex_count; ++j) {
                    auto path = gs.ShortestPath(i, j, label);

                    if (path.has_value()) {
                        got_dis_matrix[i][j] = path->length;
                    }
                }
            }

            ASSERT_EQ(got_dis_matrix, want_dis_matrix);
        }
    }
}

// Performance test on random graph with 10^5 vertices, and 10^6 edges.
TEST_P(GraphStoreTestWithDifferentStrategies, PerFormanceTest) {
    const std::uint64_t vertex_count = 100000;
    const std::uint64_t edge_count = 1000000;
    const std::uint64_t label_count = 10;
    const std::uint64_t label_length = 1000;
    const std::uint64_t queries = 100;

    auto start_time = std::chrono::system_clock::now();

    graph_store::GraphStore gs(GetParam());

    for (auto i = 0; i < vertex_count; ++i) {
        gs.CreateVertex();
    }

    for (auto i = 0; i < edge_count; ++i) {
        gs.CreateEdge(std::rand() % vertex_count, std::rand() % vertex_count);
    }

    std::vector<std::string> labels(label_count);
    for (auto i = 0; i < label_count; ++i) {
        for (auto j = 0; j < label_length; ++j) {
            labels[i] += char(i + '0');
        }
        for (auto j = 0; j < vertex_count; ++j) {
            gs.AddLabel(j, labels[i]);
        }
    }

    for (auto i = 0; i < queries; ++i) {
        std::uint64_t src_vertex = std::rand() % vertex_count;
        std::uint64_t dst_vertex = std::rand() % vertex_count;
        auto path = gs.ShortestPath(src_vertex, dst_vertex, labels[std::rand() % label_count]);
    }

    auto end_time = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    ASSERT_TRUE(elapsed.count() < 10000);
}
