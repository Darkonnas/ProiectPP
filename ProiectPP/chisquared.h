#pragma once
#include "bitmap.h"
#include <string.h>
#include <stdio.h>

int chiSquared(const char* testBMPPath) {
	printf("Performing chi-squared test on file %s ...\n", testBMPPath);
	
	unsigned int pixelNo;
	PIXEL *pixelArray = NULL;
	unsigned char *header = NULL;
	int load_error = loadBMPLiniar(testBMPPath, &header, &pixelNo, &pixelArray);

	if (load_error) {
		printf("chisquared.h:chiSquared:12 - Error loading file %s!\n", testBMPPath);
		return 1;
	}

	free(header);

	unsigned int *redFreq = NULL, *greenFreq = NULL, *blueFreq = NULL;
	redFreq = (unsigned int*)malloc(256 * sizeof(unsigned int));

	if (redFreq == NULL) {
		printf("chisquared.h:chiSquared:22 - Error allocating array <redFreq>!\n");
		free(pixelArray);
		return 1;
	}

	greenFreq = (unsigned int*)malloc(256 * sizeof(unsigned int));

	if (greenFreq == NULL) {
		printf("chisquared.h:chiSquared:30 - Error allocating array <greenFreq>!\n");
		free(pixelArray); free(redFreq);
		return 1;
	}

	blueFreq = (unsigned int*)malloc(256 * sizeof(unsigned int));

	if (blueFreq == NULL) {
		printf("chisquared.h:chiSquared:38 - Error allocating array <blueFreq>!\n");
		free(pixelArray); free(redFreq); free(greenFreq);
		return 1;
	}

	memset(redFreq, 0, 256 * sizeof(unsigned int));
	memset(greenFreq, 0, 256 * sizeof(unsigned int));
	memset(blueFreq, 0, 256 * sizeof(unsigned int));

	int iterator;

	for (iterator = 0; iterator < pixelNo; ++iterator) {
		++redFreq[pixelArray[iterator].R];
		++greenFreq[pixelArray[iterator].G];
		++blueFreq[pixelArray[iterator].B];
	}

	double redChannelValue = 0, greenChannelValue = 0, blueChannelValue = 0, averageValue = pixelNo / 256;

	
	for (iterator = 0; iterator < 256; ++iterator) {
		redChannelValue += ((redFreq[iterator] - averageValue) * (redFreq[iterator] - averageValue)) / averageValue;
		greenChannelValue += ((greenFreq[iterator] - averageValue) * (greenFreq[iterator] - averageValue)) / averageValue;
		blueChannelValue += ((blueFreq[iterator] - averageValue) * (blueFreq[iterator] - averageValue)) / averageValue;
	}

	printf("Red value: %.2lf\nGreen value: %.2lf\nBlue value: %.2lf\n", redChannelValue, greenChannelValue, blueChannelValue);

	free(pixelArray); free(redFreq); free(greenFreq); free(blueFreq);

	printf("Chi-squared test on bitmap file %s has finished!\n", testBMPPath);
	
	return 0;
}