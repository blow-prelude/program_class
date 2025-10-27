#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
// 读取并显示文件内容
int read_chinese_txt(const char* filename) {
    FILE* file = fopen(filename, "r");  // 使用参数传入的文件名
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
// 计算段落数（按空行分隔）
int count_paragraphs_in_file(FILE* fp) {
    int ch;
    int in_paragraph = 0;  // 0: 不在段落中, 1: 在段落中
    int paragraph_count = 0;

    while ((ch = fgetc(fp)) != EOF) {
        if (isspace(ch)) {
            if (ch == '\n') {
                // 换行且之前在段落中 → 段落结束
                if (in_paragraph) {
                    in_paragraph = 0;
                }
            }
        }
        else {
            // 非空白字符且不在段落中 → 新段落开始
            if (!in_paragraph) {
                in_paragraph = 1;
                paragraph_count++;
            }
        }
    }
    return paragraph_count;
}
//计算句子总数（按句号分隔）
#include <stdio.h>

// 统计UTF-8编码文件中的汉语句子数（仅识别“。”为句子结束符，可扩展其他标点）
int count_sentence_in_file(FILE* fp) {
    if (fp == NULL) { // 增加空指针判断，避免崩溃
        return 0;
    }

    int sentence_count = 0;
    int ch1, ch2, ch3; // 存储UTF-8标点的3个字节

    // 循环读取UTF-8字符：每次先读第1字节，根据特征判断是否为3字节标点
    while ((ch1 = fgetc(fp)) != EOF) {
        // UTF-8汉语句号“。”的编码为 0xE3 0x80 0x82，先匹配第1字节
        if (ch1 == 0xE3) {
            // 读取后续2个字节（UTF-8多字节字符需连续读取）
            ch2 = fgetc(fp);
            ch3 = fgetc(fp);

            // 情况1：3字节完整且匹配“。”，句子数+1
            if (ch2 == 0x80 && ch3 == 0x82) {
                sentence_count++;
            }
            // 情况2：读取到EOF（文件末尾不完整），无需处理
            else if (ch2 == EOF || ch3 == EOF) {
                break;
            }
            // 情况3：不匹配“。”，将读取的2字节放回缓冲区（避免漏读后续字符）
            else {
                ungetc(ch3, fp);
                ungetc(ch2, fp);
            }
        }
        // 非UTF-8标点的第1字节，直接跳过（不处理，继续读下一个字符）
        else {
            continue;
        }
    }
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
// 1. 结构体：存储UTF-8字符（3字节）及出现次数
typedef struct {
    unsigned char ch[3];  // UTF-8汉语字符/标点占3字节
    int count;            // 出现次数
} CharStat;

#define MAX_CHAR 10000    // 最大支持10000个不同字符
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
int main() {
    system("chcp 65001");
    const char* filename = "D:\\programs\\cpp\\program_class\\data\\res\\test01_process.txt";
    // 打开文件用于统计
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("无法打开文件（统计段落）");
        return 1;
    }
    int a = count_paragraphs_in_file(fp);
    rewind(fp);
    int b = count_sentence_in_file(fp);
    rewind(fp);
    int c = count_total_in_file(fp);
    rewind(fp);
    int read_result = read_chinese_txt(filename);
    rewind(fp);

    int statRet = statFromFile(filename);
    fclose(fp);  // 统计完成后关闭文件
    if (read_result != 0) {
        return read_result;
    }
    printf("段落总数: %d\n", a);
    printf("句子总数：%d\n", b);
    printf("总字数：%d\n", c);
    printSortedResult();
    return 0;
}
