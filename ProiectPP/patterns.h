#pragma once
#include "bitmap.h"
#include <math.h>

typedef struct {
	struct {
		unsigned int line, collumn;
	}top_left, bottom_right;

	unsigned int area;
	double correlation;
	PIXEL color;
}DETECTION;

int compareDetections(const void*, const void*);
double isCovering(DETECTION, DETECTION);
int deleteNonMax(DETECTION**, unsigned int*, double);
int startMatch(const char*, const char*, FILE*);
int match(const char*, const char*, double, unsigned int*, DETECTION**, PIXEL);
void getCorrelation(PIXEL**, PIXEL**, DETECTION*);
void colorDetection(PIXEL**, DETECTION);

int compareDetections(const void* d1, const void* d2) {
	if (((DETECTION*)d1)->correlation > ((DETECTION*)d2)->correlation) return -1;
	if (((DETECTION*)d1)->correlation == ((DETECTION*)d2)->correlation) return 0;
    return 1;
}

double isCovering(DETECTION d1, DETECTION d2) {
	DETECTION intersection;
	intersection.top_left.line = max(d1.top_left.line, d2.top_left.line);
	intersection.top_left.collumn = max(d1.top_left.collumn, d2.top_left.collumn);
	intersection.bottom_right.line = min(d1.bottom_right.line, d2.bottom_right.line);
	intersection.bottom_right.collumn = min(d1.bottom_right.collumn, d2.bottom_right.collumn);

	if (intersection.bottom_right.line < intersection.top_left.line || intersection.bottom_right.collumn < intersection.top_left.collumn)
		return 0;

	intersection.area = (intersection.bottom_right.line - intersection.top_left.line + 1)*(intersection.bottom_right.collumn - intersection.top_left.collumn + 1);

	return (double)intersection.area / ((double)d1.area + (double)d2.area - (double)intersection.area);
}

int deleteNonMax(DETECTION** detection, unsigned int* detectionNo, double coverPercentage) {
	qsort(*detection, *detectionNo, sizeof(DETECTION), compareDetections);

	int iterator1, iterator2;

	for (iterator1 = 0; iterator1 < *detectionNo; ++iterator1)
		for (iterator2 = iterator1 + 1; iterator2 < *detectionNo; ++iterator2)
			if (isCovering((*detection)[iterator1], (*detection)[iterator2]) > coverPercentage) {
				int deiterator;
				for (deiterator = iterator2; deiterator < *detectionNo; ++deiterator)
					(*detection)[deiterator] = (*detection)[deiterator + 1];

				--(*detectionNo);
				DETECTION *aux = (DETECTION*)realloc(*detection, *detectionNo * sizeof(DETECTION));

				if (aux == NULL) {
					printf("deleteNonMax:57 - Error reallocating array <*detection>!\n");
					return 1;
				}

				*detection = aux;

				--iterator2;
			}
	return 0;
}

int startMatch(const char* workplacePath, const char* resultPath, FILE* input) {
	printf("Starting pattern matching algorithm on file %s...\n", workplacePath);
	
	double threshold;
	int read = fscanf(input, "%lf", &threshold);

	if (read == 0) {
		printf("pattern.h:startMatch:75 - Error reading threshold from input!\n");
		return 1;
	}

	printf("Threshold: %lf\n", threshold);

	unsigned int patternNo;
	read = fscanf(input, "%u", &patternNo);

	if (read == 0) {
		printf("pattern.h:startMatch:85 - Error reading patternNo from input!\n");
		return 1;
	}

	printf("Number of patterns: %u\n", patternNo);
	char **pattern = (char**)malloc(patternNo * sizeof(char*));

	int iterator;

	fgetc(input);

	for (iterator = 0; iterator < patternNo; ++iterator) {
		pattern[iterator] = (char*)malloc(101 * sizeof(char));
		fgets(pattern[iterator], 101, input);
		pattern[iterator][strlen(pattern[iterator]) - 1] = '\0';
	}

	PIXEL *color = (PIXEL*)malloc(10 * sizeof(PIXEL));

	if (color == NULL) {
		printf("pattern.h:startMatch:105 - Error allocating array <color>!\n");
		return 1;
	}

	color[0] = (PIXEL) { 0, 0, 255 };
	color[1] = (PIXEL) { 0, 255, 255 };
	color[2] = (PIXEL) { 0, 255, 0 };
	color[3] = (PIXEL) { 255, 255, 0 };
	color[4] = (PIXEL) { 255, 0, 255 };
	color[5] = (PIXEL) { 255, 0, 0 };
	color[6] = (PIXEL) { 192, 192, 192 };
	color[7] = (PIXEL) { 0, 140, 255 };
	color[8] = (PIXEL) { 128, 0, 128 };
	color[9] = (PIXEL) { 0, 0, 128 };

	char* workplacePath_grayscale = (char*)malloc(110 * sizeof(char));

	if (workplacePath_grayscale == NULL) {
		printf("pattern.h:startMatch:123 - Error allocating array <workplacePath_grayscale>!\n");
		return 1;
	}

	char* workplace_extension = strchr(workplacePath, '.');
	memcpy(workplacePath_grayscale, workplacePath, (strlen(workplacePath) - strlen(workplace_extension)) * sizeof(char));
	workplacePath_grayscale[strlen(workplacePath) - strlen(workplace_extension)] = '\0';
	strcat(workplacePath_grayscale, "_g");
	strcat(workplacePath_grayscale, workplace_extension);
	int workplace_conversion_error = convertGrayscale(workplacePath, workplacePath_grayscale);

	if (workplace_conversion_error) {
		printf("pattern.h:startMatch:102 - Error converting file %s to grayscale!\n", workplacePath);
		free(workplacePath_grayscale); free(color);

		for (iterator = 0; iterator < patternNo; ++iterator) 
			free(pattern[iterator]);
		
		free(pattern);

		return 1;
	}

	DETECTION *detection = NULL;
	unsigned int detectionNo = 0;
	for (iterator = 0; iterator < patternNo; ++iterator) {
		printf("Matching pattern: %s\n", pattern[iterator]);
		int match_error = match(workplacePath_grayscale, pattern[iterator], threshold, &detectionNo, &detection, color[iterator]);

		if (match_error) {
			printf("pattern.h:startMatch:120 - Error matching pattern %s in file %s!\n", pattern[iterator], workplacePath);
			free(color);  free(workplacePath_grayscale);
			
			for (iterator = 0; iterator < patternNo; ++iterator)
				free(pattern[iterator]);

			free(pattern);

			if (detection)
				free(detection);

			return 1;
		}
	}

	remove(workplacePath_grayscale);
	free(workplacePath_grayscale);

	for (iterator = 0; iterator < patternNo; ++iterator)
		free(pattern[iterator]);

	free(pattern);
	
	int delete_error = deleteNonMax(&detection, &detectionNo, .2);

	if (delete_error) {
		printf("pattern.h:startMatch:155 - Error running non-max deletion algorithm!\n");
		free(detection); free(color);
		return 1;
	}
	
	unsigned char *result_header = NULL;
	unsigned int result_width = 0, result_height = 0;
	PIXEL **resultPixel = NULL;

	int workplace_load_error = loadBMP(workplacePath, &result_header, &result_width, &result_height, &resultPixel);

	if (workplace_load_error) {
		printf("pattern.h:startMatch:152 - Error loading file %s!\n", workplacePath);
		free(color); free(detection);
		return 1;
	}

	for (iterator = 0; iterator < detectionNo; ++iterator)
		colorDetection(resultPixel, detection[iterator]);

	int result_save_error = saveBMP(resultPath, result_header, result_width, result_height, resultPixel);

	free(color); free(detection); free(result_header);

	for (iterator = 0; iterator < result_height; ++iterator)
		free(resultPixel[iterator]);

	free(resultPixel);

	if (result_save_error) {
		printf("pattern.h:startMatch:163 - Error saving file %s!\n", resultPath);
		return 1;
	}

	printf("Pattern matching algorithm on file %s has finished!\n", workplacePath);
	return 0;
}

int match(const char* workplacePath_grayscale, const char* pattern, double threshold, unsigned int* detectionNo, DETECTION** detection, PIXEL color) {
	//Declarative zone

	unsigned char *workplace_header = NULL, *pattern_header = NULL;
	unsigned int workplace_width = 0, workplace_height = 0, pattern_width = 0, pattern_height = 0;
	PIXEL **workplacePixel = NULL, **patternPixel = NULL;

	//Getting the grayscale paths and their respective pixel maps

	int workplace_load_error = loadBMP(workplacePath_grayscale, &workplace_header, &workplace_width, &workplace_height, &workplacePixel);

	if (workplace_load_error) {
		printf("pattern.h:match:190 - Error loading file %s!\n", workplacePath_grayscale);
		return 1;
	}

	//printf("Converted workplace to grayscale!\n");

	char* pattern_grayscale = (char*)malloc(110 * sizeof(char));
	char* pattern_extension = strchr(pattern, '.');
	memcpy(pattern_grayscale, pattern, (strlen(pattern) - strlen(pattern_extension)) * sizeof(char));
	pattern_grayscale[strlen(pattern) - strlen(pattern_extension)] = '\0';
	strcat(pattern_grayscale, "_g");
	strcat(pattern_grayscale, pattern_extension);

	int pattern_conversion_error = convertGrayscale(pattern, pattern_grayscale);

	if (pattern_conversion_error) {
		printf("pattern.h:match:206 - Error converting file %s to grayscale!\n", pattern);
		free(pattern_grayscale); free(workplace_header);

		int deiterator;

		for (deiterator = 0; deiterator < workplace_height; ++deiterator)
			free(workplacePixel[deiterator]);

		free(workplacePixel);

		return 1;
	}

	int pattern_load_error = loadBMP(pattern_grayscale, &pattern_header, &pattern_width, &pattern_height, &patternPixel);

	if (pattern_load_error) {
		printf("pattern.h:match:222 - Error loading file %s!\n", pattern_grayscale);
		free(pattern_grayscale); free(workplace_header);

		int deiterator;

		for (deiterator = 0; deiterator < workplace_height; ++deiterator)
			free(workplacePixel[deiterator]);

		free(workplacePixel);
		return 1;
	}

	//printf("Converted pattern to grayscale!\n");

	//Correlation calculation

	DETECTION currentDetection;
	
	int line, collumn;
	for (line = 0; line < workplace_height - pattern_height; ++line)
		for (collumn = 0; collumn < workplace_width - pattern_width; ++collumn) {
			currentDetection.top_left.line = line; currentDetection.top_left.collumn = collumn; currentDetection.bottom_right.line = line + pattern_height - 1; currentDetection.bottom_right.collumn = collumn + pattern_width - 1;
			currentDetection.area = (currentDetection.bottom_right.line - currentDetection.top_left.line + 1)*(currentDetection.bottom_right.collumn - currentDetection.top_left.collumn + 1);
			currentDetection.correlation = 0;
			currentDetection.color = color;
			getCorrelation(workplacePixel, patternPixel, &currentDetection);

			if (currentDetection.correlation >= threshold) {
				if (*detection == NULL) {
					*detectionNo = 1;
					*detection = (DETECTION*)malloc(*detectionNo * sizeof(DETECTION));
					(*detection)[*detectionNo - 1] = currentDetection;
				}
				else {
					++(*detectionNo);
					DETECTION * aux = (DETECTION*)realloc(*detection, *detectionNo * sizeof(DETECTION));

					if (aux == NULL) {
						printf("pattern.h:match:297 - Error reallocating array <*detection>");
						return 1;
					}

					*detection = aux;
					(*detection)[*detectionNo - 1] = currentDetection;
				}
			}
		}

	//Deallocation zone

	remove(pattern_grayscale);
	free(pattern_grayscale);
	free(workplace_header); free(pattern_header);
	int iterator;

	for (iterator = 0; iterator < workplace_height; ++iterator)
		free(workplacePixel[iterator]);
	free(workplacePixel);

	for (iterator = 0; iterator < pattern_height; ++iterator)
		free(patternPixel[iterator]);
	free(patternPixel);

	return 0;
}

void getCorrelation(PIXEL** workplacePixel, PIXEL** patternPixel, DETECTION* outline) {
	double workplace_average, pattern_average, workplace_sum = 0, pattern_sum = 0, workplace_sd, pattern_sd, workplace_sd_sum = 0, pattern_sd_sum = 0;
	unsigned int pixelNo = (outline->bottom_right.line - outline->top_left.line + 1)*(outline->bottom_right.collumn - outline->top_left.collumn + 1); //This should be constant
	int line, collumn;

	for (line = outline->top_left.line; line <= outline->bottom_right.line; ++line)
		for (collumn = outline->top_left.collumn; collumn <= outline->bottom_right.collumn; ++collumn) {
			workplace_sum += workplacePixel[line][collumn].R;
			pattern_sum += patternPixel[line - outline->top_left.line][collumn - outline->top_left.collumn].R;
		}

	workplace_average = workplace_sum / pixelNo;
	pattern_average = pattern_sum / pixelNo;

	for (line = outline->top_left.line; line <= outline->bottom_right.line; ++line)
		for (collumn = outline->top_left.collumn; collumn <= outline->bottom_right.collumn; ++collumn) {
			workplace_sd_sum += (workplacePixel[line][collumn].R - workplace_average)*(workplacePixel[line][collumn].R - workplace_average);
			pattern_sd_sum += (patternPixel[line - outline->top_left.line][collumn - outline->top_left.collumn].R - pattern_average)*(patternPixel[line - outline->top_left.line][collumn - outline->top_left.collumn].R - pattern_average);
		}

	workplace_sd = sqrt(1.0 / (pixelNo - 1) * workplace_sd_sum);
	pattern_sd = sqrt(1.0 / (pixelNo - 1) * pattern_sd_sum);

	for (line = outline->top_left.line; line <= outline->bottom_right.line; ++line)
		for (collumn = outline->top_left.collumn; collumn <= outline->bottom_right.collumn; ++collumn)
			outline->correlation += 1.0 / (workplace_sd*pattern_sd)*(workplacePixel[line][collumn].R - workplace_average)*(patternPixel[line - outline->top_left.line][collumn - outline->top_left.collumn].R - pattern_average);
	
	outline->correlation *= 1.0 / pixelNo;
}

void colorDetection(PIXEL** workplacePixel, DETECTION outline) {
	int line, collumn;

	for (line = outline.top_left.line; line <= outline.bottom_right.line; ++line)		
		workplacePixel[line][outline.top_left.collumn] = workplacePixel[line][outline.bottom_right.collumn] = outline.color;
	for (collumn = outline.top_left.collumn; collumn <= outline.bottom_right.collumn; ++collumn)
		workplacePixel[outline.top_left.line][collumn] = workplacePixel[outline.bottom_right.line][collumn] = outline.color;
}