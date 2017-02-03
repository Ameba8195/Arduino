/*

Compile command:
	gcc -m32 -o amebasize tools/linux/src/amebasize.c -static

*/

#include <stdio.h>

int main(int argc, char *argv[]) {

	int size;
	char filename[512];

	if (argc < 2) {
		return -1;
	}

	sprintf(filename, "%s\\ram_1.p.bin", argv[1]);
	FILE* fp_bootloader = fopen(filename, "rb");
	if (fp_bootloader != NULL) {
		fseek(fp_bootloader, 0L, SEEK_END);
		size = ftell(fp_bootloader);
		fclose(fp_bootloader);
		printf("bootloader %d\n", size);
	}

	sprintf(filename, "%s\\ram_2.p.bin", argv[1]);
	FILE* fp_img2 = fopen(filename, "rb");
	if (fp_img2 != NULL) {
		fseek(fp_img2, 0L, SEEK_END);
		size = ftell(fp_img2);
		fclose(fp_img2);
		printf("image2 %d\n", size);
	}

	sprintf(filename, "%s\\ram_3.p.bin", argv[1]);
	FILE* fp_img3 = fopen(filename, "rb");
	if (fp_img3 != NULL) {
		fseek(fp_img3, 0L, SEEK_END);
		size = ftell(fp_img3);
		fclose(fp_img3);
		printf("image3 %d\n", size);
	}

	return 0;
}
