#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <windows.h>
#include <winnls.h>
#include <unistd.h>

#include "../inc/DocCount.h"



CharStat statList[MAX_CHAR] = { 0 };
int statLen = 0;


// 读取并显示文件内容
int read_chinese_txt(const char* filename) {
	FILE* file = fopen(filename, "rb");
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
	fclose(file);
	return 0;
}

// 计算段落数
int is_utf8_full_width_space(unsigned char b1, unsigned char b2, unsigned char b3) {
	return (b1 == 0xE3 && b2 == 0x80 && b3 == 0x80) ? 1 : 0;
}

void skip_line_half_width_space(FILE* fp) {
	int ch;
	while ((ch = fgetc(fp)) != EOF && ch != '\n') {
		if (!isspace(ch)) {
			ungetc(ch, fp);
			break;
		}
	}
}
int count_paragraphs_in_file(FILE* fp) {
	int ch1, ch2, ch3;
	int in_paragraph = 0;
	int paragraph_count = 0;
	int is_line_start = 1;
	if ((ch1 = fgetc(fp)) == EOF) return 0;
	if ((ch2 = fgetc(fp)) == EOF) return 0;
	if ((ch3 = fgetc(fp)) == EOF) return 0;
	while (1) {
		if (is_line_start) {
			if (is_utf8_full_width_space(ch1, ch2, ch3)) {
				int next1 = fgetc(fp);
				int next2 = fgetc(fp);
				int next3 = fgetc(fp);
				if (next1 == EOF || next2 == EOF || next3 == EOF) break;
				if (is_utf8_full_width_space(next1, next2, next3)) {
					paragraph_count++;
					in_paragraph = 1;
					is_line_start = 0;
					ch1 = fgetc(fp);
					ch2 = fgetc(fp);
					ch3 = fgetc(fp);
					if (ch1 == EOF || ch2 == EOF || ch3 == EOF) break;
					continue;
				} else {
					ungetc(next3, fp);
					ungetc(next2, fp);
					ungetc(next1, fp);
				}
			}
		}
		if (ch1 == '\n') {
			in_paragraph = 0;
			is_line_start = 1;
		} else if (is_line_start) {
			is_line_start = 0;
		}
		ch1 = ch2;
		ch2 = ch3;
		ch3 = fgetc(fp);
		if (ch3 == EOF) break;
	}
	return paragraph_count;
}

// 统计汉语句子数
int count_sentence_in_file(FILE* fp) {
	if (fp == NULL || feof(fp) || ferror(fp)) return 0;
	int sentence_count = 0;
	int ch1, ch2, ch3;
	const unsigned char end_punc[][3] = {
		{0xE3, 0x80, 0x82}, // 。
		{0xEF, 0xBC, 0x9F}, // ？
		{0xEF, 0xBC, 0x81}  // ！
	};
	const int punc_type = sizeof(end_punc) / sizeof(end_punc[0]);
	while (1) {
		ch1 = fgetc(fp);
		if (ch1 == EOF || ferror(fp)) break;
		if ((ch1 & 0xF0) != 0xE0) continue;
		ch2 = fgetc(fp);
		ch3 = fgetc(fp);
		if (ch2 == EOF || ch3 == EOF) {
			if (ch2 != EOF) ungetc(ch2, fp);
			ungetc(ch1, fp);
			break;
		}
		if (((ch2 & 0xC0) != 0x80) || ((ch3 & 0xC0) != 0x80)) {
			ungetc(ch3, fp);
			ungetc(ch2, fp);
			ungetc(ch1, fp);
			continue;
		}
		for (int i = 0; i < punc_type; i++) {
			if (ch1 == end_punc[i][0] && ch2 == end_punc[i][1] && ch3 == end_punc[i][2]) {
				sentence_count++;
				break;
			}
		}
	}
	if (ferror(fp)) clearerr(fp);
	return sentence_count;
}

// 计算总字数
int count_total_in_file(FILE* fp) {
	if (fp == NULL) return 0;
	int total_count = 0;
	int ch;
	while ((ch = fgetc(fp)) != EOF) {
		unsigned char c = (unsigned char)ch;
		if (c <= 0x7F) {
			if (c == 0x0D) {
				int next = fgetc(fp);
				if (next != 0x0A && next != EOF) ungetc(next, fp);
			}
			total_count++;
		} else {
			int bytes_to_skip = 0;
			if (c >= 0xC0 && c <= 0xDF) bytes_to_skip = 1;
			else if (c >= 0xE0 && c <= 0xEF) bytes_to_skip = 2;
			else if (c >= 0xF0 && c <= 0xF7) bytes_to_skip = 3;
			for (int i = 0; i < bytes_to_skip; i++) {
				if (fgetc(fp) == EOF) break;
			}
			total_count++;
		}
	}
	return total_count;
}





// UTF-8转宽字符
wchar_t utf8_to_wchar( char c1,  char c2,  char c3) {
	char utf8_str[4] = { c1, c2, c3, '\0' };
	wchar_t wstr[2] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, utf8_str, 3, wstr, 1);
	return wstr[0];
}

// 判断是否为UTF-8汉语字符
int isCnChar(unsigned char c1, unsigned char c2, unsigned char c3) {
	// 中文字符 U+4E00 - U+9FFF 的 UTF-8 编码范围
	return (c1 >= 0xE0 && c1 <= 0xEF)
		&& (c2 >= 0x80 && c2 <= 0xBF)
		&& (c3 >= 0x80 && c3 <= 0xBF);
}

// 统计字符出现次数
void countCnChar(unsigned char c1,  unsigned char c2,  unsigned char c3) {
	for (int i = 0; i < statLen; i++) {
		if (statList[i].ch[0] == c1 && statList[i].ch[1] == c2 && statList[i].ch[2] == c3) {
			statList[i].count++;
			return;
		}
	}
	if (statLen < MAX_CHAR) {
		statList[statLen].ch[0] = c1;
		statList[statLen].ch[1] = c2;
		statList[statLen].ch[2] = c3;
		statList[statLen].count = 1;
		statList[statLen].wch = utf8_to_wchar(c1, c2, c3);
		statLen++;
	}
}

void remove_all_newlines(FILE* fp) {
	if (!fp) return;

	// 读取整个文件内容
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	if (size <= 0) return;

	char* buffer = (char*)malloc(size);
	if (!buffer) return;

	size_t read_size = fread(buffer, 1, size, fp);
	rewind(fp); // 准备重写

	// 写回时跳过所有换行符
	long write_pos = 0;
	for (size_t i = 0; i < read_size; ++i) {
		if (buffer[i] != '\n') {
			buffer[write_pos++] = buffer[i];
		}
	}

	// 重写到文件
	fwrite(buffer, 1, write_pos, fp);
	fflush(fp);
	ftruncate(fileno(fp), write_pos); // 截断多余部分
	rewind(fp); // 重置文件指针

	free(buffer);
}

size_t fread_skip_newline(void* buf, size_t size, size_t count, FILE* fp) {
	unsigned char* p = (unsigned char*)buf;
	size_t total = 0;

	while (total < size * count) {
		int c = fgetc(fp);
		if (c == EOF) break;
		if (c == '\n' || c == '\r') continue; // 跳过换行符
		p[total++] = (unsigned char)c;
	}

	return total / size; // 模拟 fread 的返回值
}


// 从文件读取并统计字符
int statFromFile(const char* filename,const char* outputname) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("文件打开失败");
		return 0;
	}
	// remove_all_newlines(fp);


		unsigned char buf[3];
		int readNum;
		// while ((readNum = fread(buf, 1, 3, fp)) == 3)
			while ((readNum = fread_skip_newline(buf, 1, 3, fp)) == 3){
			printf("%c%c%c", buf[0],buf[1],buf[2]);

			if (isCnChar(buf[0], buf[1], buf[2])) {
				// printf( "[DEBUG] this is a char formated in utf-8" );
				countCnChar(buf[0], buf[1], buf[2]);
			}

			else {
				// printf("[DEBUG] this is not formated by utf-8\n");
			}
		}

		fclose(fp);
		return 1;
	}


// 核心修改：先按出现次数降序，次数相同按中文字典顺序升序
int compareChar(const void* a, const void* b) {
	CharStat* statA = (CharStat*)a;
	CharStat* statB = (CharStat*)b;

	// 第一步：按出现次数降序（次数多的排在前面）
	if (statA->count != statB->count) {
		return statB->count - statA->count;
	}

	// 第二步：次数相同时，按中文字典顺序升序
	int dict_cmp = CompareStringW(0x0804, LINGUISTIC_IGNORECASE,
	                              &statA->wch, 1, &statB->wch, 1);
	return dict_cmp - 2;  // 转换为qsort需要的正负值（升序）
}

// 打印排序后的结果（更新提示信息）
void printSortedResult() {
	printf("[DEBUG] statlen:%d ",statLen);
	qsort(statList, statLen, sizeof(CharStat), compareChar);
	printf("\n【汉语字符统计结果（按出现次数降序，次数相同按中文字典顺序升序）】\n");
	printf("----------------------------------------\n");
	for (int i = 0; i < statLen; i++) {
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

int save_all_results(const char* input_filename, const char* output_filename, int para_cnt, int sent_cnt, int total_cnt)
{
	FILE* res_fp = fopen(output_filename, "w");
	if (res_fp == NULL) {
		perror("Failed to create/open result file");
		return 1;
	}
	fprintf(res_fp, "==================== Text Statistics Results ====================\n");
	fprintf(res_fp, "Path of the file to be counted: %s\n", input_filename);
	fprintf(res_fp, "--------------------------------------------------------\n");
	fprintf(res_fp, "1. Total number of paragraphs (paragraphs starting with \"two full-width spaces\"): %d\n", para_cnt);
	fprintf(res_fp, "2. Total number of sentences (sentences ending with 。！？): %d\n", sent_cnt);
	fprintf(res_fp, "3. Total number of characters (including ASCII characters, Chinese characters, punctuation): %d\n", total_cnt);
	fprintf(res_fp, "--------------------------------------------------------\n\n");
	qsort(statList, statLen, sizeof(CharStat), compareChar);
	fprintf(res_fp, "【Chinese Character Frequency Statistics (sorted by frequency descending, same frequency sorted by Chinese dictionary order ascending)】\n");
	fprintf(res_fp, "--------------------------------------------------------\n");
	fprintf(res_fp, "Character\tFrequency\n");
	fprintf(res_fp, "--------------------------------------------------------\n");
	if (statLen == 0) {
		fprintf(res_fp, "No valid UTF-8 Chinese characters detected (please confirm the file encoding is UTF-8)\n");
	} else {
		for (int i = 0; i < statLen; i++) {
			fprintf(res_fp, "%c%c%c\t%d\n",
			        statList[i].ch[0],
			        statList[i].ch[1],
			        statList[i].ch[2],
			        statList[i].count);
		}
	}
	fprintf(res_fp, "========================================================\n");
	fclose(res_fp);
	printf("\n All statistical results have been saved to: %s\n", output_filename);
	return 0;
}
