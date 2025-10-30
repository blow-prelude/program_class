//
// Created by daans on 25-10-15.
//

#ifndef DOCPROCESS_H
#define DOCPROCESS_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

class DocProcess {
public:
    // DocProcess();
    // ~DocProcess();
    std::string convert_punctuation(const std::string& content);     // 标点转换函数

    std::string process_document(const std::string& raw_content);
    // std::string process_document(const std::string& raw_content,size_t row_length);



    std::string remove_tail_space(const std::string& line_content);            // 删除段尾空格
    std::string remove_head_space(const std::string& line_content);            // 删除段尾空格
    size_t count_hor_character(const std::string &line);           // 统计一行的字数（字节数）

    size_t return_hor_chars() { return this->hor_chars; }

    void format_para( const std::vector<std::string>& para_lines,std::vector<std::string>& formatted_para_lines);    // 控制首行缩进2格

private:
    const std::string FULL_SPACE = "　"; // 这是 U+3000 的 UTF-8 编码，占 3 字节
    size_t FULL_SPACE_LEN = FULL_SPACE.length();

    size_t hor_chars = 0;       // 一行文本的size（字节数）
    bool is_count_hor = false;  // 信号量，只计算一次一行文本的数量，减少计算量

    // 英文字符->中文字符的映射表
    std::map<char, std::string> replacements = {
        {',', "，"},
        {'.', "。"},
        {'?', "？"},
        {'!', "！"},
        {':', "："},
        {';', "；"},
        {'(', "（"},
        {')', "）"},
        {'[', "【"},
        {']', "】"},
        {'{', "｛"},
        {'}', "｝"},
        {'-', "—"},
    };

};






#endif //DOCPROCESS_H
