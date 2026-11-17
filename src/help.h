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

#include "help_text.h"
#include "firstrun_text.h"

static const short empty[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char PrintMiniFixMultiLine(int x, int y, const char *Msg, const int flags, const short color, const short bcolor, int*pi, int max_lines){
	int i, dx/*, x_eredeti, lines=0*/;
	unsigned short width;
	void *p;
	//x_eredeti=x;
	i=*pi;
	unsigned short multi;

	while ( Msg[i] && Msg[i]!='\n'){
		/*if(Msg[i]=='\n'){
			x=x_eredeti;
			y+=18;
			i++;
			lines++;
		}*/
		if(Msg[i] == 0xE5 || Msg[i] == 0xE6 || Msg[i] == 0x7F || Msg[i] == 0xF9){
			multi=Msg[i];
			multi <<= 8;
			i++;
			multi += Msg[i];
			p = GetMiniGlyphPtr( multi, &width );
		}else{
			p = GetMiniGlyphPtr( Msg[i], &width );
		}
		dx = ( 12 - width ) / 2;
		if ( dx > 0 ) {
			PrintMiniGlyph( x, y, (void*)empty, flags, dx, 0, 0, 0, 0, color, bcolor, 0 );
		}else dx = 0;
		PrintMiniGlyph( x+dx, y, p, flags, width, 0, 0, 0, 0, color, bcolor, 0 );
		if ( width+dx < 12 ){
			PrintMiniGlyph( x+width+dx, y, (void*)empty, flags, 12-width-dx, 0, 0, 0, 0, color, bcolor, 0 );
		}
		x += 12;
		i++;
	}
	*pi = i+1;
	return Msg[i];
}

void help(unsigned char mit){
	int sorok_szama;
	char * text;
	switch(mit){
		case 0:
		case 2:
			sorok_szama = help_sorok_szama;
			text = help_text;
			break;
		case 1:
			sorok_szama = firstrun_sorok_szama;
			text = firstrun_text;
			break;
	}
	int x = 40;
	int y = 25;
	int j;
	int i[sorok_szama-6];
	int ii=0;
	int elso_sor = 0;
	i[0]=0;
	MsgBoxPush(6);
	int key=0;
	while(!(key==KEY_CTRL_EXIT && (elso_sor>=sorok_szama-8 || mit==0))){
		if(key==KEY_CTRL_UP && elso_sor>0){
			elso_sor--;
		}
		if(key==KEY_CTRL_DOWN && elso_sor<sorok_szama-8){
			elso_sor++;
		}
		MsgBoxPop();
		MsgBoxPush(6);
		y=25;
		
		ii=i[elso_sor];
		PrintMiniFixMultiLine(x, y, text, 0, COLOR_BLACK, COLOR_WHITE, &ii, 1);
		if(elso_sor<sorok_szama-7){i[elso_sor+1]=ii;}
		y+=18;
		for(j=1; j<=7; j++){
			PrintMiniFixMultiLine(x, y, text, 0, COLOR_BLACK, COLOR_WHITE, &ii, 1);
			y+=18;
		}
		
		
		GetKey(&key);
	}
	MsgBoxPop();
}
