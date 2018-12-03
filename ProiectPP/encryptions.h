#pragma once
#include "bitmap.h"
#include "random.h"

int encryptBMP(const char* originalBMPPath, const char* encryptedBMPPath, const char* keyFilePath) {
	printf("Encrypting file %s ...\n", originalBMPPath);

	unsigned int pixelNo;
	PIXEL *pixelArray = NULL;
	int load_error = loadBMP(originalBMPPath, &pixelNo, &pixelArray);

	if (load_error) {
		printf("encryptions.h:encryptBMP:10 - Error loading file %s!\n", originalBMPPath);
		return 1;
	}

	unsigned int R0, SV;
	FILE *keyFile = fopen(keyFilePath, "r");

	if (keyFile == NULL) {
		printf("encryptions.h:encryptBMP:18 - Error opening file %s!\n", keyFilePath);
		return 1;
	}

	int read = fscanf(keyFile, "%u%u", &R0, &SV);
	fclose(keyFile);

	if (read == 0) {
		printf("encryptions.h:encryptBMP:25 - Error reading from file %s!\n", keyFilePath);
		return 1;
	}

	printf("R0: %u\tSV: %u\n", R0, SV);

	unsigned int *randArray = NULL, *permArray = NULL;

	int rand_error = getRandArray(R0, &randArray, 2 * pixelNo - 1);

	if (rand_error) {
		printf("encryptions.h:encryptBMP:32 - Error generating random array!\n");
		return 1;
	}

	int iterator;

	permArray = (unsigned int*)malloc(pixelNo * sizeof(unsigned int));

	if (permArray == NULL) {
		printf("encryptions.h:encryptBMP:41 - Error allocating array <permArray>!\n");
		return 1;
	}

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

	if (auxArray == NULL) {
		printf("encryptions.h:encryptBMP:61 - Error allocating array <auxArray>!\n");
		return 1;
	}

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
		printf("encryptions.h:encryptBMP:84 - Error saving file %s!\n", encryptedBMPPath);
		return 1;
	}

	printf("Encrypted file %s into file %s!\n", originalBMPPath, encryptedBMPPath);

	return 0;
}

int decryptBMP(const char* encryptedBMPPath, const char* decryptedBMPPath, const char* keyFilePath) {
	printf("Decrypting file %s ...\n", encryptedBMPPath);

	unsigned int pixelNo;
	PIXEL *pixelArray = NULL;
	int load_error = loadBMP(encryptedBMPPath, &pixelNo, &pixelArray);

	if (load_error) {
		printf("encryptions.h:decryptBMP:103 - Error loading file %s!\n", encryptedBMPPath);
		return 1;
	}

	unsigned int R0, SV;
	FILE *keyFile = fopen(keyFilePath, "r");

	if (keyFile == NULL) {
		printf("encryptions.h:decryptBMP:113 - Error opening file %s!\n", keyFilePath);
		return 1;
	}

	int read = fscanf(keyFile, "%u%u", &R0, &SV);
	fclose(keyFile);

	if (read == 0) {
		printf("encryptions.h:encryptBMP:25 - Error reading from file %s\n!", keyFilePath);
		return 1;
	}

	printf("R0: %u\tSV: %u\n", R0, SV);

	unsigned int *randArray = NULL, *permArray = NULL;

	int rand_error = getRandArray(R0, &randArray, 2 * pixelNo - 1);

	if (rand_error) {
		printf("encryptions.h:decryptBMP:125 - Error generating random array!\n");
		return 1;
	}

	int iterator;

	for (iterator = pixelNo - 1; iterator >=0; --iterator) {
		if (iterator == 0)
			pixelArray[iterator] = pixelXORu_int(pixelXORu_int(pixelArray[iterator], SV), randArray[iterator + pixelNo - 1]);
		else
			pixelArray[iterator] = pixelXORu_int(pixelXORpixel(pixelArray[iterator - 1], pixelArray[iterator]), randArray[iterator + pixelNo - 1]);
	}

	printf("Modified pixels!\n");

	permArray = (unsigned int*)malloc(pixelNo * sizeof(unsigned int));

	if (permArray == NULL) {
		printf("encryptions.h:decryptBMP:145 - Error allocating array <permArray>!\n");
		return 1;
	}

	for (iterator = 0; iterator < pixelNo; ++iterator)
		permArray[iterator] = iterator;

	for (iterator = pixelNo - 1; iterator > 0; --iterator) {
		unsigned int shiftPos = randArray[pixelNo - 1 - iterator] % (iterator + 1);
		unsigned int aux = permArray[shiftPos];
		permArray[shiftPos] = permArray[iterator];
		permArray[iterator] = aux;
	}

	unsigned int *inversePerm = malloc(pixelNo * sizeof(unsigned int));

	if (inversePerm == NULL) {
		printf("encryptions.h:decryptBMP:162 - Error allocating array <inversePerm>!\n");
		return 1;
	}

	for (iterator = 0; iterator < pixelNo; ++iterator) {
		inversePerm[permArray[iterator]] = iterator;
	}

	printf("Generated inverse permutation!\n");

	PIXEL *auxArray = NULL;
	auxArray = (PIXEL*)malloc(pixelNo * sizeof(PIXEL));

	if (auxArray == NULL) {
		printf("encryptions.h:decryptBMP:168 - Error allocating array <auxArray>!\n");
		return 1;
	}

	for (iterator = 0; iterator < pixelNo; ++iterator)
		auxArray[inversePerm[iterator]] = pixelArray[iterator];

	free(pixelArray); free(permArray); free(inversePerm);

	printf("Permuted pixels!\n");

	int save_error = saveBMP(decryptedBMPPath, encryptedBMPPath, pixelNo, auxArray);

	free(auxArray); free(randArray);

	if (save_error) {
		printf("encryptions.h:decryptBMP:182 - Error saving file %s!\n", decryptedBMPPath);
		return 1;
	}

	printf("Decrypted file %s into file %s!\n", encryptedBMPPath, decryptedBMPPath);

	return 0;
}

