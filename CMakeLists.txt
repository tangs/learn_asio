cmake_minimum_required(VERSION 3.30)
project(lobby_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-32-0  # 指定版本号
)
FetchContent_MakeAvailable(asio)

add_executable(lobby_server main.cpp)
target_include_directories(lobby_server PRIVATE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(lobby_server PRIVATE ASIO_STANDALONE)

# 创建工具库
add_library(utils
    src/utils/time_utils.cpp
)
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Timer examples
add_executable(async_timer src/async_timer.cpp)
target_include_directories(async_timer PRIVATE 
    ${asio_SOURCE_DIR}/asio/include
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(async_timer PRIVATE ASIO_STANDALONE)
target_link_libraries(async_timer PRIVATE utils)

add_executable(time1 src/time1.cpp)
target_include_directories(time1 PRIVATE 
    ${asio_SOURCE_DIR}/asio/include
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(time1 PRIVATE ASIO_STANDALONE)
target_link_libraries(time1 PRIVATE utils)

# Mutual exclusion example
add_executable(mutual_exclusion_1 src/mutual_exclusion_1.cpp)
target_include_directories(mutual_exclusion_1 PRIVATE ${asio_SOURCE_DIR}/asio/include)

# 添加 Asio 库
find_package(Threads REQUIRED)
target_compile_definitions(mutual_exclusion_1 PRIVATE ASIO_STANDALONE)
target_link_libraries(mutual_exclusion_1 PRIVATE Threads::Threads)

# 添加测试目录
add_subdirectory(tests)