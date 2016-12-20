#pragma once
#include <Windows.h>
#include <vector>


using namespace std;

vector<vector<int>> imageLoad(const char *fname) {
	vector<vector<int>> v(128, vector<int>(3));
	FILE *f = fopen(fname, "rb");
	BITMAPFILEHEADER bmp_header;

	if (f != nullptr) {
		//считываем заголовочный файл
		fread(&bmp_header, sizeof(BITMAPFILEHEADER), 1, f);
		//перемеещаем указатель на начало пиксельного массива
		fseek(f, bmp_header.bfOffBits, SEEK_SET);
		//считываем битмап
		for (int i = 0; i < 128; i++) {
			for (int j = 0; j < 3; j++) {
				fread(&v[i][j], 1, sizeof(unsigned char), f);
			}
		}
		fclose(f);
	}
	return v;
}