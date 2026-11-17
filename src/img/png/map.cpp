/*
	Chess for Casio PRIZM calculators
	Copyright (C) 2013  Balázs Dura-Kovács

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

typedef struct{
	unsigned char szurke;
	unsigned char alpha;
	unsigned int count;
} t_egyutt;

typedef struct{
	unsigned char szurke;
	unsigned int count;
} t_szurke;

typedef struct{
	unsigned char alpha;
	unsigned int count;
} t_alpha;

bool sort_alpha(t_alpha a, t_alpha b){return (a.count>b.count);}
bool sort_szurke(t_szurke a, t_szurke b){return (a.count>b.count);}
bool sort_egyutt(t_egyutt a, t_egyutt b){return (a.count>b.count);}

//sorrend: Red, Green, Blue, Alpha

unsigned short tom(unsigned char szurke, unsigned char alpha){
	unsigned short tmp=szurke;
	tmp = tmp<<8;
	tmp |= alpha;
	return tmp;
}

void help(){
	cout << "Használat: ./map.run [file list]\n";
}

int main (int argc, char* argv[]){
	ifstream listf(argv[1]);
	if (!listf.is_open()){
		cout << "Nem sikerült megnyitni a megadott filelistát tartalmazó file-t!\n";
		help();
		return 1;
	}
	
	vector<string> filenevek;
	string temp;
	while(!listf.eof()){
		listf >> temp;
		filenevek.push_back(temp);
	}
	listf.close();
	
	unsigned int i,j;
	ifstream::pos_type size;
	unsigned int file_count = filenevek.size();
	char * mem[file_count];
	unsigned short s;
	unsigned char a;
	map<unsigned char,unsigned int> szurke[file_count];
	map<unsigned char,unsigned int> alpha[file_count];
	map<unsigned short,unsigned int> egyutt[file_count];
	map<unsigned char,unsigned int> kozos_szurke;
	map<unsigned char,unsigned int> kozos_alpha;
	map<unsigned short,unsigned int> kozos_egyutt;
	for(i=0; i<file_count; i++){
		ifstream inputf(filenevek[i].c_str(), ios::in|ios::binary|ios::ate);
		if (!inputf.is_open()){
			cout << "Nem sikerült megnyitni a következő file-t: " << filenevek[i] << "\n";
			return 1;
		}
		size = inputf.tellg();
		mem[i] = new char [size];
		inputf.seekg(0, ios::beg);
		inputf.read(mem[i], size);
		inputf.close();
		for(j=0;j<size;j+=4){
			mem[i][j] >>= 3;
			mem[i][j+1] >>= 3;
			mem[i][j+2] >>= 3;
			mem[i][j+3] >>= 3;
			a=char((int(mem[i][j])+int(mem[i][j+1])+int(mem[i][j+2])))/3;
			s=tom(a,mem[i][j+3]);
			egyutt[i][s]++;
			szurke[i][a]++;
			alpha[i][mem[i][j+3]]++;
			kozos_egyutt[s]++;
			kozos_szurke[a]++;
			kozos_alpha[mem[i][j+3]]++;
		}
	}
	
	vector<t_alpha> kozos_alpha_r(kozos_alpha.size());
	vector<t_szurke> kozos_szurke_r(kozos_szurke.size());
	vector<t_egyutt> kozos_egyutt_r(kozos_egyutt.size());
	map<unsigned short,unsigned int>::iterator its;
	map<unsigned char,unsigned int>::iterator itc;
	i=0;
	for(itc=kozos_alpha.begin(); itc!=kozos_alpha.end(); ++itc){
		kozos_alpha_r[i].alpha = itc->first;
		kozos_alpha_r[i].count = itc->second;
		i++;
	}
	i=0;
	for(itc=kozos_szurke.begin(); itc!=kozos_szurke.end(); ++itc){
		kozos_szurke_r[i].szurke = itc->first;
		kozos_szurke_r[i].count = itc->second;
		i++;
	}
	i=0;
	for(its=kozos_egyutt.begin(); its!=kozos_egyutt.end(); ++its){
		kozos_egyutt_r[i].alpha = (its->first&255);
		kozos_egyutt_r[i].szurke = its->first>>8;
		kozos_egyutt_r[i].count = its->second;
		i++;
	}
	
	std::sort(kozos_alpha_r.begin(), kozos_alpha_r.end(), sort_alpha);
	std::sort(kozos_szurke_r.begin(), kozos_szurke_r.end(), sort_szurke);
	std::sort(kozos_egyutt_r.begin(), kozos_egyutt_r.end(), sort_egyutt);
	
	cout << "\nKözös szürke:\n\tméret: " << kozos_szurke_r.size() << endl;
	for(i=0;i<kozos_szurke_r.size();i++){
		cout << (int)kozos_szurke_r[i].szurke << " * => " << kozos_szurke_r[i].count << '\n';		
	}
	
	cout << "\n\nKözös alpha:\n\tméret: " << kozos_alpha_r.size() << endl;
	for(i=0;i<kozos_alpha_r.size();i++){
		cout << "* " << (int)kozos_alpha_r[i].alpha << " => " << kozos_alpha_r[i].count << '\n';		
	}
	
	cout << "\n\nKözös együtt:\n\tméret: " << kozos_egyutt_r.size() << endl;
	for(i=0;i<kozos_egyutt_r.size();i++){
		cout << (int)kozos_egyutt_r[i].szurke << " " << (int)kozos_egyutt_r[i].alpha << " => " << kozos_egyutt_r[i].count << '\n';		
	}
	
	
	return 0;
}
