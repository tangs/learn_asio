#pragma once

#include <string>
#include <string_view>

namespace utils {

/**
 * @brief 获取当前时间的字符串表示
 * @return 格式化的时间字符串，格式为 "HH:MM:SS.mmm"
 */
std::string get_current_time();

/**
 * @brief 移除字符串两端的空白字符
 * @param str 输入字符串
 * @return 返回一个不包含两端空白字符的 string_view
 */
std::string_view trim(std::string_view str);

} // namespace utils 