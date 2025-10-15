//
// Created by daans on 25-10-15.
//

#include "../algorithm/inc/FileHandle.h"
#include "../algorithm/inc/DocProcess.h"


int main() {

    system("chcp 65001"); // 设置控制台为 UTF-8 编码
    DocProcess doc_process;
    // 使用原始字符串字面量来确保字符串内部的引号
    std::string my_content = R"(我是王天睿,我正在做程序设计上机的标点转换测试."这个需要注意引号,'尤其'是引号的作用" )";
    std::cout << "before process: " << my_content << std::endl;
    const std::string processed_content = doc_process.convert_punctuation(my_content);
    std::cout << "after process: " << processed_content << std::endl;

    return 0;
}