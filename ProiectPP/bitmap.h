#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char B, G, R;
}PIXEL;

PIXEL pixelXORpixel(PIXEL pixel1, PIXEL pixel2) {
	PIXEL result;

	result.B = pixel1.B ^ pixel2.B;
	result.G = pixel1.G ^ pixel2.G;
	result.R = pixel1.R ^ pixel2.R;

	return result;
}

PIXEL pixelXORu_int(PIXEL pixel, unsigned int u_int) {
	return pixelXORpixel(pixel, *((PIXEL*)&u_int));
}

int loadBMP(const char* path, unsigned int* pixelNo, PIXEL** pixel) {
	FILE *inputBMP = fopen(path, "rb");

	if (inputBMP == NULL) {
		printf("Error opening bitmap file %s!\n", path);
		return 1;
	}

	fseek(inputBMP, 18, SEEK_SET);

	unsigned int width, height, padding;

	fread(&width, sizeof(unsigned int), 1, inputBMP);
	fread(&height, sizeof(unsigned int), 1, inputBMP);

	*pixelNo = width * height;
	*pixel = (PIXEL*)malloc(*pixelNo * sizeof(PIXEL));

	fseek(inputBMP, 54, SEEK_SET);

	int line, collumn; 

	if (width % 4 != 0)
		padding = 4 - (3 * width) % 4;
	else
		padding = 0;

	for (line = height - 1; line >= 0; --line) {
		for (collumn = 0; collumn < width; ++collumn)
			fread(*pixel + line * height + collumn, sizeof(PIXEL), 1, inputBMP);

		fseek(inputBMP, padding, SEEK_CUR);
	}

	fclose(inputBMP);

	return 0;
}

int saveBMP(const char* newPath, const char* oldPath, unsigned int pixelNo, PIXEL* pixel) {
	FILE *outputBMP = fopen(newPath, "wb");

	if (outputBMP == NULL) {
		printf("Error creating bitmap file %s!\n", newPath);
		return 1;
	}

	FILE *inputBMP = fopen(oldPath, "rb");

	if (inputBMP == NULL) {
		printf("Error creating bitmap file %s!\n", oldPath);
		return 1;
	}

	unsigned int width, height, padding;

	fseek(inputBMP, 18, SEEK_SET);

	fread(&width, sizeof(unsigned int), 1, inputBMP);
	fread(&height, sizeof(unsigned int), 1, inputBMP);

	if (width % 4 != 0)
		padding = 4 - (3 * width) % 4;
	else
		padding = 0;

	fseek(inputBMP, 0, SEEK_SET);

	unsigned char currentByte;
	int iterator;

	for (iterator = 0; iterator < 54; ++iterator) {
		fread(&currentByte, sizeof(unsigned char), 1, inputBMP);
		fwrite(&currentByte, sizeof(unsigned char), 1, outputBMP);
	}

	fclose(inputBMP);

	int line, collumn;

	for (line = height - 1; line >= 0; --line) {
		for (collumn = 0; collumn < width; ++collumn)
			fwrite(pixel + line * height + collumn, sizeof(PIXEL), 1, outputBMP);

		char a = '0';
		unsigned int padd;
		for (padd = 0; padd < padding; ++padd)
			fwrite(&a, sizeof(char), 1, outputBMP);
	}

	fclose(outputBMP);

	return 0;
}
