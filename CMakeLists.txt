project(boost_optional_ext)
cmake_minimum_required(VERSION 3.1)
enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

set(CMAKE_CXX_STANDARD 17)

SET (BOOST_OPTIONAL_EXT ${CMAKE_CURRENT_SOURCE_DIR})

include(${CMAKE_CURRENT_SOURCE_DIR}/Build/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

SET (TEST_SRC
        tests/test_optional_ext.cpp
        tests/test_optional_ext_with_const.cpp
        tests/test_hof.cpp
        tests/tests_main.cpp
)

add_executable(boost_optional_ext ${TEST_SRC})
target_link_libraries(boost_optional_ext CONAN_PKG::boost)
target_include_directories(boost_optional_ext
    PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${BOOST_OPTIONAL_EXT})

SET (EXAMPLE_SRC
        examples/ex_1/data_service/CDefDataProvider.cpp
        examples/ex_1/main.cpp
)

add_executable(boost_optional_ext_example ${EXAMPLE_SRC})
target_link_libraries(boost_optional_ext_example CONAN_PKG::boost)
target_include_directories(boost_optional_ext_example
    PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${BOOST_OPTIONAL_EXT})
    
if(MSVC)
  add_compile_options("-W" "-Wall" "MDd" "D_DEBUG")
else()
  add_compile_options("-W" "-Wall" "-O0" "-g" "-pedantic")
endif()    
