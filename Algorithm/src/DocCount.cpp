#define _CRT_SECURE_NO_WARNINGS

#include "../inc/DocCount.h"

// 读取并显示文件内容
int read_chinese_txt(const char* filename) {
    FILE* file = fopen(filename, "rb");  // 使用参数传入的文件名
    if (file == NULL) {
        perror("无法打开文件（读取内容）");
        return 1;
    }

    char buffer[10000];

    printf("文件内容:\n");
    printf("-------------------------\n");

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    printf("-------------------------\n");

    if (ferror(file)) {
        perror("读取文件时发生错误");
        fclose(file);
        return 2;
    }

    if (fclose(file) != 0) {
        perror("关闭文件时发生错误");
        return 3;
    }

    return 0;
}

// 计算段落数
// 辅助函数：判断3个连续字节是否为UTF-8编码的全角空格（UTF-8全角空格：0xE3 0x80 0x80）
int is_utf8_full_width_space(unsigned char b1, unsigned char b2, unsigned char b3) {
    return (b1 == 0xE3 && b2 == 0x80 && b3 == 0x80) ? 1 : 0;
}

// 辅助函数：跳过行内半角空白字符（避免干扰行首判断，不跳过全角空格）
void skip_line_half_width_space(FILE* fp) {
    int ch;
    while ((ch = fgetc(fp)) != EOF && ch != '\n') {
        // 仅跳过半角空白（空格/制表符等，isspace()不识别全角空格）
        if (!isspace(ch)) {
            ungetc(ch, fp); // 非半角空白字符放回缓冲区，停止跳过
            break;
        }
    }
}

int count_paragraphs_in_file(FILE* fp) {
    int ch1, ch2, ch3;       // 存储连续3字节（UTF-8全角空格占3字节）
    int in_paragraph = 0;    // 0: 不在段落中, 1: 在段落中
    int paragraph_count = 0;
    int is_line_start = 1;   // 标记当前是否处于“行首”（初始为行首）

    // 先读取前3个字节（UTF-8全角空格需3字节判断，避免初始越界）
    if ((ch1 = fgetc(fp)) == EOF) return 0; // 文件空，返回0
    if ((ch2 = fgetc(fp)) == EOF) return 0;
    if ((ch3 = fgetc(fp)) == EOF) return 0;

    while (1) {
        // 核心逻辑：判断当前是否为“行首 + 连续两个UTF-8全角空格”
        if (is_line_start) {
            // 第一个全角空格判断（ch1-ch3）
            if (is_utf8_full_width_space(ch1, ch2, ch3)) {
                // 读取下3个字节，判断是否为第二个全角空格
                int next1 = fgetc(fp);
                int next2 = fgetc(fp);
                int next3 = fgetc(fp);
                if (next1 == EOF || next2 == EOF || next3 == EOF) break; // 文件结束

                // 第二个全角空格判断（next1-next3）
                if (is_utf8_full_width_space(next1, next2, next3)) {
                    // 符合“段首两个全角空格”→ 新段落计数
                    paragraph_count++;
                    in_paragraph = 1;
                    is_line_start = 0; // 已过行首，更新标记

                    // 更新ch1-ch3为下一组字节，准备后续循环
                    ch1 = fgetc(fp);
                    ch2 = fgetc(fp);
                    ch3 = fgetc(fp);
                    if (ch1 == EOF || ch2 == EOF || ch3 == EOF) break;
                    continue; // 跳过后续判断，进入下一轮循环
                }
                else {
                    // 不是第二个全角空格→将读取的字节放回缓冲区
                    ungetc(next3, fp);
                    ungetc(next2, fp);
                    ungetc(next1, fp);
                }
            }
        }

        // 遇到换行符→下一行开始，更新行首标记
        if (ch1 == '\n') {
            in_paragraph = 0;
            is_line_start = 1;
        }
        else if (is_line_start) {
            // 行首但不是两个全角空格→不是目标段落，取消行首标记
            is_line_start = 0;
        }

        // 移动字节窗口（每次向后读1字节，保持ch1-ch3为连续3字节）
        ch1 = ch2;
        ch2 = ch3;
        ch3 = fgetc(fp);
        if (ch3 == EOF) break; // 读取到文件末尾，退出循环
    }

    return paragraph_count;
}


    // 统计UTF-8文件中汉语句子数（仅识别：。！？）
    int count_sentence_in_file(FILE* fp) {
        if (fp == NULL || feof(fp) || ferror(fp)) { // 基础校验
            return 0;
        }

        int sentence_count = 0;
        int ch1, ch2, ch3;
        // 核心：仅保留表句子终结的3种标点的UTF-8编码
        const unsigned char end_punc[][3] = {
            {0xE3, 0x80, 0x82}, // 。（句号）
            {0xEF, 0xBC, 0x9F}, // ？（问号）
            {0xEF, 0xBC, 0x81}  // ！（感叹号）
        };
        const int punc_type = sizeof(end_punc) / sizeof(end_punc[0]);

        while (1) {
            // 1. 读UTF-8 3字节字符的首字节（汉语标点均为3字节，首字节范围0xE0~0xEF）
            ch1 = fgetc(fp);
            if (ch1 == EOF || ferror(fp)) { // 终止条件：文件结束或出错
                break;
            }
            // 非3字节UTF-8字符（如英文、空格）直接跳过
            if ((ch1 & 0xF0) != 0xE0) {
                continue;
            }

            // 2. 读后续2字节，确保字符完整性
            ch2 = fgetc(fp);
            ch3 = fgetc(fp);
            // 异常：字节不足（文件截断），放回已读字节
            if (ch2 == EOF || ch3 == EOF) {
                if (ch2 != EOF) ungetc(ch2, fp);
                ungetc(ch1, fp);
                break;
            }
            // 异常：无效UTF-8（第2/3字节需为10xxxxxx格式），放回字节
            if (((ch2 & 0xC0) != 0x80) || ((ch3 & 0xC0) != 0x80)) {
                ungetc(ch3, fp);
                ungetc(ch2, fp);
                ungetc(ch1, fp);
                continue;
            }

            // 3. 匹配句末标点，命中则计数
            for (int i = 0; i < punc_type; i++) {
                if (ch1 == end_punc[i][0] && ch2 == end_punc[i][1] && ch3 == end_punc[i][2]) {
                    sentence_count++;
                    break;
                }
            }
        }

        if (ferror(fp)) clearerr(fp); // 清理错误状态
        return sentence_count;
    }


//计算总字数
int count_total_in_file(FILE* fp) {
    if (fp == NULL) { // 增加空指针判断，避免崩溃
        return 0;
    }

    int total_count = 0;
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        unsigned char c = (unsigned char)ch;

        // 1. 处理ASCII字符（1字节）：空白字符或其他ASCII（0x00-0x7F）
        if (c <= 0x7F) {
            // 单独处理回车符（避免与换行符重复统计，保持原逻辑）
            if (c == 0x0D) { // 回车符 \r
                int next = fgetc(fp);
                if (next != 0x0A && next != EOF) { // 若后续不是换行符 \n，放回
                    ungetc(next, fp);
                }
            }
            total_count++; // ASCII字符（含空白、英文等）计1次
        }
        // 2. 处理UTF-8多字节字符（2/3/4字节，此处重点适配汉语3字节）
        else {
            // UTF-8多字节字符首字节特征：
            // - 2字节：110xxxxx（0xC0-0xDF），需再读1字节
            // - 3字节：1110xxxxx（0xE0-0xEF），需再读2字节（汉语字符/标点均为此类）
            // - 4字节：11110xxx（0xF0-0xF7），需再读3字节（极少用，简化处理）

            int bytes_to_skip = 0;
            if (c >= 0xC0 && c <= 0xDF) {       // 2字节UTF-8字符
                bytes_to_skip = 1;
            }
            else if (c >= 0xE0 && c <= 0xEF) { // 3字节UTF-8字符（汉语核心）
                bytes_to_skip = 2;
            }
            else if (c >= 0xF0 && c <= 0xF7) { // 4字节UTF-8字符
                bytes_to_skip = 3;
            }

            // 跳过后续字节（确保读取完整字符，避免拆分）
            for (int i = 0; i < bytes_to_skip; i++) {
                if (fgetc(fp) == EOF) { // 若文件末尾不完整，直接退出（避免死循环）
                    break;
                }
            }
            total_count++; // 1个多字节字符（如汉语“你”“。”）计1次
        }
    }
    return total_count;
}


//统计每个字出现的次数并排序
// 存储汉字/汉语标点（GB2312编码占2字节）及计数

CharStat statList[MAX_CHAR] = { 0 };  // 统计列表（初始化为0）
int statLen = 0;                     // 已统计的字符种类数


// 2. 判断3字节是否为UTF-8汉语字符/标点（核心：匹配UTF-8编码规则）
// UTF-8汉语字符特征：首字节0xE0-0xEF，后续两字节0x80-0xBF（覆盖绝大多数简体/繁体汉语字符）
int isCnChar(unsigned char c1, unsigned char c2, unsigned char c3) {
    return (c1 >= 0xE0 && c1 <= 0xEF)   // 首字节：1110xxxx
        && (c2 >= 0x80 && c2 <= 0xBF)   // 次字节：10xxxxxx
        && (c3 >= 0x80 && c3 <= 0xBF);  // 末字节：10xxxxxx
}


// 3. 统计UTF-8字符：存在则计数+1，不存在则新增
void countCnChar(unsigned char c1, unsigned char c2, unsigned char c3) {
    // 先检查字符是否已存在于统计列表
    for (int i = 0; i < statLen; i++) {
        if (statList[i].ch[0] == c1
            && statList[i].ch[1] == c2
            && statList[i].ch[2] == c3) {
            statList[i].count++;  // 已存在，次数+1
            return;
        }
    }
    // 列表未满，新增字符
    if (statLen < MAX_CHAR) {
        statList[statLen].ch[0] = c1;
        statList[statLen].ch[1] = c2;
        statList[statLen].ch[2] = c3;
        statList[statLen].count = 1;
        statLen++;
    }
}


// 4. 从UTF-8编码文件读取并统计（二进制模式，避免编码转换）
int statFromFile(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("文件打开失败");  // 打印具体错误（如文件不存在）
        return 0;  // 打开失败返回0
    }

    unsigned char buf[3];  // 存储1个UTF-8汉语字符（3字节）
    int readNum;

    // 循环读取：每次读3字节（UTF-8汉语字符单位）
    while ((readNum = fread(buf, 1, 3, fp)) == 3) {
        // 仅统计UTF-8汉语字符/标点
        if (isCnChar(buf[0], buf[1], buf[2])) {
            countCnChar(buf[0], buf[1], buf[2]);
        }
    }

    // 处理文件末尾可能残留的1-2字节（非完整UTF-8字符，直接忽略）
    fclose(fp);
    return 1;  // 统计成功返回1
}


// 5. 排序比较函数：按“次数降序+同次数字典序升序”
// UTF-8字典序：按3字节编码值依次比较（首字节→次字节→末字节）
int compareChar(const void* a, const void* b) {
    CharStat* statA = (CharStat*)a;
    CharStat* statB = (CharStat*)b;

    // 第一步：按出现次数降序（次数大的在前）
    if (statA->count != statB->count) {
        return statB->count - statA->count;
    }
    // 第二步：次数相同，按UTF-8编码字典序升序
    else {
        // 先比首字节
        if (statA->ch[0] != statB->ch[0]) {
            return statA->ch[0] - statB->ch[0];
        }
        // 首字节相同，比次字节
        else if (statA->ch[1] != statB->ch[1]) {
            return statA->ch[1] - statB->ch[1];
        }
        // 次字节相同，比末字节
        else {
            return statA->ch[2] - statB->ch[2];
        }
    }
}


//  打印排序后的统计结果（需确保终端支持UTF-8编码）
void printSortedResult() {
    // 调用qsort排序：数组、元素数、元素大小、比较函数
    qsort(statList, statLen, sizeof(CharStat), compareChar);

    printf("\n【UTF-8汉语字符统计结果（按次数降序）】\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < statLen; i++) {
        // 直接打印3字节UTF-8字符（终端需设为UTF-8编码才正常显示）
        printf("%c%c%c：%d次\n",
            statList[i].ch[0],
            statList[i].ch[1],
            statList[i].ch[2],
            statList[i].count);
    }
    if (statLen == 0) {
        printf("未检测到有效UTF-8汉语字符（请确认文件编码为UTF-8）\n");
    }
}
int save_all_results(const char* input_filename, const char* output_filename, int para_cnt, int sent_cnt, int total_cnt) {
    // 1. 打开输出文件（"w"模式：不存在则创建，存在则覆盖）
    FILE* res_fp = fopen(output_filename, "w");
    if (res_fp == NULL) {
        perror("无法创建/打开结果文件");
        return 1;
    }

    // 2. 写入基础统计结果（段落、句子、总字数）
    fprintf(res_fp, "==================== 文本统计总结果 ====================\n");
    fprintf(res_fp, "待统计文件路径：%s\n", input_filename);
    fprintf(res_fp, "--------------------------------------------------------\n");
    fprintf(res_fp, "1. 段落总数（以“两个全角空格”开头的段落）：%d\n", para_cnt);
    fprintf(res_fp, "2. 句子总数（以。！？结尾的句子）：%d\n", sent_cnt);
    fprintf(res_fp, "3. 总字数（含ASCII字符、汉语字符、标点）：%d\n", total_cnt);
    fprintf(res_fp, "--------------------------------------------------------\n\n");

    // 3. 写入字符频次统计结果（先排序，再写入）
    qsort(statList, statLen, sizeof(CharStat), compareChar); // 确保按频次排序
    fprintf(res_fp, "【UTF-8汉语字符频次统计（按出现次数降序）】\n");
    fprintf(res_fp, "--------------------------------------------------------\n");
    fprintf(res_fp, "字符\t出现次数\n");
    fprintf(res_fp, "--------------------------------------------------------\n");
    if (statLen == 0) {
        fprintf(res_fp, "未检测到有效UTF-8汉语字符（请确认文件编码为UTF-8）\n");
    }
    else {
        for (int i = 0; i < statLen; i++) {
            // 直接写入3字节UTF-8字符（结果文件编码为UTF-8，打开时需选择UTF-8解码）
            fprintf(res_fp, "%c%c%c\t%d\n",
                statList[i].ch[0],
                statList[i].ch[1],
                statList[i].ch[2],
                statList[i].count);
        }
    }
    fprintf(res_fp, "========================================================\n");

    // 4. 关闭文件（必须执行，确保数据写入磁盘）
    if (fclose(res_fp) != 0) {
        perror("关闭结果文件时发生错误");
        return 2;
    }

    printf("\n 所有统计结果已保存到：%s\n", output_filename);
    return 0;
}

