#include <stdio.h>
#include <string.h>
#include "encryptions.h"
#include "chisquared.h"

int main() {
	char *inputPath = (char*)malloc(101 * sizeof(char));
	char *encryptedInputPath = (char*)malloc(110 * sizeof(char));
	char *decryptedInputPath = (char*)malloc(110 * sizeof(char));
	char *secret_keyPath = (char*)malloc(101 * sizeof(char));

	printf("Bitmap file to encrypt: ");
	fgets(inputPath, 101, stdin);
	inputPath[strlen(inputPath) - 1] = '\0';

	printf("Bitmap file to save encryption into: ");
	fgets(encryptedInputPath, 110, stdin);
	encryptedInputPath[strlen(encryptedInputPath) - 1] = '\0';

	printf("Bitmap file to save decryption into: ");
	fgets(decryptedInputPath, 110, stdin);
	decryptedInputPath[strlen(decryptedInputPath) - 1] = '\0';

	printf("File containing secret key: ");
	fgets(secret_keyPath, 101, stdin);
	secret_keyPath[strlen(secret_keyPath) - 1] = '\0';

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

	return 0;
}