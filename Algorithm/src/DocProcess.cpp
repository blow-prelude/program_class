//
// Created by daans on 25-10-15.
//

#include "../inc/DocProcess.h"

std::string DocProcess::convert_punctuation(const std::string& content) {
    /*
     *
     * params:std::string& content 原始的文本
     * return：std::string 进行标点转换后的字符串
     *
     * 这个函数主要实现标点的转换
     */

    std::string output_content;
    output_content.reserve(content.length() * 3);

    // 用于跟踪引号状态，确保开闭合正确
    bool in_double_quote = false;
    bool in_single_quote = false;

    // 遍历每个字节
    for (char c : content) {
        // 查找是否在直接替换的映射中
        auto it = this->replacements.find(c);
        if (it != this->replacements.end()) {
            output_content.append(it->second);
        }

        // 处理英文双引号
        else if (c == '"') {
            if (!in_double_quote) {
                output_content.append("“"); // 第一次遇到，作为开引号
                in_double_quote = true;
            } else {
                output_content.append("”"); // 第二次遇到，作为闭引号
                in_double_quote = false;
            }
        }
        // 处理英文单引号
        else if (c == '\'') {
            if (!in_single_quote) {
                output_content.append("‘"); // 第一次遇到，作为开引号
                in_single_quote = true;
            } else {
                output_content.append("’"); // 第二次遇到，作为闭引号
                in_single_quote = false;
            }
        }
        // 如果不是需要转换的标点符号，则原样添加字符
        else {
            output_content.push_back(c);
        }
    }

    return output_content;
    }

void DocProcess::format_para(
    /*
     * params: std::vector<std::string>& para_lines   需要处理的段落
     * params: std::vector<std::string>& formatted_para_lines    格式化后的段落
     *
     * 该函数实现将一个段落控制首行2格缩进
     */
    const std::vector<std::string>& para_lines,
    std::vector<std::string>& formatted_para_lines)
{
    if (para_lines.empty()) {
        return;
    }

    std::string paragraph_buffer;
    // 首行缩进
    // 先统一删除段落开头的符号，然后加上2个全角空格
    std::string first_para_line = this->remove_head_space(para_lines[0]);
    std::cout << "first line of paragraph:" << first_para_line << std::endl;
    // 如果段落只有一行，直接结束
    if (para_lines.size() == 1) {
        formatted_para_lines.push_back(FULL_SPACE + FULL_SPACE + first_para_line);
        return;
    }

    // 如果段落不止一行，可以把第一行的字数作为每行的标准字数
    else {
        // 只计算一次，降低计算量
        if (this->is_count_hor==false) {
            this->hor_chars = this->count_hor_character(first_para_line);
            this->is_count_hor = true;

        }

        size_t hor_bytes = this->return_hor_chars();

        // 将整个段落的文本内容合并成一个连续的字符串
        std::string combined_lines;
        // 预估总长度以减少内存重新分配
        size_t total_len = hor_bytes+ 2*this->FULL_SPACE_LEN;
        for (size_t i = 1; i < para_lines.size(); ++i) {
            total_len += para_lines[i].length();
        }
        combined_lines.reserve(total_len);

        combined_lines.append(first_para_line);
        for (size_t i = 1; i < para_lines.size(); ++i) {
            combined_lines.append(para_lines[i]);
        }


        // 遍历合并后的内容，根据目标长度进行分行和缩进
        size_t current_pos = 0; // 当前在combined_paragraph_content中读取的起始位置
        bool is_first_output_line = true; // 标记是否为即将输出的第一行

        while (current_pos < combined_lines.length()) {
            std::string current_output_line_str;
            size_t current_line_chars; // 当前行需要从combined_paragraph_content中取出的字节数

            if (is_first_output_line) {
                // 首行添加两个全角空格的缩进
                current_output_line_str.append(FULL_SPACE + FULL_SPACE);

                size_t indentation_bytes = 2 * FULL_SPACE_LEN;
                if (hor_bytes > indentation_bytes) {
                    current_line_chars = hor_bytes - indentation_bytes;
                } else {
                    // 如果目标总长度小于或等于缩进长度，则首行内容部分为空或非常短
                    current_line_chars = 0;
                }
            } else {
                // 后续行不进行额外缩进，内容部分直接取目标总长度
                current_line_chars = hor_bytes;
            }

            // 确保不会读取超出combined_paragraph_content的范围
            size_t actual_bytes_to_extract = std::min(current_line_chars, combined_lines.length() - current_pos);

            std::string segment = combined_lines.substr(current_pos, actual_bytes_to_extract);
            current_output_line_str.append(segment);
            formatted_para_lines.push_back(current_output_line_str);


            // 更新读取位置，为下一行做准备
            current_pos += actual_bytes_to_extract;
            is_first_output_line = false; // 标记为非首行，以便下一次循环处理

        }
    }

}


std::string DocProcess::remove_tail_space(const std::string& line_content) {
    /*
     *
     * params:const std::string& line_content   每一行的文本
     * return：std::string   删除句末空格的的文本
     *
     * 该函数实现删除给定文本句末的ascii空格、tab、enter键，以及全角空格
     */
    // 检查内容是否为空
    if (line_content.empty()) {
        return line_content;
    }

    size_t current_len = line_content.length();
    while (current_len > 0) {
        // 检查是否是 ASCII 空格 (1 字节), 制表符或回车符
        if (line_content[current_len - 1] == ' ' ||
            line_content[current_len - 1] == '\t' ||
            line_content[current_len - 1] == '\r') {
            current_len--;
            }
        // 检查是否是全角空格 (3 字节: UTF-8 编码为 E3 80 80)
        // 确保有足够的字节进行检查，并进行字符串比较
        else if ((current_len >= FULL_SPACE_LEN) &&
                 line_content.substr(current_len - FULL_SPACE_LEN, FULL_SPACE_LEN) == FULL_SPACE) {
            current_len -= FULL_SPACE_LEN; // 移除 FULL_WIDTH_SPACE 对应的字节数 (通常是 3)
                 }
        // 如果不是需要移除的空白符，则停止
        else {
            break;
        }
    }
    return line_content.substr(0, current_len);
}


std::string DocProcess::remove_head_space(const std::string& line_content) {
    /*
     *
     * params:const std::string& line_content   每一行的文本
     * return：std::string   删除句末空格的的文本
     *
     * 该函数实现删除给定文本句末的ascii空格、tab、enter键，以及全角空格
     */
    // 检查内容是否为空
    if (line_content.empty()) {
        return line_content;
    }
    size_t total_len = line_content.length();
    size_t idx = 0;
    while (idx < total_len) {
        // 检查是否是ASCII空白字符
        if (line_content[idx] == ' ' || line_content[idx] == '\t' || line_content[idx] == '\r') {
            idx++;
        }
        // 检查是否为全角空格（确保有足够的字节）
        else if (idx + 1 < total_len &&
                 line_content[idx] == FULL_SPACE[0] &&
                 line_content[idx+1] == FULL_SPACE[1]) {
            idx += 2;
                 }
        // 遇到非空白字符，停止处理
        else {
            break;
        }
    }

    return line_content.substr(idx);

}


size_t DocProcess::count_hor_character(const std::string &line) {
    /*
     * params: std::string &line   一行的文本
     * return: size_t  该行文本的字数
     *
     * 该函数实现统计一行文本的size（字节数）
     */

    // 首先判断该行是否被格式化处理过
    if (line[0]==' ' || line[0]=='\t' || line[0]=='\r' ||
        line[0] == FULL_SPACE[0] && line[1] == FULL_SPACE[1]) {
        std::cerr << "except formated line " << std::endl;
        return -1;
    }

    return line.size();
}

std::string DocProcess::process_document(const std::string& raw_content) {
    /*
     *
     * params: std::string& raw_content    未处理的原文本
     * params: size_t row_length    每行的最多字数
     * return: std::string    处理后的字符串
     *
     * 本函数实现删除段尾多余空格，删除段落之间的空行，同时控制段落的首行缩进为2个ascii空格
     * 该函数是基于理想状态，即段落之间必须有空行
     */

    std::string processed_output;
    std::vector<std::string> joined_paras; // 用于存储处理后的有效段落

    // 1. 统一换行符 (将 Windows 风格的 \r\n 转换为 Unix 风格的 \n)
    std::string content_lf = raw_content;
    size_t pos = 0;
    while ((pos = content_lf.find("\r\n", pos)) != std::string::npos) {
        content_lf.replace(pos, 2, "\n");
    }

    // 2. 将文档内容按行分割，并处理每行
    std::istringstream iss(content_lf);
    std::string line;
    std::vector<std::string> current_para; // 临时存储当前正在构建的段落的行
    std::vector<std::string> formatted_content;

    while (std::getline(iss, line)) {
        // 删除段尾空格
        std::string removed_line = this->remove_tail_space(line);


        // 如果该行不为空，把该行添加到当前段落的列表中
        if (!removed_line.empty()) {
            current_para.push_back(removed_line);
        }
        else {
            // 如果当前段落为空，说明当前段落结束，   否则是连续的空行，pass
            if (!current_para.empty()) {
                this->format_para(current_para,formatted_content);
                // 清空当前段落，继续下一轮处理
                current_para.clear();
            }
        }
    }

    // 处理最后一段
    if (!current_para.empty()) {
        this->format_para(current_para,formatted_content);
    }

    // 3. 将格式化的段落用单个换行符连接起来
    for (size_t i = 0; i < formatted_content.size(); ++i) {
        processed_output.append(formatted_content[i]);
        // 在段落之间添加一个换行符
        if (i < joined_paras.size() - 1) {
            processed_output.append("\n");
        }
    }

    return processed_output;
}







