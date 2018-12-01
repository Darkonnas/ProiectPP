#pragma once

void getRandArray(unsigned int seed, unsigned int**resultArray, unsigned int resultSize) {
	*resultArray = (unsigned int*)malloc(resultSize*(sizeof(unsigned int)));

	unsigned int currentRand = seed, iterator;

	for (iterator = 0; iterator < resultSize; ++iterator) {
		currentRand ^= currentRand << 13;
		currentRand ^= currentRand >> 17;
		currentRand ^= currentRand << 5;
		*(*resultArray + iterator) = currentRand;
	}
}