#include <stdio.h>
#include <string.h>
#include "encryptions.h"
#include "chisquared.h"
#include "patterns.h"

int main() {
	FILE *input = fopen("input.txt", "r");
	
	char *inputPath = (char*)malloc(101 * sizeof(char));

	if (inputPath == NULL) {
		printf("proiectpp.c:main:10 - Error allocating array <inputPath>!\n");
		return 0;
	}

	char *encryptedInputPath = (char*)malloc(110 * sizeof(char));

	if (encryptedInputPath == NULL) {
		printf("proiectpp.c:main:17 - Error allocating array <encryptedInputPath>!\n");
		free(inputPath);
		return 0;
	}

	char *decryptedInputPath = (char*)malloc(110 * sizeof(char));

	if (decryptedInputPath == NULL) {
		printf("proiectpp.c:main:24 - Error allocating array <decryptedInputPath>!\n");
		free(inputPath); free(encryptedInputPath);
		return 0;
	}

	char *secret_keyPath = (char*)malloc(101 * sizeof(char));

	if (secret_keyPath == NULL) {
		printf("proiectpp.c:main:31 - Error allocating array <secret_keyPath>!\n");
		free(inputPath); free(encryptedInputPath); free(decryptedInputPath);
		return 0;
	}

	fgets(inputPath, 101, input);
	inputPath[strlen(inputPath) - 1] = '\0';
	printf("Bitmap file to encrypt: %s\n", inputPath);

	fgets(encryptedInputPath, 110, input);
	encryptedInputPath[strlen(encryptedInputPath) - 1] = '\0';
	printf("Bitmap file to save encryption into: %s\n", encryptedInputPath);

	fgets(decryptedInputPath, 110, input);
	decryptedInputPath[strlen(decryptedInputPath) - 1] = '\0';
	printf("Bitmap file to save decryption into: %s\n", decryptedInputPath);

	fgets(secret_keyPath, 101, input);
	secret_keyPath[strlen(secret_keyPath) - 1] = '\0';
	printf("File containing secret key: %s\n", secret_keyPath);

	int enc_error = encryptBMP(inputPath, encryptedInputPath, secret_keyPath);

	if (enc_error) {
		printf("Unable to encrypt bitmap file %s!\n", inputPath);
		free(inputPath); free(encryptedInputPath); free(decryptedInputPath); free(secret_keyPath);
		return 0;
	}

	printf("------------------------------------\n");

	int dec_error = decryptBMP(encryptedInputPath, decryptedInputPath, secret_keyPath);

	if (dec_error) {
		printf("Unable to decrypt bitmap file %s!\n", encryptedInputPath);
		free(inputPath); free(encryptedInputPath); free(decryptedInputPath); free(secret_keyPath);
		return 0;
	}

	free(decryptedInputPath); free(secret_keyPath);

	printf("------------------------------------\n");

	int input_test_error = chiSquared(inputPath);

	if (input_test_error) {
		printf("Unable to run chi-squared test on bitmap file %s!\n", inputPath);
		free(inputPath); free(encryptedInputPath);
		return 0;
	}

	free(inputPath);

	printf("------------------------------------\n");

	int enc_test_error = chiSquared(encryptedInputPath);

	if (enc_test_error) {
		printf("Unable to run chi-squared test on bitmap file %s!\n", encryptedInputPath);
		free(encryptedInputPath); 
		return 0;
	}

	free(encryptedInputPath);
	
	printf("------------------------------------\n");
	
	char *workplacePath = (char*)malloc(101 * sizeof(char));

	if (workplacePath == NULL) {
		printf("proiectpp.c:main:96 - Error allocating array <workplacePath>!\n");
		return 0;
	}

	fgets(workplacePath, 101, input);
	workplacePath[strlen(workplacePath) - 1] = '\0';
	printf("Bitmap file to use in pattern-matching: %s\n", workplacePath);

	char *resultPath = (char*)malloc(101 * sizeof(char));

	if (resultPath == NULL) {
		printf("proiectpp.c:main:107 - Error allocating array <resultPath>!\n");
		free(workplacePath);
		return 0;
	}

	fgets(resultPath, 101, input);
	resultPath[strlen(resultPath) - 1] = '\0';
	printf("Resulting bitmap file: %s\n", resultPath);

	int error = startMatch(workplacePath, resultPath, input);

	if (error) {
		printf("Match algorithm failed on file %s!\n", workplacePath);
		free(workplacePath); free(resultPath);
		return 0;
	}

	free(workplacePath); free(resultPath);
	return 0;
}