//
// Created by daans on 25-10-27.
//

#include <iostream>
#include <string>
#include "../Algorithm/inc/FileHandle.h"
int main() {
    system("chcp 65001");

    std::string contents;
    FileHandle raw_file("../data/res/test01_process.txt");
    bool res = raw_file.read_file(contents);
    if (res) {
        size_t contents_size = contents.size();
        size_t contents_len = contents.length();
        std::cout << "size of full space: " << sizeof("3") << std::endl;
        std::cout << "size of space with double: " << sizeof("2") << std::endl;
        std::cout << "size of space with single: " << sizeof('4') << std::endl;

        std::cout << "size of content: " << contents_size << std::endl;
        std::cout << "length of content: " << contents_len << std::endl;
        if (contents_len <= 0) {
            std::cout << "contents is empty" << std::endl;
            return -1;
        }
        // size_t current_pos = 0;
        //
        //
        // current_pos = contents.find_first_of('\n', current_pos);
        std::cout << contents.substr(0, 210) << std::endl;


    }
    return 0;
}

