//
// Created by daans on 25-10-15.
//

#ifndef DOCCONVERT_H
#define DOCCONVERT_H
#include <string>


class DocConvert {
public:
    DocConvert(const std::string& content);
    int convert_hor2ver(size_t chars_per_column, const std::string& output_filepath);
private:
    std::string raw_content;
};



#endif //DOCCONVERT_H
