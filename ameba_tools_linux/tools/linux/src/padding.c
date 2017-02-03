/*

Compile command:
	gcc -m32 -o tools/linux/padding tools/linux/src/padding.c -static

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main(int argc, char *argv[])
{
	FILE* fp;

	// argv[1] -> length
	// argv[2] -> data
	// argv[3] -> output file name
	char	*unit;
	int 	length = strtod(argv[1], &unit);
	char	data = strtod(argv[2], NULL);
	char	*buf;

	printf("total %d %s, padding data %x, name %s\n", length, unit, data&0xFF, argv[3]);
	if(unit[0]==0)	length*=1;
	else if(unit[0]=='K'||unit[0]=='k')	length*=1024;
	else if(unit[0]=='M'||unit[0]=='m')	length*=(1024*1024);
	else if(unit[0]=='G'||unit[0]=='g')	length*=(1024*1024);
	else {
		printf("unit %s is Not valid\n", unit);
		return;
	}
	fp = fopen(argv[3], "r+b");
	if(!fp)	return;

	buf = malloc(length);
	memset(buf, data, length);
	printf("Original size %zd\n", fread(buf, 1, length, fp));
	fseek(fp, 0, SEEK_SET);
	printf("Padding  size %zd\n", fwrite(buf, 1, length, fp));
	
	free(buf);
	fclose(fp);
}
