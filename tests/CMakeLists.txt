include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

add_executable(graph_store_test graph_store_test.cpp)

target_link_libraries(graph_store_test gtest_main graph_store)

include(GoogleTest)
gtest_discover_tests(graph_store_test)