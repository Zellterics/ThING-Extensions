cmake_minimum_required(VERSION 3.10)
project(test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TEST_ROOT_DIR "${CMAKE_SOURCE_DIR}")

add_subdirectory(ThING-Engine)
target_compile_definitions(ThING PUBLIC $<$<CONFIG:Debug>:DEBUG>)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    demo/*.cpp
)

add_executable(test 
    main.cpp
    ${SOURCES}
)
target_link_libraries(test PRIVATE ThING)
