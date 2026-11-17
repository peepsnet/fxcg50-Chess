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

unsigned short babuk_kep_opt[2][2][6][19][19]; //[háttérszín][bábu szín][bábu típus][x][y]

unsigned short opacity(unsigned short old, unsigned short ag){
	byte r, g, b, gray;
	double alpha = (ag & 0xFF);
	alpha = alpha/256.0;
	gray = ag>>8;
	r = (old >> 11) << 3;
	g = ((old >> 5)&0x3F)<<2;
	b = (old & 0x1F)<<3;
	r = (unsigned char)(gray*alpha + r*(1-alpha));
	g = (unsigned char)(gray*alpha + g*(1-alpha));
	b = (unsigned char)(gray*alpha + b*(1-alpha));
	r >>= 3;
	g >>= 2;
	b >>= 3;
	unsigned short re = (r << 11) | (g << 5) | b;
	return re;
}

void print_mezo(unsigned short* data, unsigned short szin, irany allas, int x0, int y0){
	unsigned short uj[21][21];
	byte x, y;
	for(x=0;x<21;x++){
		uj[x][0] = szin;
		uj[x][20] = szin;
		uj[0][x] = szin;
		uj[20][x] = szin;
	}

	switch(allas){
		case balra:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[y+1][x+1] = opacity(szin, data[19*x+18-y]);
				}}
			break;
		case jobbra:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[y+1][x+1] = opacity(szin, data[19*(18-x)+y]);
				}}
			break;
		case le:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[y+1][x+1] = opacity(szin, data[19*(18-y)+18-x]);
				}}
			break;
		case fel:
		case nirany:
		default:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[x+1][y+1] = opacity(szin, data[19*x+y]);
				}}
			break;
	}

	VRAM_CopySprite(uj, x0, y0, 21, 21);
}

void print_mezo_opt(byte babu_szin, byte babu_tipus, unsigned short szin, irany allas, int x0, int y0){
	byte hatterszin;
	if(szin == settings.tabla_szinek[0]){
		hatterszin = 0;
	}else if(szin == settings.tabla_szinek[1]){
		hatterszin = 1;
	}else{
		print_mezo(babuk_kep[babu_szin][babu_tipus], szin, allas, x0, y0);
		return;
	}

	unsigned short uj[21][21];
	byte x, y;
	unsigned short* data = &babuk_kep_opt[hatterszin][babu_szin][babu_tipus];
	
	for(x=0;x<21;x++){
		uj[x][0] = szin;
		uj[x][20] = szin;
		uj[0][x] = szin;
		uj[20][x] = szin;
	}

	switch(allas){
		case balra:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[y+1][x+1] = data[19*x+18-y];
				}}
			break;
		case jobbra:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[y+1][x+1] = data[19*(18-x)+y];
				}}
			break;
		case le:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[y+1][x+1] = data[19*(18-y)+18-x];
				}}
			break;
		case fel:
		case nirany:
		default:
				for(x=0;x<19;x++){
				for(y=0;y<19;y++){
					uj[x+1][y+1] = data[19*x+y];
				}}
			break;
	}

	VRAM_CopySprite(uj, x0, y0, 21, 21);
}



void CopySprite_GA(int x0, int y0, int width, int height, unsigned short* data) {
	int x, y;
	int i=0;
	for(y=y0;y<y0+height;y++){
		for(x=x0;x<x0+width;x++){
			plot(x, y, opacity(Bdisp_GetPoint_VRAM(x,y), data[i]));
			i++;
		}
	}
}

void keret(int x0, int y0, int width, int height){
	fillArea(x0-1, y0-1, width+2, 1, COLOR_BLACK);
	fillArea(x0-1, y0+height, width+2, 1, COLOR_BLACK);
	fillArea(x0-1, y0, 1, height, COLOR_BLACK);
	fillArea(x0+width, y0, 1, height, COLOR_BLACK);
}


void graf_opt(){
	byte hatterszin, babu_szin, babu_tipus, x, y;
	unsigned short szin;
	
	for(hatterszin=0; hatterszin<=1; hatterszin++){
		szin = settings.tabla_szinek[hatterszin];
		for(babu_szin=0; babu_szin<=1; babu_szin++){
		for(babu_tipus=0; babu_tipus<=5; babu_tipus++){
			/*for(x=0;x<21;x++){
				babuk_kep_opt[hatterszin][babu_szin][babu_tipus][x][0] = szin;
				babuk_kep_opt[hatterszin][babu_szin][babu_tipus][x][20] = szin;
				babuk_kep_opt[hatterszin][babu_szin][babu_tipus][0][x] = szin;
				babuk_kep_opt[hatterszin][babu_szin][babu_tipus][20][x] = szin;
			}*/
			for(x=0;x<19;x++){
			for(y=0;y<19;y++){
				babuk_kep_opt[hatterszin][babu_szin][babu_tipus][x][y] = opacity(szin, babuk_kep[babu_szin][babu_tipus][19*x+y]);
			}}
		}
		}
	}
}
