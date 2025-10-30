//
// Created by daans on 25-10-27.
//


#include "../Algorithm/inc/FileHandle.h"
#include "../Algorithm/inc/DocConvert.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>



// 全角空格（UTF-8，3字节）
const std::string FULL_WIDTH_SPACE = "　";
const size_t CHAR_BYTES = 3; // 中文字符字节数

// int main() {
//     system("chcp 65001");
//     std::string input_str = "这是一个测试文本，用于将一段文本从一维拓展为二维。";
//     const size_t rows = 3; // 固定行数
//
//     // 步骤1：计算矩阵维度并预处理文本
//     size_t total_bytes = input_str.size();
//     size_t total_chars = total_bytes / CHAR_BYTES; // 总字符数（向下取整）
//     if (total_chars == 0) {
//         std::cout << "无有效字符" << std::endl;
//         return 0;
//     }
//
//     // 列数cols = 每行最大字符数（向上取整）
//     size_t cols = (total_chars + rows - 1) / rows;
//     std::cout << "原矩阵维度：" << rows << "行 × " << cols << "列" << std::endl;
//
//     // 步骤2：构建原矩阵（rows × cols），不足位补全角空格
//     std::vector<std::vector<std::string>> original_matrix(rows, std::vector<std::string>(cols, FULL_WIDTH_SPACE));
//     size_t char_idx = 0; // 当前字符索引
//
//     for (size_t i = 0; i < rows; ++i) { // 行（上到下）
//         for (size_t j = 0; j < cols; ++j) { // 列（左到右）
//             if (char_idx < total_chars) {
//                 // 提取当前字符（3字节）
//                 size_t byte_pos = char_idx * CHAR_BYTES;
//                 original_matrix[i][j] = input_str.substr(byte_pos, CHAR_BYTES);
//                 char_idx++;
//             } else {
//                 // 字符用完，用全角空格填充
//                 original_matrix[i][j] = FULL_WIDTH_SPACE;
//             }
//         }
//     }
//
//     // 步骤3：矩阵转置（cols × rows）
//     std::vector<std::vector<std::string>> transposed_matrix(cols, std::vector<std::string>(rows));
//     for (size_t i = 0; i < rows; ++i) {
//         for (size_t j = 0; j < cols; ++j) {
//             transposed_matrix[j][i] = original_matrix[i][j]; // 行转列，列转行
//         }
//     }
//
//     // 步骤4：列反转（转置矩阵的列顺序反转，实现从右到左）
//     std::vector<std::vector<std::string>> result_matrix;
//     for (auto& col : transposed_matrix) {
//         std::reverse(col.begin(), col.end()); // 反转每一列（原转置矩阵的行）
//         result_matrix.push_back(col);
//     }
//     // 反转整个矩阵的列顺序（最右列放最前）
//     std::reverse(result_matrix.begin(), result_matrix.end());
//
//     // 步骤5：输出结果（每行对应目标排列的一列，从上到下）
//     std::cout << "\n原矩阵（左到右，上到下）：" << std::endl;
//     for (const auto& row : original_matrix) {
//         for (const auto& c : row) {
//             std::cout << c;
//         }
//         std::cout << std::endl;
//     }
//
//     std::cout << "\n转置矩阵（上到下,左到右）：" << std::endl;
//     for (const auto& col : transposed_matrix) {
//         for (const auto& c : col) {
//             std::cout << c;
//         }
//         std::cout << std::endl;
//     }
//     std::cout << "\n转换后（从上到下，从右到左）：" << std::endl;
//     for (const auto& col : result_matrix) {
//         for (const auto& c : col) {
//             std::cout << c;
//         }
//         std::cout << std::endl;
//     }
//
//     return 0;
// }

int main() {
    system("chcp 65001");


    const std::string input_path = "../data/res/test01_process.txt";
    const std::string output_path = "../data/res/test01_final.txt";
    const std::string temp_path = "../data/res/test01_temp.txt";

    FileHandle input_handle = FileHandle(input_path);


    std::string input_content;
     bool err = input_handle.read_file(input_content);
    if (not err) {
        std::cerr << "cannot open and read" << input_path << std::endl;
    }
    std::cout << "raw content size: " << input_content.size() << std::endl;
    // std::cout << input_content << std::endl;


    DocConvert final_handle = DocConvert(input_content,temp_path,output_path);
    size_t per_row = 14;

    int no_err = final_handle.convert_hor2ver(per_row);
    if (no_err != 0) {
        std::cerr << "except at somewhere" << std::endl;
        return 1;
    }
    return 0;
}