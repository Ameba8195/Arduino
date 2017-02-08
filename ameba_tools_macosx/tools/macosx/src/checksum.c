// checksum.cpp : Defines the entry point for the console application.

/*

Compile command:
	gcc -o tools/macosx/checksum tools/macosx/src/checksum.c

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	unsigned int checksum = 0;
	//unsigned char buf[502000]; //MAX FW Size = 512K-44K/2 = 501760
	unsigned char *buf;
	int i;
	int size = 0;

	if(argc != 2)	return -1;

	fp = fopen(argv[1], "rb+");
	if(!fp)	return -2;


	fseek(fp,0L,SEEK_END);
	size = ftell(fp);
	fseek(fp,0L,SEEK_SET);
	
	buf = malloc(size+100);
	if(!buf){
		fclose(fp);
		return -3;
	}
	
	printf("size = %d \n\r", size);

	memset(buf, 0, size+100);

	fread(buf, 1, size, fp);

	for(i=0;i<size;i++){
		checksum += buf[i];
	}
	
	fseek(fp,0L,SEEK_END);
	
	fwrite(&checksum, sizeof(int), 1, fp);

	printf("checksum %x\n\r", checksum);

	free(buf);
	fclose(fp);
}
