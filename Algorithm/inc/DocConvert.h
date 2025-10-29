//
// Created by daans on 25-10-15.
//

#ifndef DOCCONVERT_H
#define DOCCONVERT_H
#include <string>
#include <vector>

class DocConvert {
public:
    DocConvert(const std::string& content);
    int convert_hor2ver(size_t chars_per_column);
    int transform_1dto2d(size_t chars_per_row);      // 将连续的字符串转成二维向量
    int transpose_matrix(void);         // 对矩阵做“转置”运算，是自定义的转置



    void remove_newline(void);     // 去除所有的\n

    void printf_raw_content(void);
private:
    std::string raw_content;
    std::vector<std::vector<std::string>> _2d_content;
    std::vector<std::vector<std::string>> transformed_content;

};



#endif //DOCCONVERT_H
