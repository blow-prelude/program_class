

### 用于格式转化的类（横行排版转竖行排版）
```
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
```
**其中可以直接调用，一体化完成转化的函数**
```
int DocConvert::convert_hor2ver(size_t characters_per_column)
/*
 *
 * params: size_t characters_per_column        每列的字数
 * return   是否转化成功
 *
 * 该函数实现将一个连续的string转化成一个2d vector
 */
{
    // 删除\n
    this->remove_newline();

    // 将连续的字符转化为二维向量
    std::cout <<  "after combine into a whole string ,size is " << this->raw_content.size() << std::endl;
    int out_size = this->transform_1dto2d( characters_per_column);
    if (out_size == 0) {
        std::cerr << "2d vector has no size "<< std::endl;
        return -1;
    }
    std::cout << "sucessfully to do transform from 1d to 2d" << std::endl;


    // 做转置运算
    int no_err = this->transpose_matrix();
    if (no_err != 0) {
        std::cerr << "fail to do  transpose " << std::endl;
        return -1;
    }
    std::cout << "sucessfully to do transposition " << std::endl;


    // 在横行每2个字符之间添加空格
    for (std::vector<std::string> &line:this->transformed_content) {
        std::vector<std::string> new_line = this->add_space(line);

        this->final_content.push_back(new_line);
        // 打印，测试
        // for (const auto& t:new_line ) {
        //     std::cout << t ;
        // }
        // std::cout << std::endl;

    }
    std::cout << "final matrix size: " << this->final_content.size() << ',' << this->final_content[0].size() << std::endl;

    // 保存
    no_err = this->save_into_txt();
    if (no_err != 0) {
        std::cerr << "Error while saving the output file." << std::endl;
    }
    return 0;
}

```


### 用于格式化文本的类
```
class DocProcess {
public:
    // DocProcess();
    // ~DocProcess();
    std::string convert_punctuation(const std::string& content);     // 标点转换函数

    std::string process_document(const std::string& raw_content);
    std::string process_document(const std::string& raw_content,size_t row_length);



    std::string remove_tail_space(const std::string& line_content);            // 删除段尾空格
    std::string remove_head_space(const std::string& line_content);            // 删除段尾空格
    size_t count_hor_character(const std::string &line);           // 统计一行的字数（字节数）

    size_t return_hor_chars() { return this->hor_chars; }

    void format_para( const std::vector<std::string>& para_lines,std::vector<std::string>& formatted_para_lines);    // 控制首行缩进2格

private:
    const std::string FULL_SPACE = "　"; // 这是 U+3000 的 UTF-8 编码，占 3 字节
    size_t FULL_SPACE_LEN = FULL_SPACE.length();

    size_t hor_chars = 0;       // 一行文本的size（字节数）
    bool is_count_hor = false;  // 信号量，只计算一次一行文本的数量，减少计算量

    // 英文字符->中文字符的映射表
    std::map<char, std::string> replacements = {
        {',', "，"},
        {'.', "。"},
        {'?', "？"},
        {'!', "！"},
        {':', "："},
        {';', "；"},
        {'(', "（"},
        {')', "）"},
        {'[', "【"},
        {']', "】"},
        {'{', "｛"},
        {'}', "｝"},
        {'-', "—"},
    };

};
```

**其中处理单个段落的格式化函数**
```
void DocProcess::format_para(
    /*
     * params: std::vector<std::string>& para_lines   需要处理的段落
     * params: std::vector<std::string>& formatted_para_lines    格式化后的段落
     *
     * 该函数实现将一个段落控制首行2格缩进
     */
    const std::vector<std::string>& para_lines,
    std::vector<std::string>& formatted_para_lines)
{
    if (para_lines.empty()) {
        return;
    }

    std::string paragraph_buffer;
    // 首行缩进
    // 先统一删除段落开头的符号，然后加上2个全角空格
    std::string first_para_line = this->remove_head_space(para_lines[0]);
    std::cout << "first line of paragraph:" << first_para_line << std::endl;
    // 如果段落只有一行，直接结束
    if (para_lines.size() == 1) {
        formatted_para_lines.push_back(FULL_SPACE + FULL_SPACE + first_para_line);
        return;
    }

    // 如果段落不止一行，可以把第一行的字数作为每行的标准字数
    else {
        // 只计算一次，降低计算量
        if (this->is_count_hor==false) {
            this->hor_chars = this->count_hor_character(first_para_line);
            this->is_count_hor = true;

        }

        size_t hor_bytes = this->return_hor_chars();

        // 将整个段落的文本内容合并成一个连续的字符串
        std::string combined_lines;
        // 预估总长度以减少内存重新分配
        size_t total_len = hor_bytes+ 2*this->FULL_SPACE_LEN;
        for (size_t i = 1; i < para_lines.size(); ++i) {
            total_len += para_lines[i].length();
        }
        combined_lines.reserve(total_len);

        combined_lines.append(first_para_line);
        for (size_t i = 1; i < para_lines.size(); ++i) {
            combined_lines.append(para_lines[i]);
        }


        // 遍历合并后的内容，根据目标长度进行分行和缩进
        size_t current_pos = 0; // 当前在combined_paragraph_content中读取的起始位置
        bool is_first_output_line = true; // 标记是否为即将输出的第一行

        while (current_pos < combined_lines.length()) {
            std::string current_output_line_str;
            size_t current_line_chars; // 当前行需要从combined_paragraph_content中取出的字节数

            if (is_first_output_line) {
                // 首行添加两个全角空格的缩进
                current_output_line_str.append(FULL_SPACE + FULL_SPACE);

                size_t indentation_bytes = 2 * FULL_SPACE_LEN;
                if (hor_bytes > indentation_bytes) {
                    current_line_chars = hor_bytes - indentation_bytes;
                } else {
                    // 如果目标总长度小于或等于缩进长度，则首行内容部分为空或非常短
                    current_line_chars = 0;
                }
            } else {
                // 后续行不进行额外缩进，内容部分直接取目标总长度
                current_line_chars = hor_bytes;
            }

            // 确保不会读取超出combined_paragraph_content的范围
            size_t actual_bytes_to_extract = std::min(current_line_chars, combined_lines.length() - current_pos);

            std::string segment = combined_lines.substr(current_pos, actual_bytes_to_extract);
            current_output_line_str.append(segment);
            formatted_para_lines.push_back(current_output_line_str);


            // 更新读取位置，为下一行做准备
            current_pos += actual_bytes_to_extract;
            is_first_output_line = false; // 标记为非首行，以便下一次循环处理

        }
    }

}
```