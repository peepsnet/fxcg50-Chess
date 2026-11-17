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
using namespace std;

typedef struct{
	unsigned char szurke;
	unsigned char alpha;
	unsigned int count;
} elem;

bool sortf(elem a, elem b){return (a.count>b.count);}

//sorrend: Red, Green, Blue, Alpha

unsigned short tom(unsigned char szurke, unsigned char alpha){
	unsigned short tmp=szurke;
	tmp = tmp<<8;
	tmp |= alpha;
	return tmp;
}

void help(){
	cout << "Használat: ./map.run [input] [output]\n";
}

int main (int argc, char* argv[]){
	ifstream inputf(argv[1], ios::in|ios::binary|ios::ate);
	if (!inputf.is_open()){
		cout << "Nem sikerült megnyitni a megadott input file-t!\n";
		help();
		return 1;
	}
	
	ifstream::pos_type size;
	char * mem;
	size = inputf.tellg();
	mem = new char [size];
	inputf.seekg(0, ios::beg);
    inputf.read(mem, size);
	inputf.close();

	map<unsigned short,unsigned int> szinek;
	

	unsigned short s;
	unsigned int i=0;
	for(i=0;i<size;i+=4){
		//mem[i] >>= 3;
		//mem[i+1] >>= 3;
		//mem[i+2] >>= 3;
		//mem[i+3] >>= 3;
		szinek[tom(char((int(mem[i])+int(mem[i+1])+int(mem[i+2])))/3,mem[i+3])]++;
	}

	
	vector<elem> elemek(szinek.size());
	std::map<unsigned short,unsigned int>::iterator it;
	i=0;
	for(it=szinek.begin(); it!=szinek.end(); ++it){
		elemek[i].alpha = (it->first&255);
		elemek[i].szurke = it->first>>8;
		elemek[i].count = it->second;
		i++;
    }
    
    std::sort(elemek.begin(), elemek.end(), sortf);
    
    cout << "Size:  " << szinek.size() << "\n";
    for(i=0;i<szinek.size();i++){
    	cout << (int)elemek[i].szurke << " " << (int)elemek[i].alpha << " => " << elemek[i].count << '\n';    	
    }
	
	
	return 0;
}
