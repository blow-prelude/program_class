#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <limits> // For std::numeric_limits

// --- UTF-8 辅助函数 ---
// 这些函数是简化版，假定输入是有效的 UTF-8 序列。
// 对于生产环境，可能需要更健壮的错误处理。

// 将 UTF-8 字符串解码为 Unicode 码点序列 (char32_t)
std::vector<char32_t> decode_utf8(const std::string& utf8_str) {
    std::vector<char32_t> unicode_chars;
    for (size_t i = 0; i < utf8_str.length(); ) {
        unsigned char c = utf8_str[i];
        char32_t code_point;
        size_t bytes_to_read = 0;

        if ((c & 0x80) == 0) { // 1-byte sequence (0xxxxxxx) - ASCII
            code_point = c;
            bytes_to_read = 1;
        } else if ((c & 0xE0) == 0xC0) { // 2-byte sequence (110xxxxx 10xxxxxx)
            if (i + 1 >= utf8_str.length() || (utf8_str[i+1] & 0xC0) != 0x80) { // Check for incomplete or invalid continuation
                code_point = 0xFFFD; bytes_to_read = 1;
            } else {
                code_point = ((c & 0x1F) << 6) | (utf8_str[i+1] & 0x3F);
                bytes_to_read = 2;
            }
        } else if ((c & 0xF0) == 0xE0) { // 3-byte sequence (1110xxxx 10xxxxxx 10xxxxxx)
            if (i + 2 >= utf8_str.length() || (utf8_str[i+1] & 0xC0) != 0x80 || (utf8_str[i+2] & 0xC0) != 0x80) {
                code_point = 0xFFFD; bytes_to_read = 1;
            } else {
                code_point = ((c & 0x0F) << 12) | ((utf8_str[i+1] & 0x3F) << 6) | (utf8_str[i+2] & 0x3F);
                bytes_to_read = 3;
            }
        } else if ((c & 0xF8) == 0xF0) { // 4-byte sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
            if (i + 3 >= utf8_str.length() || (utf8_str[i+1] & 0xC0) != 0x80 || (utf8_str[i+2] & 0xC0) != 0x80 || (utf8_str[i+3] & 0xC0) != 0x80) {
                code_point = 0xFFFD; bytes_to_read = 1;
            } else {
                code_point = ((c & 0x07) << 18) | ((utf8_str[i+1] & 0x3F) << 12) | ((utf8_str[i+2] & 0x3F) << 6) | (utf8_str[i+3] & 0x3F);
                bytes_to_read = 4;
            }
        } else {
            // Invalid UTF-8 starting byte. Treat as a single byte error.
            code_point = 0xFFFD; // Unicode Replacement Character
            bytes_to_read = 1;
        }

        unicode_chars.push_back(code_point);
        i += bytes_to_read;
    }
    return unicode_chars;
}

// 将单个 Unicode 码点编码为 UTF-8 字符串
std::string encode_utf8(char32_t unicode_char) {
    std::string utf8_bytes;
    if (unicode_char < 0x80) { // 1-byte sequence (0xxxxxxx)
        utf8_bytes += static_cast<char>(unicode_char);
    } else if (unicode_char < 0x800) { // 2-byte sequence (110xxxxx 10xxxxxx)
        utf8_bytes += static_cast<char>(0xC0 | (unicode_char >> 6));
        utf8_bytes += static_cast<char>(0x80 | (unicode_char & 0x3F));
    } else if (unicode_char < 0x10000) { // 3-byte sequence (1110xxxx 10xxxxxx 10xxxxxx)
        utf8_bytes += static_cast<char>(0xE0 | (unicode_char >> 12));
        utf8_bytes += static_cast<char>(0x80 | ((unicode_char >> 6) & 0x3F));
        utf8_bytes += static_cast<char>(0x80 | (unicode_char & 0x3F));
    } else if (unicode_char < 0x110000) { // 4-byte sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        utf8_bytes += static_cast<char>(0xF0 | (unicode_char >> 18));
        utf8_bytes += static_cast<char>(0x80 | ((unicode_char >> 12) & 0x3F));
        utf8_bytes += static_cast<char>(0x80 | ((unicode_char >> 6) & 0x3F));
        utf8_bytes += static_cast<char>(0x80 | (unicode_char & 0x3F));
    } else {
        // Invalid Unicode code point, use replacement character
        utf8_bytes = "\xEF\xBF\xBD"; // U+FFFD in UTF-8
    }
    return utf8_bytes;
}

// DocConvert 类
class DocConvert {
public:
    // 构造函数：将输入的 UTF-8 字符串解码并存储为 Unicode 码点序列。
    DocConvert(const std::string& content_utf8) {
        document_content_unicode = decode_utf8(content_utf8);
    }

    // 将存储的文档内容转换为竖排、自右向左的格式。
    //
    // chars_per_column: 每列的 **Unicode 字符** 数量。
    // output_filepath: 转换结果保存到的文件路径。
    // 返回值: 如果转换和保存成功，则返回 true；否则返回 false。
    bool convert_hor2ver(size_t chars_per_column, const std::string& output_filepath) {
        // 1. 输入验证
        if (chars_per_column == 0) {
            std::cerr << "错误：每列字数不能为零。" << std::endl;
            return false;
        }

        // 2. 打开输出文件
        // 建议以二进制模式打开，以避免操作系统对换行符的自动转换。
        // std::ios::binary 确保字节原样写入，这对于 UTF-8 很重要。
        std::ofstream outfile(output_filepath, std::ios::out | std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "错误：无法打开输出文件：" << output_filepath << std::endl;
            return false;
        }

        // 处理空内容的情况
        if (document_content_unicode.empty()) {
            outfile.close();
            return true; // 空内容，成功输出空文件
        }

        // 3. 计算维度
        size_t total_chars = document_content_unicode.size(); // Unicode字符总数
        // 计算列数（向上取整）
        size_t num_columns = (total_chars + chars_per_column - 1) / chars_per_column;
        // num_rows 在此上下文中即为 chars_per_column，它定义了每列的固定高度。

        // 4. 存储列数据
        // column_data 的每个元素将代表一列 Unicode 码点序列。
        std::vector<std::vector<char32_t>> column_data(num_columns);

        // 从原始内容中填充 column_data
        for (size_t i = 0; i < total_chars; ++i) {
            size_t col_idx = i / chars_per_column;
            column_data[col_idx].push_back(document_content_unicode[i]);
        }

        // 5. 打印到文件（自右向左，每列从上到下）
        char32_t space_char = U' '; // Unicode 空格字符

        for (size_t row = 0; row < chars_per_column; ++row) { // 遍历每一“行”
            // 从右到左遍历列
            for (size_t col_rev_idx = 0; col_rev_idx < num_columns; ++col_rev_idx) {
                size_t col_actual_idx = num_columns - 1 - col_rev_idx; // 获取实际列索引（最右边的列首先被处理）

                // 检查当前行索引是否在当前列内容的范围内
                if (row < column_data[col_actual_idx].size()) {
                    outfile << encode_utf8(column_data[col_actual_idx][row]);
                } else {
                    // 如果当前列比当前行短，则打印一个空格进行填充
                    outfile << encode_utf8(space_char);
                }

                // 在列之间添加一个空格作为分隔符，但最后一列之后不加。
                // 这样在普通文本编辑器中阅读时，不同逻辑列的字符会有区分。
                if (col_rev_idx < num_columns - 1) {
                     outfile << encode_utf8(U' ');
                }
            }
            outfile << '\n'; // 每逻辑“行”结束后换行
        }

        outfile.close();
        return true;
    }

private:
    std::vector<char32_t> document_content_unicode; // 存储解码后的 Unicode 码点
};

// 辅助函数：从文件中读取内容到字符串 (UTF-8)
std::string read_file_to_string(const std::string& filepath) {
    // 以二进制模式打开，避免操作系统对文件内容的修改
    std::ifstream infile(filepath, std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "错误：无法打开输入文件：" << filepath << std::endl;
        return "";
    }
    // 使用流迭代器高效读取文件内容
    std::string content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();
    return content;
}

int main() {

    system("chcp 65001");


    std::string input_content_from_user = "你好世界，这是一个测试文本，用于演示如何将横向文本转换为竖排、自右向左的格式。\n希望你喜欢这个例子！\n如果文本很长，也会正确处理。\n第二行测试。\n第三行也来一些内容。\n结束。";
    // std::string input_content_from_user = "你好世界";

    // 创建 DocConvert 类的实例
    DocConvert converter(input_content_from_user);

    size_t chars_per_column = 2;


    std::string output_filename = "D:\\programs\\cpp\\program_class\\data\\res\\test111.txt";
    if (converter.convert_hor2ver(chars_per_column, output_filename)) {
        std::cout << "转换成功！结果已保存到 " << output_filename << std::endl;
        std::cout << "请确保使用支持 UTF-8 编码的文本编辑器（如 Notepad++, VS Code, Sublime Text, Linux/macOS 默认编辑器）打开文件以正确显示。" << std::endl;
    } else {
        std::cout << "转换失败。" << std::endl;
    }

    return 0;
}