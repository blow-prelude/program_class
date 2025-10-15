### 程序设计上机任务分配
<font size=5>***组长 1005***</font>

#### 获取文档  <font color="blue">--1006</font>
#### 代码编辑 
1. 读取文档，保存到string变量中  <font color="blue">--1005</font>
    - 封装一个 FileHandle类 `FileHandle(const std::string& relative_file_path);`，主要封装读写函数，`bool read_file(std::string& content) const;` `bool write_file(const std::string& content);`
2. 处理文档  <font color="blue">--1005</font>
    - 封装一个 DocProcess类 `StrProcess(const std::string& content)`，主要封装标点转化函数 `std::string convert_punctuation(const std::string& content);` 删除段尾空格函数 `std::string remove_space(const std::string& content);` 控制缩进、删除多余空格则封装在处理文本函数 `std::string process_document(const std::string& input_content)`
3. 统计  <font color="blue">--1004、1006</font>
    - 封装一个 DocCount类 `DocCount(const std::string& content);`，主要封装统计段落总数 ` size_t get_para_num() const;` 统计句子总数 `size_t get_centense_num() const;`  统计总字数 `size_t get_character_num() const;` 排序字符频率列表 `std::vector<std::pair<std::string, size_t>> sort_freq() ;`
4. 文档转化  <font color="blue">--1005</font>
    - 封装一个 DocConvert类 `DocConvert(const std::string& content) ;` 主要封装转换函数 ` bool convert_hor2ver(size_t chars_per_column,const std::string& output_filepath); `

#### 报告撰写 <font color="blue">--1004、1006</font>

#### 任务汇总<font color="blue">--1005</font>

### 编码要求
1. 所有功能在函数或类中实现，提供良好的接口
2. 变量规范，添加注释
3. IDE使用Clion