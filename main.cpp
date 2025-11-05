#include <iostream>
#include "Algorithm/inc/FileHandle.h"
#include "Algorithm/inc/DocCount.h"
#include "Algorithm/inc/DocProcess.h"
#include "Algorithm/inc/DocConvert.h"


int main() {
    system("chcp 65001");

    const std::string root_path = "D:/programs/cpp/program_class/";   // 根路径
    const std::string test = "test01";
    const std::string raw_filename = root_path + "data/raw/" + test + ".txt";
    const std::string change_filename = root_path + "data/res/" + test + "_change.txt";
    const std::string format_filename = root_path + "data/res/"+ test + "_format.txt";
    const std::string no_newline_filename = root_path + "data/res/" + test + "_no_newline.txt";
    const std::string count_filename = root_path + "data/res/"+ test + "_count.txt";
    const std::string _2d_filename = root_path + "data/res/" + test + "_2d.txt";
    // const std::string transpose_filename = root_path + "data/res/test01_transpose.txt";
    const std::string final_filename = root_path + "data/res/" + test + "_final.txt";


    size_t per_col;   // 转化排版后每列的字数
    size_t total_character = 200;



    // 读取原始文本
    FileHandle raw_handle(raw_filename);
    std::string raw_content;
    bool is_open = raw_handle.read_file(raw_content);
    if (!is_open) {
        std::cerr << "[ERR] Error opening file " << raw_filename << std::endl;
        return -1;
    }

    // 格式化文本
    FileHandle change_handle(change_filename);
    FileHandle format_handle(format_filename);
    DocProcess doc_process;
    // 更改标点
    std::string change_content = doc_process.convert_punctuation(raw_content);
    is_open = change_handle.write_file(change_content);
    if (!is_open) {
        std::cerr << "[ERR] Error opening file " << change_filename << std::endl;
        return -1;
    }
    // 格式化所有段落
    std::string formated_content = doc_process.process_document(change_content);
    if (formated_content == "") {
        std::cerr << "[ERR] unexcept in format content" << std::endl;
        return 1;
    }
    is_open = format_handle.write_file(formated_content);
    if (!is_open) {
        std::cerr << "[ERR] Error opening file " << format_filename << std::endl;
        return -1;
    }

    std::cout << "[DEBUG] successfully format content." << std::endl;


    // 统计文本
    FILE* fp = fopen(format_filename.c_str(), "rb");
    // 检查文件是否成功打开
    if (fp == NULL) {
        std::cerr << "[ERR] Error opening file " << format_filename << std::endl;
        return -1;
    }
    // 执行各项统计（用rewind重置文件指针到开头）
    int para_cnt = count_paragraphs_in_file(fp);
    rewind(fp); // 重置指针，避免后续统计读取位置错误
    int sent_cnt = count_sentence_in_file(fp);
    rewind(fp);
    int total_cnt = count_total_in_file(fp);
    fclose(fp); // 关闭用于统计的文件（statFromFile会重新打开）

    int read_res = read_chinese_txt(format_filename.c_str());
    if (read_res != 0) {
        std::cerr << "[ERR] 读取文件内容失败" <<std::endl;
        return read_res;
    }
    // 统计字符频次
    int stat_res = statFromFile(format_filename.c_str(),no_newline_filename.c_str());
    if (!stat_res) {
        std::cerr << "[ERR] 字符频次统计失败" << std::endl;
        return 1;
    }
    // 显示统计结果到控制台
    std::cout << "[INFO] 【控制台统计摘要】" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "[INFO] 段落总数: " << para_cnt << std::endl;
    std::cout << "[INFO] 句子总数：" << sent_cnt << std::endl;
    std::cout << "[INFO] 总字数：" << total_cnt << std::endl;
    printSortedResult();
    // 关键：调用新增函数，将所有结果保存到文件
    int save_res = save_all_results(format_filename.c_str(), count_filename.c_str(), para_cnt, sent_cnt, total_cnt);
    if (save_res != 0) {
        std::cerr << "[ERR] 保存统计结果失败" << std::endl;
        return save_res;
    }
    std::cout << "[INFO] successfully count content." << std::endl;


    // 转化文本排版并保存
    bool is_fit = false;
    while (!is_fit) {
        std::cout << "please enter characters of per col: " << std::endl;
        std::cin >> per_col;
        // 检查字数是否合格
        if (per_col <= 0) {
            std::cerr << "please enter a number which is more than 0" << std::endl;
            continue;
        }
        if (per_col >= total_character) {
            std::cerr << "please enter a number which is less than whole content character" << std::endl;
            continue;
        }
        is_fit = true;
    }
    auto doc_convert = DocConvert(formated_content,_2d_filename,final_filename);
    int no_err = doc_convert.convert_hor2ver(per_col);
    if (no_err != 0) {
        std::cerr << "[ERR] except in convert form hor to ver" << std::endl;
        return 1;
    }

    return 0;
}