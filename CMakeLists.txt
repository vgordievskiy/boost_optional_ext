project(boost_optinal_ext)
cmake_minimum_required(VERSION 3.1)
enable_testing()
set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

SET (CMAKE_CXX_FLAGS "-std=c++2a")
SET (BOOST_OPTIONAL_EXT ${CMAKE_CURRENT_SOURCE_DIR})

message("!!! ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}")

include(${CMAKE_CURRENT_SOURCE_DIR}/Build/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# creates the executable
add_executable(boost_optional_ext tests/test_optional_ext.cpp)
target_link_libraries(boost_optional_ext CONAN_PKG::boost)

# indicates the include paths
target_include_directories(boost_optional_ext
    PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${BOOST_OPTIONAL_EXT})
