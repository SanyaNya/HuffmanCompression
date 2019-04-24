#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "HuffmanCompressionASCII.h"

//Выводит символы и соответствующие им коды
void PrintHuffmanCodeTable(struct TableSymbol* symbols, int symbolsCount)
{
	printf("Code table[%i]:\n\n", symbolsCount);

	printf("|Code | Frequency | Bits\n");

	for (int x = 0; x < symbolsCount; x++)
	{	
		if((unsigned char)symbols[x].Code >= 32)
			printf("|%c    | %i", symbols[x].Code, symbols[x].Frequency);
		else
			printf("|spec | %i",  symbols[x].Frequency);

		for (int y = 0; y < 10 - (log10(symbols[x].Frequency) + 1); y++) putchar(' ');

		printf("| %s\n", symbols[x].Bits);
	}
}

//Выводит процент сжатия
void PrintCompression(struct TableSymbol* symbols, int symbolsCount, unsigned int textLength)
{
	unsigned int compressSize = GetCompressedSize(symbols, symbolsCount);

	printf("\n\nFile size(bytes): %u\n", textLength);
	printf("Compress file size(bytes): %u\n\n", compressSize);
	printf("Compress: %.2f%%\n", ((float)(textLength - compressSize) / (float)textLength) * 100);
}

int CompareSymbolsBits(const void* s1, const void* s2)
{
	//Просто сравнение строк

	int r = (*(struct TableSymbol*)s1).Length - (*(struct TableSymbol*)s2).Length;

	if (r != 0) return r;

	for (int x = 0; x < (*(struct TableSymbol*)s1).Length; x++)
		if ((*(struct TableSymbol*)s1).Bits[x] != (*(struct TableSymbol*)s2).Bits[x])
			return (*(struct TableSymbol*)s1).Bits[x] - (*(struct TableSymbol*)s2).Bits[x];

	return 0;
}

int main()
{
	clock_t startTime = clock();

	const char* textPath = "text.txt";

	struct TableSymbol symbols[256];
	char strBuffer[65792]; //256 строк по 256 символов

	int symbolsCount;
	unsigned int textLength;

	int res = InitHuffmanCodeTable(textPath, symbols, &symbolsCount, &textLength, strBuffer);

	if (res == 0)
	{
		//Сортируем, чтобы коды символа были от меньшего к большему
		qsort(symbols, symbolsCount, sizeof(struct TableSymbol), CompareSymbolsBits);

		PrintHuffmanCodeTable(symbols, symbolsCount);

		PrintCompression(symbols, symbolsCount, textLength);
	}
	else if (res == 1) printf("File %s open error\n", textPath);
	else printf("File is empty\n");

	clock_t endTime = clock();

	printf("Time: %i ms", endTime - startTime);

	getchar();
}