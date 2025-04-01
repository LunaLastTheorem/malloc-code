#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "alloc.h"

int main()
{
	// mmap to get page
	if (init_alloc())
		return 1; // mmap failed

	char *str = alloc(512);
	char *str2 = alloc(512);

	if (str == NULL || str2 == NULL)
	{
		printf("alloc failed\n");
		return 1;
	}

	strcpy(str, "Hello, world!");
	if (strcmp("Hello, world!", str))
		printf("Hello, world! test failed\n");

	else
		printf("Hello, world! test passed\n");

	dealloc(str);
	dealloc(str2);

	printf("Elementary tests passed\n");

	printf("Starting comprehensive tests (see details in code)\n");

	/*
	Comprehensive tests:
	1. Populate entire thing in blocks of for loop (a's, b's, c's, and d's) equal size.
	2. Dealloc c's, reallocate and replace with x's.
	3.
	*/

	/*** test 1 ****/

	// Generating ideal strings for comparison
	char stringA[1000], stringB[1000], stringC[1000], stringD[1000], stringX[1000];
	for (int i = 0; i < 999; i++)
	{
		stringA[i] = 'a';
		stringB[i] = 'b';
		stringC[i] = 'c';
		stringD[i] = 'd';
		stringX[i] = 'x';
	}

	stringA[999] = stringB[999] = stringC[999] = stringD[999] = stringX[999] = '\0';

	char *strA = alloc(1000);
	char *strB = alloc(1000);
	char *strC = alloc(1000);
	char *strD = alloc(1000);

	for (int i = 0; i < 999; i++)
	{
		strA[i] = 'a';
		strB[i] = 'b';
		strC[i] = 'c';
		strD[i] = 'd';
	}

	strA[999] = strB[999] = strC[999] = strD[999] = '\0';

	if (strcmp(stringA, strA) == 0 && strcmp(stringB, strB) == 0 && strcmp(stringC, strC) == 0 && strcmp(stringD, strD) == 0)
		printf("Test 1 passed: allocated 4 chunks of 1KB each\n");
	else
		printf("Test 1 failed: A: %d, B: %d, C: %d, D: %d\n", strcmp(stringA, strA), strcmp(stringB, strB), strcmp(stringC, strC), strcmp(stringD, strD));

	/**** test 2 ****/

	dealloc(strC);

	char *strX = alloc(1000);
	for (int i = 0; i < 999; i++)
	{
		strX[i] = 'x';
	}
	strX[999] = '\0';

	if (strcmp(stringX, strX) == 0)
		printf("Test 2 passed: dealloc and realloc worked\n");
	else
		printf("Test 2 failed: X: %d\n", strcmp(stringX, strX));

	return 0;
}
