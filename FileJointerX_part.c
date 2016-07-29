// CryptX 是一種基於 Logistic 迭代的混沌對稱加密算法，C 語言實作
// 特性：
// 加密偶次即解密
// 多次加密毋須順序解密
// 作者： Gtsz
// License: GPL v3.0

#include <stdio.h>
#define PROGLEN 4096

typedef unsigned char byte;

void file_joint(char*, char*, char*);
void encrypt_codec(char*, char*, char*);
long int str_size(const char*);
void usage();

char tempfile_name[] = {"temp"};
const int MAXSRTLEN = 1024;

int main(int argc, char *argv[])
{
	int i, filename_length;
	
    if (argc == 3){
		printf("Encrypt the file and embed the decrypter\n");
		printf("version: alpha-1.0\n");
		printf("by Gtsz(email:chuyt@live.hk)\n");
		filename_length = str_size(argv[1]) + 4;
		char output_filename[filename_length];
		
		for (i = 0; i < filename_length; i++){
			output_filename[i] = argv[1][i];
		}
		output_filename[filename_length - 4] = '.';
		output_filename[filename_length - 3] = 'e';
		output_filename[filename_length - 2] = 'x';
		output_filename[filename_length - 1] = 'e';
		output_filename[filename_length] = '\0';
			
		printf("Input file: %s\n", argv[1]);
		printf("Output file: %s\n", output_filename);
		
		encrypt_codec(argv[2], argv[1], tempfile_name);
		file_joint(argv[0], tempfile_name, output_filename);
		
		remove(tempfile_name);
	} else {
		usage();
	}
    return 0;
}

void file_joint(char* infile_name1, char* infile_name2, char* outfile_name)
{
	FILE *outfile, *infile1, *infile2;
	int count, i, j;
	const long int BUFLEN = 8192;
	byte buffer[BUFLEN];
	
	outfile = fopen(outfile_name, "wb");
	
	if (!(infile1 = fopen(infile_name1, "rb"))){
		exit(0);
	}
	
	if (!(infile2 = fopen(infile_name2, "rb"))){
		exit(0);
	}
	
	fseek(infile1, PROGLEN, SEEK_SET); // 跳過程式自身

	while ((count = fread(buffer, 1, BUFLEN, infile1)) != 0){
        fwrite(buffer, 1, count, outfile);
    }
	
	while ((count = fread(buffer, 1, BUFLEN, infile2)) != 0){
        fwrite(buffer, 1, count, outfile);
    }
	
    fclose(infile1);
	fclose(infile2);
    fclose(outfile);
	printf("Process finished\n");
}

void encrypt_codec(char* key, char* infile_name, char* outfile_name)
{
	FILE *outfile, *infile;
	int t_calc, count, i, j;
	float key_temp;
	long int file_length;
	const long int BUFLEN = 8192; // 緩衝區大小， 必須爲 4 的倍數（浮點數長度）
	int float_length = sizeof(float); // 預先取得 float 佔用位元組，避免之後頻繁呼叫 sizeof 函數
	
	printf("buffer size: %d Byte(s)\n", BUFLEN);
	
	if (!(infile = fopen(infile_name, "rb")) || fseek(infile, 0, SEEK_END)){ // 開啓檔案，然後將指針移至文件末，同時作例外處理
		usage();
	}
	
	long int seed_length = str_size(key) + 1;
	float seed[seed_length];
	float key_mtx[seed_length][BUFLEN / float_length + 1]; // 緩衝區內原始密鑰陣列，大小爲緩衝區長度與密碼字元數之乘積
	float key_temp_arr[seed_length];
	byte buffer[BUFLEN];
	outfile = fopen(outfile_name, "wb");
	file_length = ftell(infile);
	rewind(infile);

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
}

long int str_size(const char* str){
	long int count;
	for (count = 0; *str; count++){
		*str++;
	}
	return count;
}

void usage()
{
	printf("ERROR!\n");
    printf("Usage: CryptXEmbeder <input_file> <key>\n");
	exit(1);
}
