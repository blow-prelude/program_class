//
// Created by daans on 25-10-27.
//

#include "../Algorithm/inc/DocConvert.h"
#include "../Algorithm/inc/FileHandle.h"

int main() {
    system("chcp 65001");
    std::string input_path = "../data/res/test05_process.txt";
    std::string output_filename = "../data/res/test05_final.txt";
    size_t chars_per_column = 20;

    // FileHandle input_file = FileHandle(input_path);
    std::string input_str = "你好世界，这是一个测试文本，用于演示如何将横向文本转换为竖排、自右向左的格式。希望你喜欢这个例子！如果文本很长，也会正确处理。\n第二行测试。\n第三行也来一些内容。\n结束。";;
    // input_file.read_file(input_str);
    DocConvert converter(input_str);

    if (converter.convert_hor2ver(chars_per_column, output_filename) == 0) {
        std::cout << "转换成功！结果已保存到 " << output_filename << std::endl;
    } else {
        std::cout << "转换失败。" << std::endl;
    }
    return 0;
}