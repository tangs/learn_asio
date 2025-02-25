# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.14)

# 设置项目名称
project(MutualExclusionTests)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# 对于 CMake 3.14+，使用以下命令
FetchContent_MakeAvailable(googletest)

# 启用测试
enable_testing()

# 添加测试可执行文件
add_executable(mutual_exclusion_test mutual_exclusion_test.cpp)
target_include_directories(mutual_exclusion_test PRIVATE ${asio_SOURCE_DIR}/asio/include)

# 链接 Google Test 和其他必要的库
target_link_libraries(mutual_exclusion_test
  PRIVATE
  gtest_main
  gmock_main
  pthread
)

# 添加 Asio 库
find_package(Threads REQUIRED)
# target_include_directories(mutual_exclusion_test PRIVATE ${CMAKE_SOURCE_DIR}/../)
target_compile_definitions(mutual_exclusion_test PRIVATE ASIO_STANDALONE)
target_link_libraries(mutual_exclusion_test PRIVATE Threads::Threads)

# 添加测试
include(GoogleTest)
gtest_discover_tests(mutual_exclusion_test) 