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
  add_definitions(-D_ITERATOR_DEBUG_LEVEL=0 -DBOOST_ALL_NO_LIB verbosity:detailed)
  add_compile_options("-W" "-Wall" "verbose")
  set_property(TARGET boost_optional_ext PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  set_property(TARGET boost_optional_ext_example PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
else()
  add_compile_options("-W" "-Wall" "-O0" "-g" "-pedantic")
endif()    
