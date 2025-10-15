//
// Created by daans on 25-10-14.
//

#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include <iostream>
#include <string>
#include <fstream>

class FileHandle {
public:
    FileHandle(const std::string& relative_file_path);
    // ~FileHandle();
    bool read_file(std::string& content) const;   // 从文件中读取字符，存储在string类对象里
    bool write_file(const std::string& content) const;  // 将string类对象里的内容写入文件中


private:
    const std::string &_file_path;   // 文件相对路径

};



#endif //FILEHANDLE_H
