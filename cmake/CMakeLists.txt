project(TestCMakeProject)

set(CMAKE_CXX_STANDARD 14)
set (SOURCE_FILES test_cmake.cc)

add_executable (test_cmake ${SOURCE_FILES})
