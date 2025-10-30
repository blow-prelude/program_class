//
// Created by daans on 25-10-30.
//

#include "../Algorithm/inc/FileHandle.h"
#include "../Algorithm/inc/DocCount.h"
#include "../Algorithm/inc/DocProcess.h"

int main() {
    system("chcp 65001");
    const char* raw_filename = "../data/raw/test01.txt";
    const char* processed_filename = "../data/res/test01_process.txt"; // 待统计文件
    const char* output_filename = "../data/res/test01_count.txt"; // 结果保存文件

    FileHandle raw_file(raw_filename);
    FileHandle convert_file("../data/res/test01_convert.txt");
    FileHandle processed_file(processed_filename);

    std::string raw_content;
    // 读取原始字符串
    bool is_true = raw_file.read_file(raw_content);
    if (!is_true) {
        std::cerr << "Error reading file" << std::endl;
        return 1;
    }

    // 处理文本
    DocProcess doc_process;
    // std::cout << "raw content:\n" << raw_content << std::endl;
    // 更改标点
    std::string convert_content = doc_process.convert_punctuation(raw_content);
    // std::cout << "after convert:\n" << processed_content << std::endl;
    is_true = convert_file.write_file(convert_content);
    if (!is_true) {
        std::cerr << "cannot open and write the after_convert file." << std::endl;
    }
    // 格式化文本
    std::string remove_content = doc_process.process_document(convert_content);
    std::cout << "after process:\n" << remove_content << std::endl;
    is_true = processed_file.write_file(remove_content);
    if (!is_true) {
        std::cerr << "cannot open and write the after_remove file." << std::endl;
    }
    else {

    }

    // 文档统计
    FILE* fp = fopen(processed_filename, "rb"); // 改为"rb"（二进制模式），适配UTF-8读取

    // 检查文件是否成功打开
    if (fp == NULL) {
        perror("无法打开文件（统计主流程）");
        return 1;
    }
    // 执行各项统计（用rewind重置文件指针到开头）
    int para_cnt = count_paragraphs_in_file(fp);
    rewind(fp); // 重置指针，避免后续统计读取位置错误
    int sent_cnt = count_sentence_in_file(fp);
    rewind(fp);
    int total_cnt = count_total_in_file(fp);
    fclose(fp); // 关闭用于统计的文件（statFromFile会重新打开）
    // 读取文件内容并显示（单独打开，不影响前序统计）
    int read_res = read_chinese_txt(processed_filename);
    if (read_res != 0) {
        printf("读取文件内容失败\n");
        return read_res;
    }
    // 统计字符频次
    int stat_res = statFromFile(processed_filename);
    if (!stat_res) {
        printf("字符频次统计失败\n");
        return 1;
    }
    // 显示统计结果到控制台
    printf("\n【控制台统计摘要】\n");
    printf("----------------------------------------\n");
    printf("段落总数: %d\n", para_cnt);
    printf("句子总数：%d\n", sent_cnt);
    printf("总字数：%d\n", total_cnt);
    printSortedResult();
    // 关键：调用新增函数，将所有结果保存到文件
    int save_res = save_all_results(processed_filename, output_filename, para_cnt, sent_cnt, total_cnt);
    if (save_res != 0) {
        printf("保存统计结果失败\n");
        return save_res;
    }
    return 0;
}