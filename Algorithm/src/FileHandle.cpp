//
// Created by daans on 25-10-14.
//

#include "../inc/FileHandle.h"

FileHandle::FileHandle(const std::string &relative_file_path):_file_path(relative_file_path) {

}

bool FileHandle::read_file(std::string& content) const {
    /*
     * param: std::string& content 存储读取的文本的字符串
     * return：bool
     *
     * 该函数实现读取指定文本中的内容，并存储到字符串中
     */
    // 使用ifstream打开文件，会自动关闭文件
    std::ifstream file(this->_file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // 定位到文件末尾获取文件大小
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 预留足够空间
    content.resize(static_cast<size_t>(size));

    if (!file.read(&content[0], size)) {
        return false;
    }

    return true;
}

bool FileHandle::write_file(const std::string& content) const {
    /*
     * param: const std::string& content 存储文本的字符串
     *
     * 该函数实现将string对象里的内容写入指定文件中
     */

    std::ofstream file(this->_file_path, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    file.write(content.data(), content.size());

    return !file.fail();
}
