#include "../inc/DocConvert.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream> // 用于文件操作

DocConvert::DocConvert(const std::string& content):raw_content(content){}


int convert_hor2ver(size_t characters_per_column)
/*
 *
 * params: size_t characters_per_column        每列的字数
 * return   是否转化成功
 *
 * 该函数实现将一个连续的string转化成一个2d vector
 */
{


}


int DocConvert::transform_1dto2d(size_t characters_per_row)
/*
 *
 * params: size_t characters_per_row   每行的字符数
 *
 * 该函数实现将一个连续的字符串转成二维向量，其中每行的字数可控
 */ {
    // 从字符串中每次取出一定字数作为一行（一个字符串），遇到全角空格就把该行剩余部分全部用全角空格填充,然后处理下一段


    const size_t PER_CHARACTER_LEN = 3; // 每个UTF-8字符3字节
    const std::string FULL_WIDTH_SPACE = "\xE3\x80\x80"; // 全角空格
    const std::string DOUBLE_FULL_SPACE = FULL_WIDTH_SPACE + FULL_WIDTH_SPACE;

    size_t total_len = this->raw_content.size();
    size_t current_pos = 6;      // 当前的字符索引（单位char）

    std::vector<std::string> current_paragraph;   // 存储当前段落的所有行
    _2d_content.clear();

    while (current_pos < total_len-1) {
        // std::cout << "start a new loop at " << current_pos << std::endl;
        std::vector<std::string> current_line_content; // 存储当前行的字符
        size_t current_line_count = 0; // 当前行的字符数
        bool is_para_break = false; // 标志位，指示是否检测到段落结束符

        // 循环构建一行
        while (current_line_count < characters_per_row && current_pos + PER_CHARACTER_LEN <= total_len) {

            // 判断是否检测到段落结束标志（2个连续的全角空格）
            if (current_pos + 2 * PER_CHARACTER_LEN <= total_len &&
                this->raw_content.substr(current_pos, 2 * PER_CHARACTER_LEN) == DOUBLE_FULL_SPACE)
                {
                is_para_break = true; // 设置标志
                break; // 退出内层循环，表示本行构建结束，需要处理段落结束
            }

            // 处理普通字符
            std::string ch = this->raw_content.substr(current_pos, PER_CHARACTER_LEN);
            current_line_content.push_back(ch);
            current_pos += PER_CHARACTER_LEN;
            current_line_count++;
        }

        // std::cout << "current line has break \n"  << std::endl;
        for (auto t : current_line_content) {
            std::cout  << t ;
        }
        std::cout << std::endl;

        // 内层循环结束，可能是因为一行已满，或raw_content已读完，或遇到了段落结束标记

        // 检测到结束标志，在段落最后一行后面填充全角空格
        if (is_para_break) {
            // std::cout << "has detected paragraph break signal" << std::endl;
            // 1. 填充当前行（如果未满）
            while (current_line_count < characters_per_row) {
                current_line_content .push_back(FULL_WIDTH_SPACE);
                current_line_count++;
            }
            // 2. 将填充后的当前行添加到当前段落
            if (!current_line_content.empty()) {
                this->_2d_content.push_back(current_line_content);
            }


            // 4. 重置，为下一个段落做准备
            current_paragraph.clear();
            is_para_break = false;

            // 5. 跳过两个全角空格的长度
            current_pos += 2 * PER_CHARACTER_LEN;

            // 开始处理新段落
            continue;
        }

        // 没有检测到段落结束标记，可能是当前行已满或者到达文本最后一行

        // 处理文本最后一行
        while (current_line_count < characters_per_row) {
            current_line_content.push_back(FULL_WIDTH_SPACE);
            current_line_count++;
        }

        // 将填充完整的行添加
        if (!current_line_content.empty()) {
            this->_2d_content.push_back(current_line_content);
        }

        std::cout << "current line size: " << current_line_content.size() << '\n' << std::endl;
    }

    std::cout << "transformed successfully \n " << this->_2d_content.size() << ' ' << this->_2d_content.back().size() << ',' << std::endl;
    for (const auto& t : this->_2d_content) {
        for (const auto& i :t ) {
            std::cout << i;
        }
        std::cout << std::endl;
    }

    std::string filename = "D:\\programs\\cpp\\program_class\\data\\res\\test05_temp.txt";
    std::ofstream output_file(filename);
    // 检查文件是否成功打开
    if (output_file.is_open()) {
        // 遍历外层 vector，每一项代表一行
        for (const std::vector<std::string>& line_parts : this->_2d_content) {
            // 将内层 vector<string> 中的所有部分拼接成一个完整的行
            std::string full_line = "";
            for (const std::string& part : line_parts) {
                full_line += part;
            }

            // 也可以使用 std::accumulate 来拼接字符串 (需要 <numeric> 头文件)
            // std::string full_line = std::accumulate(line_parts.begin(), line_parts.end(), std::string(""));

            // 将拼接好的完整行写入文件，并添加换行符
            output_file << full_line << std::endl;
        }
        // 关闭文件流
        output_file.close();
        std::cout << "文本已成功写入文件：" << filename << std::endl;
    } else {
        std::cerr << "无法打开文件进行写入：" << filename << std::endl;
    }


        // 返回段落的数量
        return static_cast<int>(this->_2d_content.size());
    }







int DocConvert::transpose_matrix(void)
/*
 *
 * 该函数实现对一个矩阵做”转置“运算，其中转置是 (i,j) -> (n-j-1,i)
 */ {


    size_t rows = this->_2d_content.size();  // 原矩阵行数
    size_t cols = this->_2d_content[0].size();   // 原矩阵列数
    if (rows==0 || cols==0) {
        return -1;
    }

    std::cout << "origin matrix rows, cols: " << rows << ", " << cols << std::endl;
    std::vector<std::vector<std::string>> transposed_matrix(cols, std::vector<std::string>(rows));
    std::cout << "transposed_matrix size:" << transposed_matrix.size() << ',' << transposed_matrix[0].size() << std::endl;
    for (size_t i = 0; i < cols; ++i) {
        for (size_t j = 0; j < rows; ++j) {
            transposed_matrix[i][j] = this->_2d_content[rows-j-1][i];
            // transposed_matrix[i][j] = this->_2d_content[j][cols - 1 - i];
        }
    }

    //
    // std::cout << "transposed matrix: " << std::endl;
    // for (const auto& row : transposed_matrix) {
    //     for (const auto& col : row) {
    //         std::cout << col ;
    //     }
    // }
    std::cout << std::endl;

    this->transformed_content = transposed_matrix;

    std::string filename = "D:\\programs\\cpp\\program_class\\data\\res\\test05_final.txt";
    std::ofstream output_file(filename);
    // 检查文件是否成功打开
    if (output_file.is_open()) {
        // 遍历外层 vector，每一项代表一行
        for (const std::vector<std::string>& line_parts : transposed_matrix) {
            // 将内层 vector<string> 中的所有部分拼接成一个完整的行
            std::string full_line = "";
            for (const std::string& part : line_parts) {
                full_line += part;
            }

            // 也可以使用 std::accumulate 来拼接字符串 (需要 <numeric> 头文件)
            // std::string full_line = std::accumulate(line_parts.begin(), line_parts.end(), std::string(""));

            // 将拼接好的完整行写入文件，并添加换行符
            output_file << full_line << std::endl;
        }
        // 关闭文件流
        output_file.close();
        std::cout << "文本已成功写入文件：" << filename << std::endl;
    } else {
        std::cerr << "无法打开文件进行写入：" << filename << std::endl;
    }

    return 0;

}



void DocConvert::remove_newline(void)
/*
 *
 * 该函数实现将字符串的所有 \n 全部去除
 *
 */ {
    auto new_end = std::remove(this->raw_content.begin(), this->raw_content.end(), '\n');
    // 截断字符串，移除末尾的全角空格
    this->raw_content.erase(new_end, this->raw_content.end());
}


void DocConvert::printf_raw_content(void) {
    std::cout << this->raw_content << std::endl;
    std::cout << "after remove ,size: " << this->raw_content.size() << std::endl;
}