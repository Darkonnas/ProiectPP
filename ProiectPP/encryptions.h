#pragma once
#include "bitmap.h"
#include "random.h"

int encryptBMP(const char* originalBMPPath, const char* encryptedBMPPath, const char* keyFilePath) {
	printf("Encrypting bitmap file %s ...\n", originalBMPPath);

	unsigned int pixelNo;
	PIXEL *pixelArray = NULL;
	int load_error = loadBMP(originalBMPPath, &pixelNo, &pixelArray);

	if (load_error) {
		printf("Error loading bitmap file %s!\n", originalBMPPath);
		return 1;
	}

	unsigned int R0, SV;
	FILE *keyFile = fopen(keyFilePath, "r");

	if (keyFile == NULL) {
		printf("Error opening secret key file %s!\n", keyFilePath);
		return 1;
	}

	fscanf(keyFile, "%u%u", &R0, &SV);
	fclose(keyFile);

	printf("R0: %u\tSV: %u\n", R0, SV);

	unsigned int *randArray = NULL, *permArray = NULL;

	getRandArray(R0, &randArray, 2 * pixelNo - 1);

	int iterator;

	permArray = (unsigned int*)malloc(pixelNo * sizeof(unsigned int));

	for (iterator = 0; iterator < pixelNo; ++iterator)
		permArray[iterator] = iterator;

	for (iterator = pixelNo - 1; iterator > 0; --iterator) {
		unsigned int shiftPos = randArray[pixelNo - 1 - iterator] % (iterator + 1);
		unsigned int aux = permArray[shiftPos];
		permArray[shiftPos] = permArray[iterator];
		permArray[iterator] = aux;
	}

	printf("Generated permutation!\n");

	PIXEL *auxArray = NULL;
	auxArray = (PIXEL*)malloc(pixelNo * sizeof(PIXEL));

	for (iterator = 0; iterator < pixelNo; ++iterator)
		auxArray[permArray[iterator]] = pixelArray[iterator];

	free(pixelArray); free(permArray);

	printf("Permuted pixels!\n");

	for (iterator = 0; iterator < pixelNo; ++iterator) {
		if (iterator == 0)
			auxArray[iterator] = pixelXORu_int(pixelXORu_int(auxArray[iterator], SV), randArray[iterator + pixelNo - 1]);
		else
			auxArray[iterator] = pixelXORu_int(pixelXORpixel(auxArray[iterator - 1], auxArray[iterator]), randArray[iterator + pixelNo - 1]);
	}

	printf("Modified pixels!\n");

	int save_error = saveBMP(encryptedBMPPath, originalBMPPath, pixelNo, auxArray);

	free(auxArray); free(randArray);

	if (save_error) {
		printf("Unable to save bitmap file %s!\n", encryptedBMPPath);
		return 1;
	}

	printf("Encrypted bitmap file %s into file %s!\n", originalBMPPath, encryptedBMPPath);

	return 0;
}

int decryptBMP(const char* encryptedBMPPath, const char* decryptedBMPPath, const char* keyFilePath) {
	printf("Decrypting bitmap file %s ...\n", encryptedBMPPath);

	unsigned int pixelNo;
	PIXEL *pixelArray = NULL;
	int load_error = loadBMP(encryptedBMPPath, &pixelNo, &pixelArray);

	if (load_error) {
		printf("Error loading bitmap file %s!\n", encryptedBMPPath);
		return 1;
	}

	unsigned int R0, SV;
	FILE *keyFile = fopen(keyFilePath, "r");

	if (keyFile == NULL) {
		printf("Error opening secret key file %s!\n", keyFilePath);
		return 1;
	}

	fscanf(keyFile, "%u%u", &R0, &SV);
	fclose(keyFile);

	printf("R0: %u\tSV: %u\n", R0, SV);

	unsigned int *randArray = NULL, *permArray = NULL;

	getRandArray(R0, &randArray, 2 * pixelNo - 1);

	int iterator;

	for (iterator = pixelNo - 1; iterator >=0; --iterator) {
		if (iterator == 0)
			pixelArray[iterator] = pixelXORu_int(pixelXORu_int(pixelArray[iterator], SV), randArray[iterator + pixelNo - 1]);
		else
			pixelArray[iterator] = pixelXORu_int(pixelXORpixel(pixelArray[iterator - 1], pixelArray[iterator]), randArray[iterator + pixelNo - 1]);
	}

	printf("Modified pixels!\n");

	permArray = (unsigned int*)malloc(pixelNo * sizeof(unsigned int));

	for (iterator = 0; iterator < pixelNo; ++iterator)
		permArray[iterator] = iterator;

	for (iterator = pixelNo - 1; iterator > 0; --iterator) {
		unsigned int shiftPos = randArray[pixelNo - 1 - iterator] % (iterator + 1);
		unsigned int aux = permArray[shiftPos];
		permArray[shiftPos] = permArray[iterator];
		permArray[iterator] = aux;
	}

	unsigned int *inversePerm = malloc(pixelNo * sizeof(unsigned int));
	for (iterator = 0; iterator < pixelNo; ++iterator) {
		inversePerm[permArray[iterator]] = iterator;
	}

	printf("Generated inverse permutation!\n");

	PIXEL *auxArray = NULL;
	auxArray = (PIXEL*)malloc(pixelNo * sizeof(PIXEL));

	for (iterator = 0; iterator < pixelNo; ++iterator)
		auxArray[inversePerm[iterator]] = pixelArray[iterator];

	free(pixelArray); free(permArray); free(inversePerm);

	printf("Permuted pixels!\n");

	int save_error = saveBMP(decryptedBMPPath, encryptedBMPPath, pixelNo, auxArray);

	free(auxArray); free(randArray);

	if (save_error) {
		printf("Unable to save bitmap file %s!\n", decryptedBMPPath);
		return 1;
	}

	printf("Decrypted bitmap file %s into file %s!\n", encryptedBMPPath, decryptedBMPPath);

	return 0;
}

