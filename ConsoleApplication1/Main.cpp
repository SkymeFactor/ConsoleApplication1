#include <iostream>
#include <fstream>
#include <iomanip>
//#include <cstdio>
#include "MyBMPLoader.h"
#include "Source.h"
#include <vector>
#include <map>
//#include <Windows.h>
//#include <cmath>
#include <algorithm>
//#include <cstdlib>
//#include <string>

using namespace std;

char name[1000] = "image.bmp";
std::ofstream fout("output.txt");
map <int, int> mapCount; //���-�� ������������� �������
map <int, double> mapFreq; //������� �� �������� ���������� �������� � ������ �������������
vector<pair<int, double>> vecFreq; // ������ ��� ������ - �������������

//���������� ��� ���������� ��� ������-�������
bool cmp(pair <int, double> a, pair <int, double> b) {
	return a.second == b.second ? a.first < b.first : a.second > b.second;
}
//������� ��� ������� ������� c ��������� 
void debugv(vector<int> v) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++) {
			fout.width(3);
			fout << right << j * 16 + i + 1 << ":";
			fout.width(3);
			fout << v[j * 16 + i] << " | ";
		}
		fout << "\n";
	}
	fout << "\n";
}
//������� ��� �������� ������� � ������
char* str(vector<int> v) {
	char* res = (char*)malloc(v.size());
	for (int i = 0; i < v.size(); i++)
		res[i] = char(v[i] + '0');
	res[v.size()] = '\0';
	return res;
}


//������� ��� ��������������� ����;
map <int, vector<int>> mapUni;


//������-����
vector<pair<int, vector<int>>> vecShen; //������ � ������
map<int, vector<int>> mapShen; //������� � ������
void Shen(int up, int down) {
	double sumDown = 0, sumUp = 0;
	for (int i = up; i < down; i++)
		sumDown += vecFreq[i].second;
	int mid = up;
	double dif = sumDown;
	//������� �����, �� ������� ������� �������
	for (int i = up; i < down; i++) {
		sumUp += vecFreq[i].second;
		sumDown -= vecFreq[i].second;
		if (sumUp >= sumDown) {
			if (dif > abs(sumUp - sumDown))
				mid = i + 1;
			else
				mid = i;
			break;
		}
		dif = abs(sumUp - sumDown);
	}
	//������� � ����� ������ ����� �������, ������ - ���� 
	for (int i = up; i < mid; i++)
		vecShen[i].second.push_back(1);
	for (int i = mid; i < down; i++)
		vecShen[i].second.push_back(0);
	//���������� ���������� �� ����� ������, ���� �� ������ > 1
	if (mid - up > 1)
		Shen(up, mid);
	if (down - mid > 1)
		Shen(mid, down);
}
void resShen() {
	for (auto it : vecShen)
		mapShen[it.first] = it.second;
}


//�������
struct point {
	int symbol; // -1, ���� �� ����
	double val;
	bool used;
	int l, r;  // -1, ���� ����
};
vector<point> hf;
//���������� �������
int Huf() {
	hf.resize(vecFreq.size());
	for (int i = 0; i < vecFreq.size(); i++) {
		hf[i].symbol = vecFreq[i].first;
		hf[i].used = 0;
		hf[i].val = vecFreq[i].second;
		hf[i].l = hf[i].r = -1;
	}
	int countNotUsed = vecFreq.size();
	while (true) {
		//���� ������ �������
		int min1 = -1;
		for (int i = 0; i < hf.size(); i++)
			if (!hf[i].used && (min1 == -1 || hf[i].val < hf[min1].val))
				min1 = i;
		hf[min1].used = 1;
		//���� ������ �������
		int min2 = -1;
		for (int i = 0; i < hf.size(); i++)
			if (!hf[i].used && (min2 == -1 || hf[i].val < hf[min2].val))
				min2 = i;
		if (min2 == -1)
			return min1;
		hf[min2].used = 1;
		//��������� ����� ����
		point newPoint;
		newPoint.symbol = -1;
		newPoint.l = min1;
		newPoint.r = min2;
		newPoint.val = hf[min1].val + hf[min2].val;
		newPoint.used = 0;
		hf.push_back(newPoint);
	}
}
vector<pair<vector<int>, int>> vecHuf; //������ � ������
map<int, vector<int>> mapHuf; //������� � ������
vector<int> now;
//������� ������� � ������� � ������
void resHuf(int num) {
	if (hf[num].symbol != -1) {
		vecHuf.push_back(make_pair(now, hf[num].symbol));
		mapHuf[hf[num].symbol] = now;
		return;
	}
	//����������� �� ������ �����
	now.push_back(0);
	resHuf(hf[num].l);
	now.pop_back();

	//����������� �� ������� �����
	now.push_back(1);
	resHuf(hf[num].r);
	now.pop_back();
}


int main() {
	setlocale(LC_ALL, "Russian");
	//SetConsoleOutputCP(1252); // ��������� ������� �������� win-cp 1251 � ����� ������ (���������)
	
	//freopen("output.txt", "w", stdout);

	cout << "Reading the image...........";
	vector<vector<int>> arr = imageLoad(name); // ��������� ������ �������� ������
	vector<int> v(128); // ������ �� �������� �� ��������� (������� ���-�� ������ �������� ������)
	cout << "Done\n\n";
	cout << "Data processing.............";
	//����������� �������� ������� v
	for (int i = 0; i < 128; i++)
		v[i] = arr[i][2] * 0.299 + arr[i][1] * 0.587 + arr[i][0] * 0.114;
	fout << "�������� ���������:\n";
	debugv(v);
	cout << "Done\n\n";
	cout << "Data quantization...........";
	for (int i = 0; i < 128; i++) {
		//�������� �� ������������ �������
		v[i] = (v[i] / 20) * 20;
		//��������� ������� 
		mapFreq[v[i]] += 1.0 / 128;
	}
	fout << "��������� �����������:\n";
	debugv(v);
	cout << "Done\n\n";
	cout << "Data sorting................";
	//����� ������� �������
	fout << "��������� �������:\n";
	for (auto it : mapFreq) 
		fout << setw(3) << it.first << ':' << fixed << it.second << '|';
	fout << "\n\n";

	//�������������� ������� � ������
	fout << "������������� �� ������������� ������� ������������� ���������� ��������:\n";
	for (auto it : mapFreq)
		vecFreq.push_back(make_pair(it.first, it.second));
	sort(vecFreq.begin(), vecFreq.end(), cmp); //��������� �� ���������������� ������� �������������
	for (auto it : vecFreq) 
		fout << setw(3) << it.first << ": " << fixed << it.second << '\n';
	cout << "Done\n\n";
	cout << "Binary code building........";
	//������� ����� ��������� ����
	fout << "\n������� ����� ��������� ����: ";
	double averageLen = 0;
	for (auto it : vecFreq)
		averageLen += -it.second * log2(it.second);
	fout << fixed << averageLen << "\n\n";

	//�������� ����������� �������������� ���
	fout << "�������� ����������� �������������� ���:\n";
	int k = 1;
	int st = 0; //2^st - ������ ����� >= frequency.size() 
	while (k < vecFreq.size())
		k *= 2, st++;
	int kod = 0;
	for (auto it : vecFreq) {
		fout.width(3);
		fout << setw(3) << it.first << ": ";
		//����� ������������ ����
		for (int i = st - 1; i >= 0; i--) {
			int s = bool((1 << i) & kod); //��������� ������ � ����
			fout << s;
			mapUni[it.first].push_back(s); //���������� �������
		}
		fout << '\n';
		kod++;
	}
	cout << "Done\n\n";
	cout << "Shannon code building.......";
	//������-����
	//������� ������ � ����������� ���������
	vecShen.resize(vecFreq.size());
	for (int i = 0; i < vecFreq.size(); i++)
		vecShen[i].first = vecFreq[i].first;
	Shen(0, vecFreq.size()); //������� ������� � ������
	resShen(); //������� ������� � ������
			   //����� ����������
	fout << "\n������-����\n";
	for (int i = 0; i < vecShen.size(); i++) {
		fout << setw(3) << vecShen[i].first <<'('<< fixed << vecFreq[i].second << "): ";
		for (int j = 0; j < vecShen[i].second.size(); j++)
			fout << vecShen[i].second[j];
		fout << '\n';
	}
	cout << "Done\n\n";
	cout << "Huffman code building.......";
	//�������
	int num = Huf(); //���������� �������
					 //����� �������
	fout << "\n������� ��� ���� ��������:\n";
	fout << "num sym   val  l  r\n";
	for (int i = 0; i < hf.size(); i++) {
		fout << setw(3) << i << ' ' << setw(3) << hf[i].symbol << ' ' << fixed << hf[i].val
			 << setw(2) << hf[i].l << setw(2) << hf[i].r << '\n';
	}
	fout << "\n��� ��������:\n";
	resHuf(num); //���������� ������� � ������� 
				 //����� �����
	sort(vecHuf.begin(), vecHuf.end());
	for (auto it : vecHuf) {
		fout << setw(3) << it.second <<'('<< fixed << mapFreq[it.second] <<"): ";
		for (auto it2 : it.first)
			fout << it2;
		fout << "\n";
	}
	cout << "Done\n\n";
	cout << "Other calculations..........";
	//��������� �������������� ������� ������ ���������
	fout << "����������� ����� ��������� ���� ��� ������ �������: " << averageLen << '\n';
	fout << "����� ��������� ��� ������������ ��������� ����: " << averageLen * 128 << '\n';

	fout << "\n����������� ���:\n";
	int countUni = 0;
	for (int i = 0; i < v.size(); i++) {
		if (i % 32 == 0 && i > 0)
			fout << '\n';
		fout << str(mapUni[v[i]]) << ' ';
		countUni += mapUni[v[i]].size();
		
	}
	fout << "\n\n���-�� ��������: " << countUni << "\n������� ����� ����: " << fixed << countUni / 128.0
		 << "\n������������ ����: " << countUni / 128.0 / averageLen << '\n';

	fout << "\n��� ��������:\n";
	int countHuf = 0;
	for (int i = 0; i < v.size(); i++) {
		if (i % 32 == 0 && i > 0)
			fout << '\n';
		fout << str(mapHuf[v[i]]) << ' ';
		countHuf += mapHuf[v[i]].size();
	}
	fout << "\n\n���-�� ��������: "<< countHuf << "\n������� ����� ����: "<< fixed << countHuf / 128.0
		 <<"\n������������ ����: " << countHuf / 128.0 / averageLen << '\n';


	fout << "\n��� �������-����\n";
	int countShen = 0;
	for (int i = 0; i < v.size(); i++) {
		if (i % 32 == 0 && i > 0)
			fout << '\n';
		fout << str(mapShen[v[i]]) << ' ';
		countShen += mapShen[v[i]].size();
	}
	fout << "\n\n���-�� ��������: "<< countShen <<"\n������� ����� ����: "<< fixed << countShen / 128.0
		 <<"\n������������ ����: "<< countShen / 128.0 / averageLen << '\n';
	cout << "Done\n\n\n\n";
	fout.close();
	cout << "The task successfully completed\n\n";

	system("PAUSE>>VOID");
}