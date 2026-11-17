#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;
//sorrend: Red, Green, Blue, Alpha

unsigned short tom(unsigned char szurke, unsigned char alpha){
	unsigned short tmp=szurke;
	tmp = tmp<<8;
	tmp |= alpha;
	return tmp;
}

unsigned int get_bits(unsigned char* data, unsigned int darab/*hányadik darab blokk*/, unsigned int numBits/*hány bitet olvassunk ki*/) {
	int dd = (darab*numBits)%8;
	data += (darab*numBits)/8;
	//darab = *((int*)data);
	//return darab;
	return ((unsigned short)( ((((unsigned short)data[0]) << 8) | data[1]) << dd) >> (16+dd-numBits));  //little endian
}

void help(){
	cout << "Használat: ./grey_alpha.run [file list] [out]\n";
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
	
	ofstream outputf(argv[2]);
	if (!outputf.is_open()){
		cout << "Nem sikerült megnyitni a megadott output file-t!\n";
		help();
		return 1;
	}
	
	unsigned int i,j;
	ifstream::pos_type size;
	unsigned int file_count = filenevek.size();
	char * mem[file_count];
	unsigned short s;
	unsigned char a;
	for(i=0; i<file_count-1; i++){
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
		
		outputf <<  "const unsigned short " << filenevek[i] << "_kep[" << dec << size/4*2 << "] = {\n";
		for(j=0;j<size-4;j+=4){
			a=char((int(mem[i][j])+int(mem[i][j+1])+int(mem[i][j+2])))/3;
			s=tom(a,mem[i][j+3]);
			outputf << "0x" << hex << s << ",";
		}
		a=char((int(mem[i][j])+int(mem[i][j+1])+int(mem[i][j+2])))/3;
		s=tom(a,mem[i][j+3]);
		outputf << "0x" << hex << s << "\n};\n\n";
	}

	outputf.close();
	return 0;
}
