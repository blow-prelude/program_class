#include "../inc/DocConvert.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream> // 用于文件操作

DocConvert::DocConvert(
    const std::string& content,
    const std::string& temp_filename,
    const std::string& final_filename):
    raw_content(content), _2d_filename(temp_filename),
    transformed_filename(final_filename),
    _2d_file_handle(_2d_filename),
    transformed_file_handle(transformed_filename){

}


int DocConvert::convert_hor2ver(size_t characters_per_column)
/*
 *
 * params: size_t characters_per_column        每列的字数
 * return   是否转化成功
 *
 * 该函数实现将一个连续的string转化成一个2d vector
 */
{
    // 删除\n
    this->remove_newline();

    // 将连续的字符转化为二维向量
    std::cout <<  "after combine into a whole string ,size is " << this->raw_content.size() << std::endl;
    int out_size = this->transform_1dto2d( characters_per_column);
    if (out_size == 0) {
        std::cerr << "2d vector has no size "<< std::endl;
        return -1;
    }
    std::cout << "sucessfully to do transform from 1d to 2d" << std::endl;


    // 做转置运算
    int no_err = this->transpose_matrix();
    if (no_err != 0) {
        std::cerr << "fail to do  transpose " << std::endl;
        return -1;
    }
    std::cout << "sucessfully to do transposition " << std::endl;


    // 在横行每2个字符之间添加空格
    for (std::vector<std::string> &line:this->transformed_content) {
        std::vector<std::string> new_line = this->add_space(line);

        this->final_content.push_back(new_line);
        // 打印，测试
        // for (const auto& t:new_line ) {
        //     std::cout << t ;
        // }
        // std::cout << std::endl;

    }
    std::cout << "final matrix size: " << this->final_content.size() << ',' << this->final_content[0].size() << std::endl;

    // 保存
    no_err = this->save_into_txt();
    if (no_err != 0) {
        std::cerr << "Error while saving the output file." << std::endl;
    }
    return 0;
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
    size_t last_pos = (size_t)-1;

    std::vector<std::string> current_paragraph;   // 存储当前段落的所有行
    _2d_content.clear();

    while (current_pos + PER_CHARACTER_LEN <= total_len) {
        if (current_pos == last_pos) {
            std::cerr << "stuck at " << current_pos << std::endl;
            break; // 防止死循环
        }
        last_pos = current_pos;
        // std::cout << "start a new loop at " << current_pos << std::endl;
        std::vector<std::string> current_line_content; // 存储当前行的字符
        size_t current_line_count = 0; // 当前行的字符数
        bool is_para_break = false; // 标志位，指示是否检测到段落结束符

        // 循环构建一行
        while (current_line_count < characters_per_row && current_pos + PER_CHARACTER_LEN <= total_len) {

            // 确保每次只步进3个字节
            if (current_pos % PER_CHARACTER_LEN != 0) {
                std::cerr << "UTF-8 misalignment at " << current_pos << std::endl;
                current_pos = (current_pos / PER_CHARACTER_LEN + 1) * PER_CHARACTER_LEN;
            }

            // 判断是否检测到段落结束标志（2个连续的全角空格）
            if (current_pos + 2 * PER_CHARACTER_LEN <= total_len &&
                this->raw_content.substr(current_pos, 2 * PER_CHARACTER_LEN) == DOUBLE_FULL_SPACE)
                {
                is_para_break = true; // 设置标志
                current_pos += 2 * PER_CHARACTER_LEN; // 跳过两个全角空格
                break; // 退出内层循环，表示本行构建结束，需要处理段落结束
            }

            // 处理普通字符
            std::string ch = this->raw_content.substr(current_pos, PER_CHARACTER_LEN);
            current_line_content.push_back(ch);
            current_pos += PER_CHARACTER_LEN;
            current_line_count++;
        }

        // 填充行尾
        while (current_line_count < characters_per_row) {
            current_line_content.push_back(FULL_WIDTH_SPACE);
            current_line_count++;
        }

        // 将填充完整的行添加到矩阵中
        if (!current_line_content.empty()) {
            this->_2d_content.push_back(current_line_content);
        }

        // std::cout << "current line has break \n"  << std::endl;

        // 打印当前行
        // for (auto t : current_line_content) {
        //     std::cout  << t ;
        // }
        // std::cout << std::endl;

        // 内层循环结束，可能是因为一行已满，或raw_content已读完，或遇到了段落结束标记

        // 检测到结束标志，在段落最后一行后面填充全角空格
        if (is_para_break) {
            // 开始处理新段落
            continue;
        }




        // std::cout << "current line character counter:" << current_line_count << std::endl;

        // std::cout << "current line size: " << current_line_content.size() << std::endl;
    }

    // std::cout << "transformed successfully \n " << this->_2d_content.size() << ' ' << this->_2d_content.back().size() << ',' << std::endl;
    // for (const auto& t : this->_2d_content) {
    //     for (const auto& i :t ) {
    //         std::cout << i;
    //     }
    //     std::cout << std::endl;
    // }



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

    // std::cout << "origin matrix rows, cols: " << rows << ", " << cols << std::endl;
    std::vector<std::vector<std::string>> transposed_matrix(cols, std::vector<std::string>(rows));
    // std::cout << "transposed_matrix size:" << transposed_matrix.size() << ',' << transposed_matrix[0].size() << std::endl;
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
    // std::cout << std::endl;

    this->transformed_content = transposed_matrix;

    return 0;

}



void DocConvert::remove_newline(void)
/*
 *
 * 该函数实现将字符串的所有 \n 全部去除
 *
 */ {
    auto new_end = std::remove(this->raw_content.begin(), this->raw_content.end(), '\n');
    // 截断字符串，移除末尾的 \n
    this->raw_content.erase(new_end, this->raw_content.end());
    std::cout << "after remove \\n, raw_content size: "<< this->raw_content.size() << std::endl;

}




int DocConvert::save_into_txt(void)
/*
 *
 * 该函数实现将2个二维向量都保存到文本中
 *
 */
{
    bool no_err = this->_2d_file_handle.write_file(this->_2d_content);
    if (no_err==false) {
        return -1;
    }
    no_err = this->transformed_file_handle.write_file(this->final_content);
    if (no_err==false) {
        return -1;
    }
    return 0;

}


std::vector<std::string> DocConvert::add_space(std::vector<std::string> &line)
/*
 *
 * params: std::vector<std::string> &line    存储一行文本的一维向量，每个元素都是一个字符（string类型）
 */
{
    if (line.size() <= 1) {
        return line;
    }

    std::vector<std::string> result;
    result.reserve(2 * line.size() - 1); // 预留足够空间，避免多次扩容

    // 迭代原容器，在元素间插入全角空格
    for (size_t i = 0; i < line.size(); ++i) {
        result.push_back(line[i]);
        // 最后一个元素后不插入空格
        if (i != line.size() - 1) {
            result.push_back("　"); // 全角空格
        }
    }

    return result;

}