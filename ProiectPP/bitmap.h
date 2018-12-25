#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char B, G, R;
}PIXEL;

PIXEL pixelXORpixel(PIXEL, PIXEL);
PIXEL pixelXORu_int(PIXEL, unsigned int);
int loadBMPLiniar(const char*, unsigned char**, unsigned int*, PIXEL**);
int saveBMPLiniar(const char*, unsigned char*, unsigned int, PIXEL*);
int loadBMP(const char*, unsigned char**, unsigned int*, unsigned int*, PIXEL***);
int saveBMP(const char*, unsigned char*, unsigned int, unsigned int, PIXEL**);

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

int loadBMPLiniar(const char* loadPath, unsigned char** header, unsigned int* pixelNo, PIXEL** pixelArray) {
	FILE *inputBMP = fopen(loadPath, "rb");

	if (inputBMP == NULL) {
		printf("bitmap.h:loadBMPLiniar:24 - Error opening file %s!\n", loadPath);
		return 1;
	}

	char *extension = strchr(loadPath, '.');

	if (extension == NULL || strcmp(extension, ".bmp")) {
		printf("bitmap.h:loadBMPLiniar:loadPath - Provided file (%s) is not a bitmap-type file!\n", loadPath);
		fclose(inputBMP);
		return 1;
	}

	*header = (unsigned char*)malloc(54 * sizeof(unsigned char));

	if (*header == NULL) {
		printf("bitmap.h:loadBMPLiniar:38 - Error allocating array <*header>!\n");
		fclose(inputBMP);
		return 1;
	}

	fread(*header, sizeof(unsigned char), 54, inputBMP);

	unsigned int width, height, padding;

	memcpy(&width, *header + 18, sizeof(unsigned int));
	memcpy(&height, *header + 22, sizeof(unsigned int));

	*pixelNo = width * height;
	*pixelArray = (PIXEL*)malloc(*pixelNo * sizeof(PIXEL));

	if (*pixelArray == NULL) {
		printf("bitmap.h:loadBMPLiniar:55 - Error allocating array <*pixel>!\n");
		fclose(inputBMP);
		free(*header);
		return 1;
	}

	int line, collumn; 

	if (width % 4 != 0)
		padding = 4 - (3 * width) % 4;
	else
		padding = 0;

	for (line = height - 1; line >= 0; --line) {
		for (collumn = 0; collumn < width; ++collumn)
			fread(&(*pixelArray)[line * height + collumn], sizeof(PIXEL), 1, inputBMP);

		fseek(inputBMP, padding, SEEK_CUR);
	}

	fclose(inputBMP);

	return 0;
}

int saveBMPLiniar(const char* savePath, unsigned char* header, unsigned int pixelNo, PIXEL* pixelArray) {
	char *extension = strchr(savePath, '.');

	if (extension == NULL || strcmp(extension, ".bmp")) {
		printf("bitmap.h:saveBMPLiniar:savePath - Provided file (%s) is not a bitmap-type file!\n", savePath);
		return 1;
	}
	
	FILE *outputBMP = fopen(savePath, "wb");

	if (outputBMP == NULL) {
		printf("bitmap.h:saveBMPLiniar:91 - Error opening file %s!\n", savePath);
		return 1;
	}

	fwrite(header, sizeof(unsigned char), 54, outputBMP);

	unsigned int width, height, padding;

	memcpy(&width, header + 18, sizeof(unsigned int));
	memcpy(&height, header + 22, sizeof(unsigned int));

	if (width % 4 != 0)
		padding = 4 - (3 * width) % 4;
	else
		padding = 0;;

	int line, collumn;

	for (line = height - 1; line >= 0; --line) {
		for (collumn = 0; collumn < width; ++collumn)
			fwrite(&pixelArray[line * height + collumn], sizeof(PIXEL), 1, outputBMP);

		char a = '0';
		fwrite(&a, sizeof(char), padding, outputBMP);
	}

	fclose(outputBMP);

	return 0;
}

int loadBMP(const char* loadPath, unsigned char** header, unsigned int* width, unsigned int* height, PIXEL*** pixelMap) {
	FILE *inputBMP = fopen(loadPath, "rb");

	if (inputBMP == NULL) {
		printf("bitmap.h:loadBMP:126 - Error opening file %s!\n", loadPath);
		return 1;
	}

	char *extension = strchr(loadPath, '.');

	if (extension == NULL || strcmp(extension, ".bmp")) {
		printf("bitmap.h:loadBMP:loadPath - Provided file (%s) is not a bitmap-type file!\n", loadPath);
		fclose(inputBMP);
		return 1;
	}

	*header = (unsigned char*)malloc(54 * sizeof(unsigned char));

	if (*header == NULL) {
		printf("bitmap.h:loadBMP:141 - Error allocating array <*header>!\n");
		fclose(inputBMP);
		return 1;
	}

	fread(*header, sizeof(unsigned char), 54, inputBMP);

	unsigned int padding;
	int iterator;

	memcpy(width, *header + 18, sizeof(unsigned int));
	memcpy(height, *header + 22, sizeof(unsigned int));

	*pixelMap = (PIXEL**)malloc(*height * sizeof(PIXEL*));

	if (*pixelMap == NULL) {
		printf("bitmap.h:loadBMP:157 - Error allocating array <*pixelMap>!\n");
		fclose(inputBMP);
		free(*header);
		return 1;
	}

	for (iterator = 0; iterator < *height; ++iterator) {
		(*pixelMap)[iterator] = (PIXEL*)malloc(*width * sizeof(PIXEL));

		if ((*pixelMap)[iterator] == NULL) {
			printf("bitmap.h:loadBMP:167 - Error allocating array <(*pixelMap)[%d]>!\n", iterator);
			fclose(inputBMP);
			free(*header);

			int deiterator;

			for (deiterator = 0; deiterator < iterator; ++deiterator)
				free((*pixelMap)[deiterator]);

			free(*pixelMap);
			return 1;
		}
	}

	if (*width % 4 != 0)
		padding = 4 - (3 * *width) % 4;
	else
		padding = 0;

	int line, collumn;

	for (line = *height - 1; line >= 0; --line) {
		for (collumn = 0; collumn < *width; ++collumn)
			fread(&(*pixelMap)[line][collumn], sizeof(PIXEL), 1, inputBMP);

		fseek(inputBMP, padding, SEEK_CUR);
	}

	fclose(inputBMP);

	return 0;
}

int saveBMP(const char* savePath, unsigned char* header, unsigned int width, unsigned int height, PIXEL** pixelMap) {
	char *extension = strchr(savePath, '.');

	if (extension == NULL || strcmp(extension, ".bmp")) {
		printf("bitmap.h:saveBMP:savePath - Provided file (%s) is not a bitmap-type file!\n", savePath);
		return 1;
	}

	FILE *outputBMP = fopen(savePath, "wb");

	if (outputBMP == NULL) {
		printf("bitmap.h:saveBMP:211 - Error opening file %s!\n", savePath);
		return 1;
	}

	fwrite(header, sizeof(unsigned char), 54, outputBMP);

	unsigned int padding;

	if (width % 4 != 0)
		padding = 4 - (3 * width) % 4;
	else
		padding = 0;;

	int line, collumn;

	for (line = height - 1; line >= 0; --line) {
		for (collumn = 0; collumn < width; ++collumn)
			fwrite(&pixelMap[line][collumn], sizeof(PIXEL), 1, outputBMP);

		char a = '0';
		fwrite(&a, sizeof(char), padding, outputBMP);
	}

	fclose(outputBMP);

	return 0;
}

int convertGrayscale(const char* originalPath, const char* resultPath) {
	FILE *originalBMP = fopen(originalPath, "rb");

	if (originalBMP == NULL) {
		printf("bitmap.h:convertGrayscale:243 - Cannot open file %s!\n", originalPath);
		return 1;
	}

	char *originalPath_extension = strchr(originalPath, '.');

	if (strcmp(originalPath_extension, ".bmp")) {
		printf("bitmap.h:convertGrayscale:originalBMPPath - Provided file (%s) is not a bitmap-type file!\n", originalPath);
		fclose(originalBMP);
		return 1;
	}

	char *resultPath_extension = strchr(resultPath, '.');

	if (strcmp(resultPath_extension, ".bmp")) {
		printf("bitmap.h:convertGrayscale:resultBMPPath - Provided file (%s) is not a bitmap-type file!\n", resultPath);
		fclose(originalBMP);
		return 1;
	}

	FILE *resultBMP = fopen(resultPath, "wb");

	if (resultBMP == NULL) {
		printf("bitmap.h:convertGrayscale:266 - Cannot open file %s!\n", resultPath);
		fclose(originalBMP);
		return 1;
	}

	int iterator;
	unsigned char c;

	for (iterator = 0; iterator < 54; ++iterator) {
		fread(&c, sizeof(unsigned char), 1, originalBMP);
		fwrite(&c, sizeof(unsigned char), 1, resultBMP);
	}

	PIXEL current;

	while (fread(&current, sizeof(PIXEL), 1, originalBMP) == 1) {
		current.R = current.G = current.B = 0.299*current.R + 0.587*current.G + 0.114*current.B;
		fwrite(&current, sizeof(PIXEL), 1, resultBMP);
	}

	fclose(originalBMP); fclose(resultBMP);

	return 0;
}