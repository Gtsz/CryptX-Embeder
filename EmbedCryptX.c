// CryptX 是一種基於 Logistic 迭代的混沌對稱加密算法，C 語言實作
// 特性：
// 加密偶次即解密
// 多次加密毋須順序解密
// 作者： Gtsz
// License: GPL v3.0

#include <stdio.h>
#define PROGLEN 4096

typedef unsigned char byte;

void codec(char*, char*, char*, int);
long int str_size(char*);
void err_message();

//const int PROGLEN = 4096;
const int MAXKEYLEN = 1024;
const char ENTER = 13;

int main(int argc, char *argv[])
{
	int i, j, filename_length;
	char c;
	char key[MAXKEYLEN];
	printf("Key (It will proceed anyway): ");
	for (i = 0; (c = getch()) != ENTER; i++){
		key[i] = c;
		printf("*");
	}
	
	filename_length = str_size(argv[0]) - 4;
	char output_filename[filename_length];
	for (j = 0; j < filename_length; j++){
		output_filename[j] = argv[0][j];
	}
	output_filename[filename_length] = '\0';
	
	codec(key, argv[0], output_filename, i);
    return 0;
}

void codec(char* key, char* infile_name, char* outfile_name, int key_length)
{
	FILE *outfile, *infile;
	int t_calc, count, i, j;
	float key_temp;
	long int file_length;
	const long int BUFLEN = 8192; // 緩衝區大小， 必須爲 4 的倍數（浮點數長度）
	int float_length = sizeof(float); // 預先取得 float 佔用位元組，避免之後頻繁呼叫 sizeof 函數
	
	printf("\nbuffer size: %d Byte(s)\n", BUFLEN);
	
	if (!(infile = fopen(infile_name, "rb")) || fseek(infile, 0, SEEK_END)){ // 開啓檔案，然後將指針移至文件末，同時作例外處理
		err_message();
	}
	
	long int seed_length = key_length + 1;
	float seed[seed_length];
	float key_mtx[seed_length][BUFLEN / float_length + 1]; // 緩衝區內原始密鑰陣列，大小爲緩衝區長度與密碼字元數之乘積
	float key_temp_arr[seed_length];
	byte buffer[BUFLEN];
	outfile = fopen(outfile_name, "wb");
	file_length = ftell(infile) - PROGLEN;
	fseek(infile, PROGLEN, SEEK_SET); // 跳過程式自身

	printf("file size: %d Byte(s)\n", file_length);
	printf("key length: %d\n", seed_length - 1);
	printf("Processing started...\n");
	
	// 納入密鑰源
	seed[0] = 0.2f + (float)(file_length%919)/1000000; // 檔案長度作一因子，取一質數以 mod，避免過大，降冪
	for (i = 1; *key; i++){
		seed[i] = 0.2f + (float)(*key++)/1000000; // 密碼字元各作因子，降冪
	}
	
	// 初始化密鑰陣列
	t_calc = 23; // 跳過開始的可預測部分
	for (i = 0; i < seed_length; i++){
		key_temp_arr[i] = seed[i];
		t_calc++; // 移位，密碼必須順序正確
        for (j = 0; j < t_calc; j++){
			key_temp = key_temp_arr[i];
            key_temp_arr[i] = 4.0f * key_temp * (1.0f - key_temp); // 預計算密鑰
        }
    }
	
	// 讀取檔案並加密／解密
	while ((count = fread(buffer, 1, BUFLEN, infile)) != 0){
		t_calc = count / float_length + 1;
		for (i = 0; i < seed_length; i++){
			key_mtx[i][0] = key_temp_arr[i];
			for (j = 1; j < t_calc; j++){
				key_mtx[i][j] = 4.0f * key_mtx[i][j-1] * (1.0f - key_mtx[i][j-1]); // 計算密鑰
			}
			for (j = 0; j < count; j++){
				buffer[j] ^= *((byte*)&key_mtx[i][0] + j); // 將一個密鑰浮點數轉換爲四個位元組與檔案內容作異或運算
			}
			key_temp_arr[i] = key_mtx[i][t_calc-1];
		}
        fwrite(buffer, 1, count, outfile);
    }
	
    fclose(infile);
    fclose(outfile);
	printf("Process finished\n");
}

void err_message()
{
	printf("Process error\n");
	exit(1);
}

long int str_size(char* str){
	long int count;
	for (count = 0; *str; count++){
		*str++;
	}
	return count;
}
