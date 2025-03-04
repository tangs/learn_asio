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

include(FetchContent)
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-32-0  # 指定版本号
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
    )
    target_compile_definitions(${target_name} PRIVATE ASIO_STANDALONE)
    
    # Windows 平台特定设置
    if(WIN32)
        target_link_libraries(${target_name} PRIVATE ws2_32 wsock32)
    endif()
    
    # 所有平台都需要线程库
    find_package(Threads REQUIRED)
    target_link_libraries(${target_name} PRIVATE Threads::Threads)
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

# Mutual exclusion example
add_executable(mutual_exclusion_1 src/mutual_exclusion_1.cpp)
set_common_target_properties(mutual_exclusion_1)

# 添加测试目录
add_subdirectory(tests)