//
// Created by daans on 25-10-15.
//

#ifndef DOCCOUNT_H
#define DOCCOUNT_H
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>

int read_chinese_txt(const char* filename);   // 读取并显示文件内容
int is_utf8_full_width_space(unsigned char b1, unsigned char b2, unsigned char b3);  // 辅助函数：判断3个连续字节是否为UTF-8编码的全角空格
void skip_line_half_width_space(FILE* fp);    // 辅助函数：跳过行内半角空白字符
int count_paragraphs_in_file(FILE* fp);    // 计算段落数
int count_sentence_in_file(FILE* fp);        // 统计UTF-8文件中汉语句子数
int count_total_in_file(FILE* fp);   // 统计总字数
int isCnChar(unsigned char c1, unsigned char c2, unsigned char c3);     // 判断3字节是否为UTF-8汉语字符/标点
void countCnChar( unsigned char c1,unsigned char c2,unsigned char c3);    // 统计UTF-8字符
int statFromFile(const char* filename);      // 从UTF-8编码文件读取并统计
int compareChar(const void* a, const void* b);    // 排序比较函数
void printSortedResult();     // 打印排序后的统计结果
int save_all_results(const char* input_filename, const char* output_filename, int para_cnt, int sent_cnt, int total_cnt);   // 保存统计结果


#define MAX_CHAR 10000    // 最大支持10000个不同字符


// 字符统计结构体
typedef struct {
    unsigned char ch[3];  // UTF-8字符的3个字节
    int count;            // 出现次数
    wchar_t wch;          // 转换后的宽字符（用于字典排序）
} CharStat;

#endif //DOCCOUNT_H
