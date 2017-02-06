/*

Compile command:
	mingw32-gcc.exe -o tools\windows\pick.exe tools\windows\src\pick.c -static

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>

#define PATTERN_1 0x96969999
#define PATTERN_2 0xFC66CC3F
#define PATTERN_3 0x03CC33C0
#define PATTERN_4 0x6231DCE5

unsigned int fw_head[4] = {PATTERN_1, PATTERN_2, PATTERN_3, PATTERN_4};
unsigned int seg_head[4] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};

int main(int argc, char* argv[])
{
	int arg_num = 6;

	if(strstr(argv[5],"head"))	arg_num++;

	if(argc!=arg_num){
			printf("Usage: pick.exe <start addr> <end addr> <input name> <output name> <body[+offset] , head [image2_start]>\n");
			return -1;
	}

	unsigned char *buf;

	unsigned int start;//=atoi(argv[1]);
	unsigned int end;// = atoi(argv[2]);
	unsigned int base;

	int is_raw = 0;
	int is_sig = 0;

	char *inf = argv[3];
	char *outf = argv[4];

	int size;
	FILE *ifp, *ofp;
	
	//if(argv[1][0]=='0'&&(argv[1][1]=='x'||argv[1][1]=='X'))
	//	sscanf(argv[1], "0x%x", &start);
	//else
	//	start=atoi(argv[1]);
	start = strtol(argv[1], NULL, 0);

	//if(argv[2][0]=='0'&&(argv[2][1]=='x'||argv[2][1]=='X'))
	//	sscanf(argv[2], "0x%x", &end);
	//else
	//	end=atoi(argv[2]);
	end = strtol(argv[2], NULL, 0);

	base = start&0xFFFF0000;

	if(strstr(argv[5],"reset_offset")){
		base = start;
	}

	if(strstr(argv[5],"raw")){
		is_raw = 1;
	}else
		is_raw = 0;

	if(strstr(argv[5],"sig")){
		is_sig = 1;
	}else
		is_sig = 0;

	printf("b:%d s:%d e:%d\n", base, start, end);
	//printf("%s %s\n", inf, outf);	

	ifp = fopen(inf, "rb");
	if(!ifp)	return -2;
	ofp = fopen(outf, "wb");
	if(!ofp)	return -3;

	fseek(ifp, 0, SEEK_END);
	size = ftell(ifp);
	
	printf("size %d\n", size);
	buf = malloc(size);
	if(!buf)	return -4;

	if(end==0)	end = base+size;

	if( end-start+1 > 0){	
		fseek(ifp, start-base, SEEK_SET);
		fread(buf, end-start, 1, ifp);
		if(is_raw==0){
			if(strcmp(argv[5],"head")==0){
				int offset = strtol(argv[6], NULL, 0);
				printf("append fw head %x\n", offset);
				fwrite(fw_head, 4, sizeof(unsigned int), ofp);
				seg_head[2] = (0xFFFF0000|(offset/1024));
			}else{
				if(is_sig){
					seg_head[2] = 0x35393138;
					seg_head[3] = 0x31313738;
				}else{
					seg_head[2] = 0xFFFFFFFF;
					seg_head[3] = 0xFFFFFFFF;
				}
			}
			seg_head[0] = end-start;
			seg_head[1] = start;
			fwrite(seg_head, 4, sizeof(unsigned int), ofp);
		}
		fwrite(buf, end-start, 1, ofp);
			
	}	
	printf("copy size %d\n", end-start);
	fclose(ifp);
	fclose(ofp);
	free(buf);

	return 0;
}
