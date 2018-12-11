#include <stdio.h>
#include <string.h>
#include "encryptions.h"
#include "chisquared.h"
#include "patterns.h"

int main() {
	FILE *input = fopen("input.txt", "r");

	char *inputPath = (char*)malloc(101 * sizeof(char));
	char *encryptedInputPath = (char*)malloc(110 * sizeof(char));
	char *decryptedInputPath = (char*)malloc(110 * sizeof(char));
	char *secret_keyPath = (char*)malloc(101 * sizeof(char));

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

	printf("------------------------------------\n");

	int input_test_error = chiSquared(inputPath);

	if (input_test_error) {
		printf("Unable to run chi-squared test on bitmap file %s!\n", inputPath);
		free(inputPath); free(encryptedInputPath); free(decryptedInputPath); free(secret_keyPath);
		return 0;
	}

	printf("------------------------------------\n");

	int enc_test_error = chiSquared(encryptedInputPath);

	if (enc_test_error) {
		printf("Unable to run chi-squared test on bitmap file %s!\n", encryptedInputPath);
		free(inputPath); free(encryptedInputPath); free(decryptedInputPath); free(secret_keyPath);
		return 0;
	}

	free(inputPath); free(encryptedInputPath); free(decryptedInputPath); free(secret_keyPath);
	
	printf("------------------------------------\n");

	char *workplacePath = (char*)malloc(101 * sizeof(char));
	fgets(workplacePath, 101, input);
	workplacePath[strlen(workplacePath) - 1] = '\0';
	printf("Bitmap file to use in pattern-matching: %s\n", workplacePath);

	char *resultPath = (char*)malloc(101 * sizeof(char));
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