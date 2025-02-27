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

# 使用父项目的通用函数设置属性
set_common_target_properties(mutual_exclusion_test)

# 链接 Google Test
target_link_libraries(mutual_exclusion_test
  PRIVATE
  gtest_main
  gmock_main
)

# 添加测试
include(GoogleTest)
gtest_discover_tests(mutual_exclusion_test) 