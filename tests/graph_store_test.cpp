#include <gtest/gtest.h>
#include "graph_store.hpp"

TEST(GraphStoreTest, TestShortestPath)
{
    graph_store::GraphStore gs;
    EXPECT_EQ(gs.ShortestPath(0 , 1 , "a") , std::nullopt);
}
