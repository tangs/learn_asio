#include "utils/time_utils.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace utils {

std::string get_current_time() {
    const auto now = std::chrono::system_clock::now();
    const auto now_c = std::chrono::system_clock::to_time_t(now);
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    return ss.str();
}

std::string_view trim(std::string_view str) {
    // 找到第一个非空白字符
    const auto left = std::find_if_not(str.begin(), str.end(),
                                     [](const unsigned char ch) { 
                                         return std::isspace(ch); 
                                     });
    
    // 如果字符串全是空白字符，返回空 string_view
    if (left == str.end()) {
        return {};
    }
    
    // 从右边找到第一个非空白字符
    const auto right = std::find_if_not(str.rbegin(), str.rend(),
                                      [](const unsigned char ch) { 
                                          return std::isspace(ch); 
                                      }).base();
    
    // 返回去除两端空白后的 string_view
    return str.substr(left - str.begin(), right - left);
}

} // namespace utils 