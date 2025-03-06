cmake_minimum_required(VERSION 3.30)
project(lobby_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 特定设置
if(WIN32)
    add_compile_definitions(
        _WIN32_WINNT=0x0A00  # Windows 10
        WIN32_LEAN_AND_MEAN  # 排除不常用的 Windows headers
        NOMINMAX            # 禁用 Windows 的 min/max 宏
    )
endif()

# 添加 Protocol Buffers
include(FetchContent)
FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v3.21.12  # 选择一个稳定版本
)

# 配置 protobuf 构建选项
set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests")
set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(protobuf_MSVC_STATIC_RUNTIME OFF CACHE BOOL "Link static runtime libraries")
set(protobuf_INSTALL OFF CACHE BOOL "Install protobuf binaries and files")
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "Build examples")

# 使 protobuf 可用
FetchContent_MakeAvailable(protobuf)

# 添加 ASIO
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-32-0
)
FetchContent_MakeAvailable(asio)

# 创建工具库
add_library(utils
    src/utils/time_utils.cpp
)
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 通用函数设置目标属性
function(set_common_target_properties target_name)
    target_include_directories(${target_name} PRIVATE 
        ${asio_SOURCE_DIR}/asio/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/generated  # 为生成的 protobuf 文件添加包含路径
    )
    target_compile_definitions(${target_name} PRIVATE ASIO_STANDALONE)
    
    # Windows 平台特定设置
    if(WIN32)
        target_link_libraries(${target_name} PRIVATE ws2_32 wsock32)
    endif()
    
    # 所有平台都需要线程库
    find_package(Threads REQUIRED)
    target_link_libraries(${target_name} PRIVATE 
        Threads::Threads
        protobuf::libprotobuf  # 直接使用 protobuf 目标
    )
endfunction()

# 创建 proto 文件目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)

# 编译 proto 文件的函数
function(compile_proto proto_file)
    get_filename_component(proto_file_abs ${proto_file} ABSOLUTE)
    get_filename_component(proto_file_name ${proto_file} NAME_WE)
    
    add_custom_command(
        OUTPUT
            "${CMAKE_BINARY_DIR}/generated/${proto_file_name}.pb.cc"
            "${CMAKE_BINARY_DIR}/generated/${proto_file_name}.pb.h"
        COMMAND protobuf::protoc
        ARGS
            --cpp_out=${CMAKE_BINARY_DIR}/generated
            -I${CMAKE_SOURCE_DIR}/protos
            ${proto_file_abs}
        DEPENDS ${proto_file_abs}
        COMMENT "Generating protobuf files for ${proto_file}"
    )
endfunction()

# 主程序
add_executable(lobby_server main.cpp)
set_common_target_properties(lobby_server)

# Timer examples
add_executable(timer1 src/timer1.cpp)
set_common_target_properties(timer1)
target_link_libraries(timer1 PRIVATE utils)

add_executable(timer2 src/timer2.cpp)
set_common_target_properties(timer2)
target_link_libraries(timer2 PRIVATE utils)

add_executable(timer3 src/timer3.cpp)
set_common_target_properties(timer3)
target_link_libraries(timer3 PRIVATE utils)

add_executable(timer4 src/timer4.cpp)
set_common_target_properties(timer4)
target_link_libraries(timer4 PRIVATE utils)

add_executable(timer5 src/timer5.cpp)
set_common_target_properties(timer5)
target_link_libraries(timer5 PRIVATE utils)

add_executable(timer6 src/timer6.cpp)
set_common_target_properties(timer6)
target_link_libraries(timer6 PRIVATE utils)

add_executable(daytime1 src/daytime1.cpp)
set_common_target_properties(daytime1)
target_link_libraries(daytime1 PRIVATE utils)

add_executable(daytime_server src/daytime_server.cpp)
set_common_target_properties(daytime_server)
target_link_libraries(daytime_server PRIVATE utils)

add_executable(async_daytime_server src/async_daytime_server.cpp)
set_common_target_properties(async_daytime_server)
target_link_libraries(async_daytime_server PRIVATE utils)

# Mutual exclusion example
add_executable(mutual_exclusion_1 src/mutual_exclusion_1.cpp)
set_common_target_properties(mutual_exclusion_1)

# 添加测试目录
add_subdirectory(tests)