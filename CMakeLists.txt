
cmake_minimum_required(VERSION 3.16)
project(entt_snapshot)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include (FetchContent)

enable_testing()
conan_basic_setup()

file(GLOB_RECURSE sources "src/**.cpp")

file(GLOB_RECURSE test_cases "test/**.cpp")

add_library(entt_snapshot_deps INTERFACE)
target_link_libraries(entt_snapshot_deps INTERFACE
    ${CONAN_LIBS}
)

target_compile_options(entt_snapshot_deps INTERFACE
    -Werror=return-type
    -Werror=unused
    -Werror=pessimizing-move
    -Werror=redundant-move
)

set_target_properties(entt_snapshot_deps PROPERTIES
    INTERFACE CXX_STANDARD 20
    INTERFACE CXX_STANDARD_REQUIRED True
)

target_include_directories(entt_snapshot_deps
    INTERFACE include
    INTERFACE external
)

add_library(entt_snapshot SHARED
    ${sources}
)
target_link_libraries(entt_snapshot entt_snapshot_deps)

# We only want to build tests locally
if(NOT ${only_lib})
    set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external/")

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(entt_snapshot_test
        ${test_cases} ${test_sources}
    )

    target_link_libraries(entt_snapshot_test entt_snapshot gtest_main)

    include(GoogleTest)
    gtest_discover_tests(entt_snapshot_test)
endif()