cmake_minimum_required(VERSION 3.28)
project(ecs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_TESTS "Build all tests" ON)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME Expected
        VERSION 1.1.0
        GITHUB_REPOSITORY "TartanLlama/expected"
        OPTIONS "EXPECTED_BUILD_TESTS OFF"
)

add_library(ecs STATIC
        include/ecs.hpp
        src/ecs.cpp
        include/error.hpp
        include/types.hpp
        include/compressor.hpp
        src/compressor.cpp
        include/entity.hpp
        src/entity.cpp
        include/component.hpp
        include/const.hpp
)
target_include_directories(ecs PUBLIC include)
target_link_libraries(ecs PUBLIC tl::expected)

if (BUILD_TESTS)
    CPMAddPackage(
            NAME Catch2
            VERSION 3.6.0
            GITHUB_REPOSITORY "catchorg/Catch2"
            OPTIONS "CATCH_INSTALL_HELPERS OFF" "CATCH_BUILD_TESTING OFF" "CATCH_INSTALL_DOCS OFF"
    )
    enable_testing()

    #  If the CONFIGURE_DEPENDS flag is specified, CMake will add logic to the main build system check target
    #  to rerun the flagged GLOB commands at build time. If any of the outputs change, CMake will regenerate the build system.
    file(GLOB test-sources CONFIGURE_DEPENDS test/*.cpp)
    add_executable(${PROJECT_NAME}-tests "${test-sources}")
    target_compile_options(${PROJECT_NAME}-tests PRIVATE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>)
    target_link_libraries(${PROJECT_NAME}-tests
            PRIVATE
            Catch2::Catch2WithMain
            ecs
    )
endif ()

add_executable(${PROJECT_NAME}-main main.cpp)
target_link_libraries(${PROJECT_NAME}-main ecs)
target_include_directories(${PROJECT_NAME}-main PRIVATE include)

