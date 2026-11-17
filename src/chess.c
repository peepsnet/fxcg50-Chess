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

#include <fxcg/display.h>
#include <color.h>
#include <display_syscalls.h>
#include <disp_tools.hpp>
#include <keyboard.h>
#include <keyboard.hpp>
#include <string.h>
#include <sprite.h>
#include <stdlib.h>
#include <fxcg.h>

#define VERSION 102

#define tabla_x 0
#define tabla_y 25

#define CELL_WH 21


#define px(x) (tabla_x+1+24*(x))
#define py(y) (tabla_y+1+24*(y))


/* fx-CG50 compatibility patch:
 * - push VRAM to the display explicitly before every GetKey()
 * - replace SaveVRAM_1/LoadVRAM_1 (old Prizm syscalls) with our own buffer */
#define CG50_VRAM_BYTES (384*216*2)
static unsigned char cg50_vram_backup[CG50_VRAM_BYTES];
static void cg50_save_vram(void){ memcpy(cg50_vram_backup, GetVRAMAddress(), CG50_VRAM_BYTES); }
static void cg50_load_vram(void){ memcpy(GetVRAMAddress(), cg50_vram_backup, CG50_VRAM_BYTES); }
#define SaveVRAM_1() cg50_save_vram()
#define LoadVRAM_1() cg50_load_vram()
#define GetKey(k) (Bdisp_PutDisp_DD(), GetKey(k))

#include "bstring/bstrlib.c"
#include "engine.h"
#include "babuk.h"
#include "setup.h"
#include "display.h"
bool playing = true;
bool firstrun = true;
#include "file.h"
#include "help.h"



typedef enum {rm_tabla, rm_kurzor, rm_minden} rajzmodok;
byte kurzor[2]={4,4};
byte kurzor2[2]={4,4};
byte * aktiv_kurzor = kurzor;
void rajzol(rajzmodok mod, byte cp);
void new_game();
bool test_lepes();
void lepes();
void print_message();
irany get_feher_terfel(byte cp);
void forgat(irany i, byte x, byte y, byte * x_uj, byte * y_uj);
bool beero_popup = false;
void billentyus(int key);



int main(void) {
bstring b = bfromcstr ("Hello");
	Bdisp_EnableColor(1);
	EnableStatusArea(0);
	DefineStatusAreaFlags(DSA_SETDEFAULT, 0, 0, 0);
	DefineStatusAreaFlags(3, SAF_BATTERY | SAF_TEXT | SAF_GLYPH | SAF_ALPHA_SHIFT, 0, 0);
	DefineStatusMessage("Chess",1,0,0);

	
	load_settings();
	load_game();
	
	int result;
	GetFKeyPtr( 0x0186, &result );//NEW
	FKey_Display( 3, result );
	GetFKeyPtr( 0x03E4, &result );//SETUP
	FKey_Display( 4, result );
	GetFKeyPtr( 0x03FD, &result );//HELP
	FKey_Display( 5, result );
	
	graf_opt();
	
	rajzol(rm_tabla, jatek.cp);
	print_message();
	SaveVRAM_1();
	
	if(firstrun){
		help(1);
		help(2);
		save_settings();
	}
	
	if(playing){rajzol(rm_kurzor, jatek.cp);}
	int key;
	while (1) {
		GetKey(&key);
		LoadVRAM_1();
		DisplayStatusArea();
		if(playing){
			switch (key){
				case KEY_CTRL_LEFT:
					aktiv_kurzor[0]=(aktiv_kurzor[0]+7)%8;
					rajzol(rm_kurzor, jatek.cp);
					break;
				case KEY_CTRL_RIGHT:
					aktiv_kurzor[0]=(aktiv_kurzor[0]+1)%8;
					rajzol(rm_kurzor, jatek.cp);
					break;
				case KEY_CTRL_UP:
					aktiv_kurzor[1]=(aktiv_kurzor[1]+7)%8;
					rajzol(rm_kurzor, jatek.cp);
					break;
				case KEY_CTRL_DOWN:
					aktiv_kurzor[1]=(aktiv_kurzor[1]+1)%8;
					rajzol(rm_kurzor, jatek.cp);
					break;
				case KEY_CTRL_EXE:
				case KEY_CHAR_STORE:
					if(aktiv_kurzor == kurzor && test_lepes()){
						kurzor2[0] = kurzor[0]; kurzor2[1] = kurzor[1];
						aktiv_kurzor = kurzor2;
						rajzol(rm_kurzor, jatek.cp);
					}else if(aktiv_kurzor == kurzor2){
						lepes();
					}else{
						rajzol(rm_kurzor, jatek.cp);
					}
					break;
				case KEY_CTRL_EXIT:
					aktiv_kurzor = kurzor;
					rajzol(rm_kurzor, jatek.cp);
					break;
				default:
					billentyus(key);
					rajzol(rm_kurzor, jatek.cp);
					break;
			}
		}
		switch (key){
			case KEY_CTRL_F4:
				new_game();
				break;
			case KEY_CTRL_F5:
				setup();
				rajzol(rm_tabla, jatek.cp);
				SaveVRAM_1();
				if(playing){rajzol(rm_kurzor, jatek.cp);}
				break;
			case KEY_CTRL_F1:
			case KEY_CTRL_F6:
				help(0);
				break;
			
		}
		
	}
	return 0;
}

void rajzol(rajzmodok mod, byte cp){
	//rm_tabla
	byte x,y,x_uj,y_uj;
	int kezdoszin=0;
	color_t *mszin = settings.tabla_szinek;
	irany feher_terfel;
	irany babu_irany[2];
	unsigned char betu[3] = "a ";
	int d;
	int px, py;
	unsigned char betu_init[5] = {'h', '1', 'a', '8', '#'};
	int d_init[5] = {-1, 1, 1, -1, 0};
	
	if(settings.forgatas.mod == FORGATAS_MOD_STATIKUS){
		feher_terfel = settings.forgatas.statikus.feher_terfel;
		babu_irany[FEHER] = settings.forgatas.statikus.feher_babu;
		babu_irany[FEKETE] = settings.forgatas.statikus.fekete_babu;
	}else{
		feher_terfel = settings.forgatas.dinamikus.soros_terfel;
		if(cp==FEKETE){
			feher_terfel = invert_irany[feher_terfel];
			babu_irany[FEKETE] = settings.forgatas.dinamikus.soros_babu;
			babu_irany[FEHER] = settings.forgatas.dinamikus.ellen_babu;
		}else{
			babu_irany[FEHER] = settings.forgatas.dinamikus.soros_babu;
			babu_irany[FEKETE] = settings.forgatas.dinamikus.ellen_babu;
		}
	}
	kezdoszin=1;
	
	switch(mod){
		case rm_tabla:
			//cellák A1 (eredetileg bal alsó) mindig sötét, rajta fehér bástya!
			for(x=0;x<8;x++){
			for(y=0;y<8;y++){
				forgat(feher_terfel, x, y, &x_uj, &y_uj);
				if(jatek.tabla[x][y].babu==ures){
					fillArea(tabla_x+1+x_uj*CELL_WH, tabla_y+1+y_uj*CELL_WH, CELL_WH, CELL_WH, mszin[(x+y+kezdoszin)%2]);
				}else{
					print_mezo_opt(jatek.tabla[x][y].szin, jatek.tabla[x][y].babu-1, mszin[(x+y+kezdoszin)%2], babu_irany[jatek.tabla[x][y].szin], tabla_x+1+x_uj*CELL_WH, tabla_y+1+y_uj*CELL_WH);
				}
			}
			}
			//szegély
			fillArea(tabla_x, tabla_y, 1, 8*CELL_WH+1, COLOR_BLACK);
			fillArea(tabla_x, tabla_y+8*CELL_WH+1, 8*CELL_WH+2, 1, COLOR_BLACK);
			fillArea(tabla_x+8*CELL_WH+1, tabla_y, 1, 8*CELL_WH+1, COLOR_BLACK);
			fillArea(tabla_x, tabla_y, 8*CELL_WH+1, 1, COLOR_BLACK);
			//felirat
			//fillArea();
			betu[0] = betu_init[feher_terfel];
			d = d_init[feher_terfel];
			py = tabla_y+8*CELL_WH-19;
			for(x=0;x<8;x++){
				px=0;
				PrintMini(&px, &py, betu, 0x00, 0xFFFFFFFF, 0, 0, COLOR_BLACK, COLOR_WHITE, 0, 0);
				px=tabla_x+CELL_WH*x+(CELL_WH-px)/2+1;
				PrintMini(&px, &py, betu, 0x00, 0xFFFFFFFF, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
				betu[0] += d;
			}
			betu[0] = betu_init[(feher_terfel+1)%4];
			d = d_init[(feher_terfel+1)%4];
			for(y=0;y<8;y++){
				px = tabla_x+CELL_WH*8+5;
				py = tabla_y+y*CELL_WH-CELL_WH+(CELL_WH-17)/2;
				PrintMini(&px, &py, betu, 0x00, 0xFFFFFFFF, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
				betu[0] += d;
			}
			
			//bábuk
			//CopySprite_GA(50, 50, 19, 19, babuk_kep[0][0]);
			
			
			//kurzor szegélyek törlése
			/*fillArea(tabla_x, tabla_y-1, 193, 1, COLOR_WHITE);
			fillArea(tabla_x-1, tabla_y-1, 1, 194, COLOR_WHITE);
			fillArea(px(8), tabla_y-1, 1, 194, COLOR_WHITE);
			plot(px(8), tabla_y, COLOR_BLACK);
			fillArea(tabla_x-1, py(8), 194, 1, COLOR_WHITE);*/
			break;
		case rm_kurzor:
			forgat(feher_terfel, kurzor[0], kurzor[1], &x_uj, &y_uj);
			if(jatek.tabla[x_uj][y_uj].babu==ures){
				fillArea(tabla_x+1+kurzor[0]*CELL_WH, tabla_y+1+kurzor[1]*CELL_WH, CELL_WH, CELL_WH, COLOR_RED);
			}else{
				color_t szin;
				if(aktiv_kurzor == kurzor2){
					szin = COLOR_GREEN;
				}else{
					szin = test_lepes() ? COLOR_GREEN : COLOR_RED;
				}
				print_mezo_opt(jatek.tabla[x_uj][y_uj].szin, jatek.tabla[x_uj][y_uj].babu-1, szin, babu_irany[jatek.tabla[x_uj][y_uj].szin], tabla_x+1+kurzor[0]*CELL_WH, tabla_y+1+kurzor[1]*CELL_WH);
			}
			char ckur[11] = "xx        ";
			ckur[2] = 'a'+x_uj; ckur[3] = '1'+y_uj;
			if(aktiv_kurzor == kurzor2){
				forgat(feher_terfel, kurzor2[0], kurzor2[1], &x_uj, &y_uj);
				ckur[5] = 0xE6; ckur[6] = 0x91;
				ckur[8] = 'a'+x_uj; ckur[9] = '1'+y_uj;
				filledCircle(tabla_x+1+kurzor2[0]*CELL_WH+CELL_WH/2, tabla_y+1+kurzor2[1]*CELL_WH+CELL_WH/2, 5, test_lepes() ? COLOR_GREEN : COLOR_RED);
			}
			PrintXY(13, 3, ckur, TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			break;
		case  rm_minden:
			rajzol(rm_tabla, cp);
			rajzol(rm_kurzor, cp);
			break;
	}
}

void new_game(){
	if(playing){
		MsgBoxPush(6);
		PrintXY(3, 2, "xxAre you sure you", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		PrintXY(3, 3, "xxwant to start a ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		PrintXY(3, 4, "xxnew game?", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		PrintXY_2(TEXT_MODE_NORMAL, 1, 6, 3, TEXT_COLOR_BLACK);
		PrintXY_2(TEXT_MODE_NORMAL, 1, 7, 4, TEXT_COLOR_BLACK);
		int key;
		char vissza=2;
		do{
			GetKey(&key);
			switch(key){
				case KEY_CTRL_F1:
					vissza = 1;
					break;
				case KEY_CTRL_F6:
				case KEY_CTRL_EXIT:
					vissza = 0;
					break;
			}
		}while(vissza==2);
		MsgBoxPop();
		if(vissza==0){return;}
	}
		
	
	jatek_init();
	playing = true;
	save_game();
	aktiv_kurzor=kurzor;
	kurzor[0] = 3; kurzor[1] = 1;
	PrintXY(12, 1, "xx          ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(12, 2, "xx  White   ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	rajzol(rm_tabla, FEHER);
	SaveVRAM_1();
	rajzol(rm_kurzor, FEHER);
}

bool test_lepes(){
	byte x,y,x_uj,y_uj;
	irany feher_terfel = get_feher_terfel(jatek.cp);
	forgat(feher_terfel, kurzor[0], kurzor[1], &x_uj, &y_uj);
		
	if(jatek.tabla[x_uj][y_uj].babu == ures || (jatek.tabla[x_uj][y_uj].babu != ures && jatek.tabla[x_uj][y_uj].szin != jatek.cp)){return false;}	

	t_lepes lepes = {{x_uj, y_uj}, {0,0}};
	
	if(aktiv_kurzor==kurzor){
		for(x=0;x<8;x++){
		for(y=0;y<8;y++){
			forgat(feher_terfel, x, y, &x_uj, &y_uj);
			lepes.hova.oszlop = x_uj;
			lepes.hova.sor = y_uj;
			if(ervenyes_lepes(lepes, jatek.tabla, true)){
				return true;
			}
		}
		}
		return false;
	}else{
		forgat(feher_terfel, kurzor2[0], kurzor2[1], &x_uj, &y_uj);
		lepes.hova.oszlop = x_uj;
		lepes.hova.sor = y_uj;
		return ervenyes_lepes(lepes, jatek.tabla, true);
	}
	
	/*for(x=0;x<8;x++){
	for(y=0;y<8;y++){
		forgat(feher_terfel, x, y, &x_uj, &y_uj);
		lepes.hova.oszlop = x_uj;
		lepes.hova.sor = y_uj;
		if(ervenyes_lepes(lepes, jatek.tabla, true)){
			fillArea(tabla_x+1+x*CELL_WH, tabla_y+1+y*CELL_WH, CELL_WH/3, CELL_WH/3, COLOR_GREEN);
		}
	}
	}
	
	char in[2][4] = {"xxn", "xxi"};
	PrintXY(12, 2, "xxFeher s:", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(21, 2, in[sakkvane(FEHER, jatek.tabla)], TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(12, 3, "xxFekete s:", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(21, 3, in[sakkvane(FEKETE, jatek.tabla)], TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);*/
	
	
	
}

void lepes(){
	if(!test_lepes()){rajzol(rm_kurzor, jatek.cp); return;}
	
	byte x_uj,y_uj;
	irany feher_terfel = get_feher_terfel(jatek.cp);
	
	forgat(feher_terfel, kurzor[0], kurzor[1], &x_uj, &y_uj);
	t_lepes lepes = {{x_uj, y_uj}, {0,0}};
	forgat(feher_terfel,  kurzor2[0],  kurzor2[1], &x_uj, &y_uj);
	lepes.hova.oszlop = x_uj;
	lepes.hova.sor = y_uj;
	
	beero_popup = true;
	lep(lepes, jatek.tabla);
	jatek.cp = 1-jatek.cp;
	beero_popup = false;
	
	print_message();
	aktiv_kurzor = kurzor;

	if(playing){
		rajzol(rm_tabla, jatek.cp);
		SaveVRAM_1();
		rajzol(rm_kurzor, jatek.cp);
	}else{
		jatek.cp = 1-jatek.cp;
		rajzol(rm_tabla, jatek.cp);
		jatek.cp = 1-jatek.cp;
		SaveVRAM_1();
	}
	save_game();
	
	
}

void print_message(){
	char * uzenet;
	char * uzenet2 = ((jatek.cp==FEKETE)?"xx  Black    ":"xx  White    ");
	if(sakkvane(jatek.cp, jatek.tabla)){
		if(!tud_e_lepni(jatek.cp, jatek.tabla)){
			uzenet = "xxCheckmate!";
			uzenet2 = (jatek.cp?"xxWhite wins!":"xxBlack wins!");
			PrintXY(13, 3, "xx        ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			playing = false;
		}else{
			uzenet = "xx  Check!  ";
		}
	}else{
		if(pattvane(jatek.cp, jatek.tabla)){
			playing = false;
			uzenet = "xx  Draw!  ";
			uzenet2 = "xx          ";
			if(playing==false){
				PrintXY(13, 3, "xx        ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			}
		}else{
			uzenet =  "xx           ";
		}
	}
	PrintXY(12, 1, uzenet, TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(12, 2, uzenet2, TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
}

t_babu get_beero(byte cp){
	if(!beero_popup){return paraszt;}
	
	MsgBoxPush(3);
	PrintXY(3, 3, "xxChoose a piece:", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	t_babu lehetosegek[4] = {kiralyno, bastya, lo, futo};
	
	keret(150, 108, 4*CELL_WH, CELL_WH);
	
	byte c=1;
	for(c=2;c<4;c++){
		print_mezo_opt(cp, lehetosegek[c]-1, settings.tabla_szinek[(c+1)%2], fel, 150+c*CELL_WH, 108);
	}
	c=1;
	int key=KEY_CTRL_LEFT;
	do{
		if(key == KEY_CTRL_LEFT || key == KEY_CTRL_RIGHT){
			print_mezo_opt(cp, lehetosegek[c]-1, settings.tabla_szinek[(c+1)%2], fel, 150+c*CELL_WH, 108);
			switch (key){
				case KEY_CTRL_LEFT:	
					c=(c+3)%4;
					break;
				case KEY_CTRL_RIGHT:
					c=(c+1)%4;
					break;
			}
			print_mezo(babuk_kep[cp][lehetosegek[c]-1], COLOR_GREEN, fel, 150+c*CELL_WH, 108);
		}
		GetKey(&key);
	}while(key!=KEY_CTRL_EXE);
	
	MsgBoxPop();
	return lehetosegek[c];
}

void forgat(irany i, byte x, byte y, byte * x_uj, byte * y_uj){
	switch(i){
		case jobbra:
			*x_uj=7-y; *y_uj=7-x;
			break;
		case balra:
			*x_uj=y; *y_uj=x;
			break;
		case fel:
			*x_uj=7-x; *y_uj=y;
			break;
		case le:
		case nirany:
		default:
			*x_uj=x; *y_uj=7-y;
			break;
	}
}

irany get_feher_terfel(byte cp){
	irany feher_terfel = nirany;
	if(settings.forgatas.mod == FORGATAS_MOD_STATIKUS){
		feher_terfel = settings.forgatas.statikus.feher_terfel;
	}else{
		feher_terfel = settings.forgatas.dinamikus.soros_terfel;
		if(cp==FEKETE){
			feher_terfel = invert_irany[feher_terfel];
		}
	}
	return feher_terfel;
}

void billentyus(int key){
	byte pos[2];
	irany feher_terfel = get_feher_terfel(jatek.cp);
	forgat(feher_terfel, aktiv_kurzor[0], aktiv_kurzor[1], pos, pos+1);
	
	
	switch(key){
		case KEY_CTRL_XTT:
		case 32596:
		case KEY_CHAR_A:
			pos[0] = 0;
			break;
		case 149:
		case 181:
		case KEY_CHAR_B:
			pos[0] = 1;
			break;
		case 133:
		case 165:
		case KEY_CHAR_C:
			pos[0] = 2;
			break;
		case 129:
		case 145:
		case KEY_CHAR_D:
			pos[0] = 3;
			break;
		case 130:
		case 146:
		case KEY_CHAR_E:
			pos[0] = 4;
			break;
		case 131:
		case 147:
		case KEY_CHAR_F:
			pos[0] = 5;
			break;
		case 187:
		case 30054:
		case KEY_CHAR_G:
			pos[0] = 6;
			break;
		case 30046:
		case 30026:
		case KEY_CHAR_H:
			pos[0] = 0;
			break;
		case 85:
		case 32593:
		case KEY_CHAR_1:
			pos[1] = 0;
			break;
		case 86:
		case 32576:
		case KEY_CHAR_2:
			pos[1] = 1;
			break;
		case 87:
		case KEY_CHAR_3:
			pos[1] = 2;
			break;
		case 80:
		case KEY_CHAR_4:
			pos[1] = 3;
			break;
		case 30101:
		case 81:
		case KEY_CHAR_5:
			pos[1] = 4;
			break;
		case 82:
		case KEY_CHAR_6:
			pos[1] = 5;
			break;
		case 77:
		case KEY_CHAR_7:
			pos[1] = 6;
			break;
		case 30050:
		case 78:
		case KEY_CHAR_8:
			pos[1] = 7;
			break;

	}
	forgat(feher_terfel, pos[0], pos[1], aktiv_kurzor, aktiv_kurzor+1);
	

}
