//
// Created by daans on 25-10-15.
//

#ifndef DOCCONVERT_H
#define DOCCONVERT_H
#include <string>
#include <vector>
#include "FileHandle.h"


class DocConvert {
public:
    DocConvert(const std::string& content,const std::string& temp_filename,const std::string& final_filename);
    int convert_hor2ver(size_t chars_per_column);
    int transform_1dto2d(size_t chars_per_row);      // 将连续的字符串转成二维向量
    int transpose_matrix(void);         // 对矩阵做“转置”运算，是自定义的转置



    void remove_newline(void);     // 去除所有的\n
    int save_into_txt(void);       // 将结果保存到文本文件中

    std::vector<std::string> add_space(std::vector<std::string> &line);    // 对于一行文本，给
private:
    std::string raw_content;    // 原始的一维字符串
    std::vector<std::vector<std::string>> _2d_content;     // 转化得到的二维向量文本
    std::vector<std::vector<std::string>> transformed_content;   // 变换得到的纵向排列的二维向量文本
    std::vector<std::vector<std::string>> final_content;

    // 用于存储转化为二维向量的文本
    std::string _2d_filename;
    FileHandle _2d_file_handle;

    // 用于存储最终纵向排列的文本
    std::string transformed_filename;
    FileHandle transformed_file_handle;

};



#endif //DOCCONVERT_H
