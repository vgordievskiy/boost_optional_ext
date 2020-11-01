project(boost_optional_ext)
cmake_minimum_required(VERSION 3.1)
enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

set(CMAKE_CXX_STANDARD 17)

SET (BOOST_OPTIONAL_EXT ${CMAKE_CURRENT_SOURCE_DIR})

include(${CMAKE_CURRENT_SOURCE_DIR}/Build/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Boost optional extension
SET (EXT_SRC
        boost/optional_ext.hpp
)
add_library(boost_optional_ext_src ${EXT_SRC})
set_target_properties(boost_optional_ext_src PROPERTIES LINKER_LANGUAGE CXX)

# Unit-tests for Boost optional extension
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

# Examples of usage of Boost optional extension
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
    
# Group all files under "src" name
source_group("src"
    FILES ${EXT_SRC} ${TEST_SRC} ${EXAMPLE_SRC}
)
    
if(MSVC)
  add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
  add_compile_options("-W" "-Wall" "verbose")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /VERBOSE")
else()
  add_compile_options("-W" "-Wall" "-O0" "-g" "-pedantic")
endif()    
