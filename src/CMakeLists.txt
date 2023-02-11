add_library(graph_store graph_store.hpp graph_store.cpp graph_util.cpp graph_util.hpp)
target_include_directories(graph_store PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})