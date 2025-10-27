//
// Created by daans on 25-10-15.
//

#include "../inc/DocConvert.h"

#include <iostream>
#include <ostream>

DocConvert::DocConvert(const std::string& content): raw_content(content){}


int DocConvert::convert_hor2ver(
    size_t ver_character,
    const std::string& output_filepath) {
    /*
     * params: size_t ver_character   转化后的文档每列的字数
     * params: const std::string& output_filepath   转化后保存到的文件路径
     *
     * 本函数实现将水平排列的文档转化为竖直排列
     */
    // 确保每行字数不为0且不大于全部文档的字数
    if (ver_character == 0 || ver_character >= this->raw_content.size()) {
        std::cerr << "invaild character of per volumn" << std::endl;
        return -1;
    }

    // 检查文档是否为空
    if (this->raw_content.empty()) {
        std::cerr << "empty content" << std::endl;
        return 1;
    }

}