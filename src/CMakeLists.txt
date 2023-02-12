add_subdirectory(util)
add_library(graph_store graph_store.hpp graph_store.cpp util/graph_util.cpp util/graph_util.hpp util/vertex_state.cpp util/vertex_state.hpp)
target_include_directories(graph_store PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})