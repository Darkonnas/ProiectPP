#pragma once
int getRandArray(unsigned int seed, unsigned int**resultArray, unsigned int resultSize) {
	*resultArray = (unsigned int*)malloc(resultSize*(sizeof(unsigned int)));

	if (*resultArray == NULL) {
		printf("random.h:getRandArray:4 - Insufficient memory for random array <resultArray> allocation!\n");
		return 1;
	}

	unsigned int currentRand = seed, iterator;

	for (iterator = 0; iterator < resultSize; ++iterator) {
		currentRand ^= currentRand << 13;
		currentRand ^= currentRand >> 17;
		currentRand ^= currentRand << 5;
		*(*resultArray + iterator) = currentRand;
	}

	return 0;
}