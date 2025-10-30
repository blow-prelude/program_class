//
// Created by daans on 25-10-15.
//

#include "../Algorithm/inc/FileHandle.h"
#include "../Algorithm/inc/DocProcess.h"



int main() {
    system("chcp 65001");
    FileHandle raw_file("../data/raw/test02.txt");
    FileHandle convert_file("../data/res/test02_convert.txt");
    FileHandle processed_file("../data/res/test02_process.txt");

    std::string raw_content;
    bool no_err = raw_file.read_file(raw_content);
    if (no_err) {
        DocProcess doc_process;
        // std::cout << "raw content:\n" << raw_content << std::endl;
        std::string convert_content = doc_process.convert_punctuation(raw_content);
        // std::cout << "after convert:\n" << processed_content << std::endl;
        no_err = convert_file.write_file(convert_content);
        if (!no_err) {
            std::cerr << "cannot open and write the after_convert file." << std::endl;
        }

        std::string remove_content = doc_process.process_document(convert_content);
        std::cout << "after process:\n" << remove_content << std::endl;
        no_err = processed_file.write_file(remove_content);
        if (!no_err) {
            std::cerr << "cannot open and write the after_remove file." << std::endl;
        }
        else {

        }
    }
    else {
        std::cerr << "cannot open and read raw content file." << std::endl;
        return -1;
    }


    return 0;
}
