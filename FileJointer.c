#include <stdio.h>

typedef unsigned char byte;

void codec(char*, char*, char*);

int main(int argc, char *argv[])
{
	codec(argv[1], argv[2], argv[3]);
    return 0;
}

void codec(char* infile_name1, char* infile_name2, char* outfile_name)
{
	FILE *outfile, *infile1, *infile2;
	int count;
	const long int BUFLEN = 8192;
	byte buffer[BUFLEN];
	
	outfile = fopen(outfile_name, "wb");
	
	if (!(infile1 = fopen(infile_name1, "rb"))){
		exit(0);
	}
	
	if (!(infile2 = fopen(infile_name2, "rb"))){
		exit(0);
	}

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
