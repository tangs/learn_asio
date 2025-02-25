cmake_minimum_required(VERSION 3.30)
project(lobby_server)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-32-0  # 指定版本号
)
FetchContent_MakeAvailable(asio)

add_executable(lobby_server main.cpp)
target_include_directories(lobby_server PRIVATE ${asio_SOURCE_DIR}/asio/include)

add_executable(mutual_exclusion_1 src/mutual_exclusion_1.cpp)
target_include_directories(mutual_exclusion_1 PRIVATE ${asio_SOURCE_DIR}/asio/include)