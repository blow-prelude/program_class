//
// Created by daans on 25-10-14.
//

#include "../inc/FileHandle.h"

FileHandle::FileHandle(const std::string &relative_file_path):file_path(relative_file_path) {

}

bool FileHandle::read_file(std::string& content) const {
    /*
     * param: std::string& content 存储读取的文本的字符串
     * return：bool
     *
     * 该函数实现读取指定文本中的内容，并存储到字符串中
     */
    // 使用ifstream打开文件，会自动关闭文件
    std::ifstream file(this->file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << this->file_path << std::endl;
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
    std::cout << "successfully read " << std::endl;
    return true;
}

bool FileHandle::write_file(const std::string& content) const {
    /*
     * param: const std::string& content 存储文本的字符串
     *
     * 该函数实现将string对象里的内容写入指定文件中
     */

    // std::ofstream file(this->file_path, std::ios::out | std::ios::binary);
    std::ofstream output_file(this->file_path);

    if (!output_file.is_open()) {
        std::cerr << "cannot write into:" <<  this->file_path << std::endl;
        return false;
    }

    // output_file.write(content.data(), content.size());
    output_file << content;
    // 关闭文件流
    output_file.close();
    std::cout << "content has written into:" << this->file_path << std::endl;

    return true;
}


bool FileHandle::write_file(const std::vector<std::vector<std::string>>& matrix_content) const
{
    // 检测是否是二维向量
    if (matrix_content.size()<=0 || matrix_content[0].size()<=0) {
        std::cerr << "matrix has not suitable size" << std::endl;
        return false;
    }

    std::ofstream output_file(this->file_path);
    // 检查文件是否成功打开
    if (output_file.is_open()) {
        // 遍历外层 vector，每一项代表一行
        for (const std::vector<std::string>& line_parts : matrix_content) {
            // 将内层 vector<string> 中的所有部分拼接成一个完整的行
            std::string full_line = "";
            for (const std::string& part : line_parts) {
                full_line += part;
            }

            // 也可以使用 std::accumulate 来拼接字符串 (需要 <numeric> 头文件)
            // std::string full_line = std::accumulate(line_parts.begin(), line_parts.end(), std::string(""));

            // 将拼接好的完整行写入文件，并添加换行符
            output_file << full_line << std::endl;
        }

        // 关闭文件流
        output_file.close();
        std::cout << "content has written into:" << this->file_path << std::endl;
    }

    else {
        std::cerr << "cannot write into:" <<  this->file_path << std::endl;
    }
    return true;
}