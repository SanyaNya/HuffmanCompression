#ifndef HUFFMAN_COMPRESSION_ASCII_H
#define HUFFMAN_COMPRESSION_ASCII_H

struct TableSymbol
{
	char Code;              //Код символа
	unsigned int Frequency; //Сколько раз встречается в тексте
	char* Bits;             //Строка из 0 и 1, содержащая код символа, полученный алгоритмом Хаффмана
	unsigned int Length;    //Длина кода символа, полученного алгоритмом Хаффмана
};

//Инициализирует массив symbols
//------------------------------------------------------------------------------------------
//Аргументы:
//textPath - путь к файлу
//symbols - буфер, должно быть не менее 256 элементов
//out_symbolsCount - количество кодов символов
//out_textLength - длина текста
//strBuffer - буфер для создания строк с кодом символа, должно быть не менее 65792 элементов
//------------------------------------------------------------------------------------------
//Возвращает:
//0 - нет ошибок
//1 - ошибка открытия файла
//2 - файл пуст
int InitHuffmanCodeTable(const char* textPath, struct TableSymbol* symbols, int* out_symbolsCount, unsigned int* out_textLength, char* strBuffer);

//Возвращает размер файла в байтах, сжатого алгоритмом Хаффмана
unsigned int GetCompressedSize(struct TableSymbol* symbols, int symbolsCount);

#endif