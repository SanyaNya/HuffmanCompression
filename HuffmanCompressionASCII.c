#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HuffmanCompressionASCII.h"

static struct TableSymbolNode
{
	char Code;                       //Код символа
	unsigned int Count;              //Количество символов с кодом Code в тексте

	short leftChildId, rightChildId; //Номера левого и правого дочернего узла
};

static struct TableSymbolNode* nodesPtr;

static int symbolIndex;

static int CompareNodes(const void* id1, const void* id2)
{
	return nodesPtr[*(short*)id2].Count - nodesPtr[*(short*)id1].Count;
}

static int InitFrequencyTable(const char* textPath, short* nodeIds, struct TableSymbolNode* nodes)
{
	FILE* textStream = fopen(textPath, "rb");

	if (textStream == NULL) return 1;

	unsigned char myBuffer[65536];
	int myBufferSize;

	do
	{
		myBufferSize = fread(myBuffer, 1, 65536, textStream);

		for(int x = 0; x < myBufferSize; x++)
			nodes[myBuffer[x]].Count++; //Инкрементируем количество символов с кодом myBuffer[x] в тексте
	} while (myBufferSize != 0);

	fclose(textStream);

	//Присваеваем глобальной переменной чтобы передать в функцию CompareNodes
	nodesPtr = nodes;

	qsort(nodeIds, 256, sizeof(short), CompareNodes);

	return 0;
}

static short GetNonNullNodesCount(short* nodeIds, struct TableSymbolNode* nodes)
{
	//Массив отсортирован
	//Тогда для определения количества кодов символов ищем символ с количеством 0, номер которого будет длиной

	for (int x = 0; x < 256; x++)
		if (nodes[nodeIds[x]].Count == 0) return x;

	return 256;
}

static short CreateTree(short* nodeIds, struct TableSymbolNode* nodes, short minElementIdIndex)
{
	short nodeNextId = 256;

	//Пока количество основных узлов больше 1
	while(minElementIdIndex > 0)
	{
		//Создаем узел с двумя дочерними узлами, которые имеют наименьшее количество символов
		//и декрементируем количество основных узлов в массиве
		short firstMinElementId = nodeIds[minElementIdIndex];
		short secondMinElementId = nodeIds[--minElementIdIndex];

		nodes[nodeNextId].Count = nodes[firstMinElementId].Count + nodes[secondMinElementId].Count;
		nodes[nodeNextId].leftChildId = secondMinElementId;
		nodes[nodeNextId].rightChildId = firstMinElementId;

		unsigned int nodeCount = nodes[nodeNextId].Count;

		//Вставляем новый узел так, чтобы массив был упорядочен
		short insertPos = minElementIdIndex - 1;

		for (; insertPos >= 0; insertPos--)
			if (nodes[nodeIds[insertPos]].Count >= nodeCount) break;

		insertPos++;

		memmove(nodeIds + insertPos + 1, nodeIds + insertPos, sizeof(short) * (minElementIdIndex - insertPos));

		nodeIds[insertPos] = nodeNextId;

		nodeNextId++;
	}

	return nodeIds[0];
}

static void InitSymbolCodes(struct TableSymbolNode* nodes, short treeId, struct TableSymbol* symbols, char* strBuffer, int lvl)
{
	//Если узел является листом
	if (nodes[treeId].leftChildId == -1)
	{
		//Копируем строку, расположенную в начале буфера, в неиспользуемую часть буфера
		char* symbolCodeStr = strBuffer + 256 * (symbolIndex + 1);

		memcpy(symbolCodeStr, strBuffer, lvl);

		//Конец строки
		symbolCodeStr[lvl] = 0;

		//Инициализируем символ
		symbols[symbolIndex].Code = nodes[treeId].Code;
		symbols[symbolIndex].Bits = symbolCodeStr;
		symbols[symbolIndex].Frequency = nodes[treeId].Count;
		symbols[symbolIndex].Length = lvl;

		symbolIndex++;

		return;
	}

	//Идем по левой ветке, добавляем в строку, расположенную в начале буфера,'0'
	strBuffer[lvl] = '0';
	InitSymbolCodes(nodes, nodes[treeId].leftChildId, symbols, strBuffer, lvl + 1);

	//Идем по правой ветке, добавляем в строку, расположенную в начале буфера,'1'
	strBuffer[lvl] = '1';
	InitSymbolCodes(nodes, nodes[treeId].rightChildId, symbols, strBuffer, lvl + 1);
}

int InitHuffmanCodeTable(const char* textPath, struct TableSymbol* symbols, int* out_symbolsCount, unsigned int* out_textLength, char* strBuffer)
{
	//Номера узлов из nodes
	short nodeIds[256] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };
	//Узлы. первые 256 значений проинициализированы так, что коды символа идут по порядку, количество кодов символов равно 0, и номера дочерних узлов -1. Оставшиеся 255 узлов требуются для работы функции CreateTree
	struct TableSymbolNode nodes[511] = { { 0, 0, -1, -1 },{ 1, 0, -1, -1 },{ 2, 0, -1, -1 },{ 3, 0, -1, -1 },{ 4, 0, -1, -1 },{ 5, 0, -1, -1 },{ 6, 0, -1, -1 },{ 7, 0, -1, -1 },{ 8, 0, -1, -1 },{ 9, 0, -1, -1 },{ 10, 0, -1, -1 },{ 11, 0, -1, -1 },{ 12, 0, -1, -1 },{ 13, 0, -1, -1 },{ 14, 0, -1, -1 },{ 15, 0, -1, -1 },{ 16, 0, -1, -1 },{ 17, 0, -1, -1 },{ 18, 0, -1, -1 },{ 19, 0, -1, -1 },{ 20, 0, -1, -1 },{ 21, 0, -1, -1 },{ 22, 0, -1, -1 },{ 23, 0, -1, -1 },{ 24, 0, -1, -1 },{ 25, 0, -1, -1 },{ 26, 0, -1, -1 },{ 27, 0, -1, -1 },{ 28, 0, -1, -1 },{ 29, 0, -1, -1 },{ 30, 0, -1, -1 },{ 31, 0, -1, -1 },{ 32, 0, -1, -1 },{ 33, 0, -1, -1 },{ 34, 0, -1, -1 },{ 35, 0, -1, -1 },{ 36, 0, -1, -1 },{ 37, 0, -1, -1 },{ 38, 0, -1, -1 },{ 39, 0, -1, -1 },{ 40, 0, -1, -1 },{ 41, 0, -1, -1 },{ 42, 0, -1, -1 },{ 43, 0, -1, -1 },{ 44, 0, -1, -1 },{ 45, 0, -1, -1 },{ 46, 0, -1, -1 },{ 47, 0, -1, -1 },{ 48, 0, -1, -1 },{ 49, 0, -1, -1 },{ 50, 0, -1, -1 },{ 51, 0, -1, -1 },{ 52, 0, -1, -1 },{ 53, 0, -1, -1 },{ 54, 0, -1, -1 },{ 55, 0, -1, -1 },{ 56, 0, -1, -1 },{ 57, 0, -1, -1 },{ 58, 0, -1, -1 },{ 59, 0, -1, -1 },{ 60, 0, -1, -1 },{ 61, 0, -1, -1 },{ 62, 0, -1, -1 },{ 63, 0, -1, -1 },{ 64, 0, -1, -1 },{ 65, 0, -1, -1 },{ 66, 0, -1, -1 },{ 67, 0, -1, -1 },{ 68, 0, -1, -1 },{ 69, 0, -1, -1 },{ 70, 0, -1, -1 },{ 71, 0, -1, -1 },{ 72, 0, -1, -1 },{ 73, 0, -1, -1 },{ 74, 0, -1, -1 },{ 75, 0, -1, -1 },{ 76, 0, -1, -1 },{ 77, 0, -1, -1 },{ 78, 0, -1, -1 },{ 79, 0, -1, -1 },{ 80, 0, -1, -1 },{ 81, 0, -1, -1 },{ 82, 0, -1, -1 },{ 83, 0, -1, -1 },{ 84, 0, -1, -1 },{ 85, 0, -1, -1 },{ 86, 0, -1, -1 },{ 87, 0, -1, -1 },{ 88, 0, -1, -1 },{ 89, 0, -1, -1 },{ 90, 0, -1, -1 },{ 91, 0, -1, -1 },{ 92, 0, -1, -1 },{ 93, 0, -1, -1 },{ 94, 0, -1, -1 },{ 95, 0, -1, -1 },{ 96, 0, -1, -1 },{ 97, 0, -1, -1 },{ 98, 0, -1, -1 },{ 99, 0, -1, -1 },{ 100, 0, -1, -1 },{ 101, 0, -1, -1 },{ 102, 0, -1, -1 },{ 103, 0, -1, -1 },{ 104, 0, -1, -1 },{ 105, 0, -1, -1 },{ 106, 0, -1, -1 },{ 107, 0, -1, -1 },{ 108, 0, -1, -1 },{ 109, 0, -1, -1 },{ 110, 0, -1, -1 },{ 111, 0, -1, -1 },{ 112, 0, -1, -1 },{ 113, 0, -1, -1 },{ 114, 0, -1, -1 },{ 115, 0, -1, -1 },{ 116, 0, -1, -1 },{ 117, 0, -1, -1 },{ 118, 0, -1, -1 },{ 119, 0, -1, -1 },{ 120, 0, -1, -1 },{ 121, 0, -1, -1 },{ 122, 0, -1, -1 },{ 123, 0, -1, -1 },{ 124, 0, -1, -1 },{ 125, 0, -1, -1 },{ 126, 0, -1, -1 },{ 127, 0, -1, -1 },{ 128, 0, -1, -1 },{ 129, 0, -1, -1 },{ 130, 0, -1, -1 },{ 131, 0, -1, -1 },{ 132, 0, -1, -1 },{ 133, 0, -1, -1 },{ 134, 0, -1, -1 },{ 135, 0, -1, -1 },{ 136, 0, -1, -1 },{ 137, 0, -1, -1 },{ 138, 0, -1, -1 },{ 139, 0, -1, -1 },{ 140, 0, -1, -1 },{ 141, 0, -1, -1 },{ 142, 0, -1, -1 },{ 143, 0, -1, -1 },{ 144, 0, -1, -1 },{ 145, 0, -1, -1 },{ 146, 0, -1, -1 },{ 147, 0, -1, -1 },{ 148, 0, -1, -1 },{ 149, 0, -1, -1 },{ 150, 0, -1, -1 },{ 151, 0, -1, -1 },{ 152, 0, -1, -1 },{ 153, 0, -1, -1 },{ 154, 0, -1, -1 },{ 155, 0, -1, -1 },{ 156, 0, -1, -1 },{ 157, 0, -1, -1 },{ 158, 0, -1, -1 },{ 159, 0, -1, -1 },{ 160, 0, -1, -1 },{ 161, 0, -1, -1 },{ 162, 0, -1, -1 },{ 163, 0, -1, -1 },{ 164, 0, -1, -1 },{ 165, 0, -1, -1 },{ 166, 0, -1, -1 },{ 167, 0, -1, -1 },{ 168, 0, -1, -1 },{ 169, 0, -1, -1 },{ 170, 0, -1, -1 },{ 171, 0, -1, -1 },{ 172, 0, -1, -1 },{ 173, 0, -1, -1 },{ 174, 0, -1, -1 },{ 175, 0, -1, -1 },{ 176, 0, -1, -1 },{ 177, 0, -1, -1 },{ 178, 0, -1, -1 },{ 179, 0, -1, -1 },{ 180, 0, -1, -1 },{ 181, 0, -1, -1 },{ 182, 0, -1, -1 },{ 183, 0, -1, -1 },{ 184, 0, -1, -1 },{ 185, 0, -1, -1 },{ 186, 0, -1, -1 },{ 187, 0, -1, -1 },{ 188, 0, -1, -1 },{ 189, 0, -1, -1 },{ 190, 0, -1, -1 },{ 191, 0, -1, -1 },{ 192, 0, -1, -1 },{ 193, 0, -1, -1 },{ 194, 0, -1, -1 },{ 195, 0, -1, -1 },{ 196, 0, -1, -1 },{ 197, 0, -1, -1 },{ 198, 0, -1, -1 },{ 199, 0, -1, -1 },{ 200, 0, -1, -1 },{ 201, 0, -1, -1 },{ 202, 0, -1, -1 },{ 203, 0, -1, -1 },{ 204, 0, -1, -1 },{ 205, 0, -1, -1 },{ 206, 0, -1, -1 },{ 207, 0, -1, -1 },{ 208, 0, -1, -1 },{ 209, 0, -1, -1 },{ 210, 0, -1, -1 },{ 211, 0, -1, -1 },{ 212, 0, -1, -1 },{ 213, 0, -1, -1 },{ 214, 0, -1, -1 },{ 215, 0, -1, -1 },{ 216, 0, -1, -1 },{ 217, 0, -1, -1 },{ 218, 0, -1, -1 },{ 219, 0, -1, -1 },{ 220, 0, -1, -1 },{ 221, 0, -1, -1 },{ 222, 0, -1, -1 },{ 223, 0, -1, -1 },{ 224, 0, -1, -1 },{ 225, 0, -1, -1 },{ 226, 0, -1, -1 },{ 227, 0, -1, -1 },{ 228, 0, -1, -1 },{ 229, 0, -1, -1 },{ 230, 0, -1, -1 },{ 231, 0, -1, -1 },{ 232, 0, -1, -1 },{ 233, 0, -1, -1 },{ 234, 0, -1, -1 },{ 235, 0, -1, -1 },{ 236, 0, -1, -1 },{ 237, 0, -1, -1 },{ 238, 0, -1, -1 },{ 239, 0, -1, -1 },{ 240, 0, -1, -1 },{ 241, 0, -1, -1 },{ 242, 0, -1, -1 },{ 243, 0, -1, -1 },{ 244, 0, -1, -1 },{ 245, 0, -1, -1 },{ 246, 0, -1, -1 },{ 247, 0, -1, -1 },{ 248, 0, -1, -1 },{ 249, 0, -1, -1 },{ 250, 0, -1, -1 },{ 251, 0, -1, -1 },{ 252, 0, -1, -1 },{ 253, 0, -1, -1 },{ 254, 0, -1, -1 },{ 255, 0, -1, -1 } };

	//Определяем сколько символов каждого кода содержится в тексте, затем сортируем nodeIds(по количеству символов) от большего к меньшему
	int initFreqTableRes = InitFrequencyTable(textPath, nodeIds, nodes);

	if (initFreqTableRes != 0) return initFreqTableRes;

	//Определяем количество кодов символов в тексте
	int nonNullNodesCount = GetNonNullNodesCount(nodeIds, nodes);

	if (nonNullNodesCount == 0) return 2;

	if (nonNullNodesCount > 1)
	{
		//Создаем дерево Хаффмана по таблице встречаемости, treeId - номер главного узла дерева
		short treeId = CreateTree(nodeIds, nodes, nonNullNodesCount - 1);

		//Инициализируем Bits и Length всех символов
		symbolIndex = 0;
		InitSymbolCodes(nodes, treeId, symbols, strBuffer, 0);
	}
	else
	{
		symbols[0].Code = nodes[nodeIds[0]].Code;
		symbols[0].Bits = "0";
		symbols[0].Frequency = nodes[nodeIds[0]].Count;
		symbols[0].Length = 1;
	}

	*out_symbolsCount = nonNullNodesCount;     //Возвращаем symbolsCount
	*out_textLength = nodes[nodeIds[0]].Count; //Возвращаем textLength

	return 0;
}

unsigned int GetCompressedSize(struct TableSymbol* symbols, int symbolsCount)
{
	unsigned long long size = 0;
	unsigned long long s = 0;

	for (int x = 0; x < symbolsCount; x++)
	{
		size += ((unsigned long long)symbols[x].Length * (unsigned long long)symbols[x].Frequency) / 8;
		s += ((unsigned long long)symbols[x].Length * (unsigned long long)symbols[x].Frequency) % 8;
	}

	return size + (s % 8 != 0 ? s/8 + 1 : s/8);
}