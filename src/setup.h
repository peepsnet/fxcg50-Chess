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

#define FORGATAS_MOD_STATIKUS 0
#define FORGATAS_MOD_DINAMIKUS 1

#define PX(xx) (18*(xx)-18)
#define PY(yy) (24*(yy))

typedef struct {
	struct{
		byte mod; //FORGATAS_MOD_STATIKUS vagy FORGATAS_MOD_DINAMIKUS
		struct{
			irany feher_terfel; //fehér térfele
			irany feher_babu; //fehér bábuk fejének iránya
			irany fekete_babu; //fekete bábuk fejének iránya	
		} statikus;
		
		struct{
			irany soros_terfel; //soron jövő térfele
			irany soros_babu; //soron jövő bábujának feje
			irany ellen_babu; //a nem soron jövő játékos bábujának fejének iránya
		} dinamikus;

	} forgatas;
	color_t tabla_szinek[2]; //első fehér, második fekete
} t_settings;

t_settings settings = {{FORGATAS_MOD_DINAMIKUS, {le, fel, fel}, {le, fel, fel} }, {COLOR_ORANGE, COLOR_SADDLEBROWN}};

void setup();

void setup(){
	t_settings tset;
	tset.forgatas.mod = settings.forgatas.mod;
	tset.forgatas.statikus.feher_terfel = settings.forgatas.statikus.feher_terfel;
	tset.forgatas.statikus.feher_babu = settings.forgatas.statikus.feher_babu;
	tset.forgatas.statikus.fekete_babu = settings.forgatas.statikus.fekete_babu;
	tset.forgatas.dinamikus.soros_terfel = settings.forgatas.dinamikus.soros_terfel;
	tset.forgatas.dinamikus.soros_babu = settings.forgatas.dinamikus.soros_babu;
	tset.forgatas.dinamikus.ellen_babu = settings.forgatas.dinamikus.ellen_babu;
	
	irany * forg[3];
	
	MsgBoxPush(6);
	PrintXY(9, 2, "xxSETUP", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(3, 3, "xxRotation:", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(12, 3, "xx\xE6\x9A     \xE6\x9B", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(3, 4, "xxBoard:", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	keret(PX(18), PY(4)+1, CELL_WH, CELL_WH);
	keret(PX(18), PY(5)+1, CELL_WH, CELL_WH);
	keret(PX(18), PY(6)+1, CELL_WH, CELL_WH);
	
	int fillforg[4][4] = {
		{PX(18), PY(4)+2+CELL_WH/2, CELL_WH,  CELL_WH/2},//le
		{PX(18)+1+CELL_WH/2, PY(4)+1, CELL_WH/2,  CELL_WH},//jobbra
		{PX(18), PY(4)+1, CELL_WH,  CELL_WH/2},//fel
		{PX(18), PY(4)+1, CELL_WH/2,  CELL_WH}//balra
	};
	
	
	
	
	int key;
	byte selected=0;
	bool megy=1;
	while(megy){
		PrintXY(13, 3, (tset.forgatas.mod?"xxdyn. ":"xxstat."), selected==0, TEXT_COLOR_BLACK);
		if(tset.forgatas.mod==FORGATAS_MOD_STATIKUS){
			forg[0] = &tset.forgatas.statikus.feher_terfel;
			forg[1] = &tset.forgatas.statikus.feher_babu;
			forg[2] = &tset.forgatas.statikus.fekete_babu;
			
			PrintXY(3, 5, "xxWhite pieces:", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			PrintXY(3, 6, "xxBlack pieces:", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);	
		}else{
			forg[0] = &tset.forgatas.dinamikus.soros_terfel;
			forg[1] = &tset.forgatas.dinamikus.soros_babu;
			forg[2] = &tset.forgatas.dinamikus.ellen_babu;
			
			PrintXY(3, 5, "xxCurrent:     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			PrintXY(3, 6, "xxOpponent:    ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		}
		
		fillArea(PX(18), PY(4)+1, CELL_WH, CELL_WH, COLOR_WHITE);
		fillArea(fillforg[*forg[0]][0], fillforg[*forg[0]][1], fillforg[*forg[0]][2], fillforg[*forg[0]][3], COLOR_BLACK);
		
		print_mezo(babuk_kep[FEHER][futo-1], settings.tabla_szinek[1], *forg[1], PX(18), PY(5)+1);
		print_mezo(babuk_kep[FEKETE][futo-1], settings.tabla_szinek[0], *forg[2], PX(18), PY(6)+1);
		
		PrintXY(16, 4, (selected==1)?"xx\xE6\x91":"xx ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		PrintXY(16, 5, (selected==2)?"xx\xE6\x91":"xx ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		PrintXY(16, 6, (selected==3)?"xx\xE6\x91":"xx ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	
		PrintXY(3, 7, "xxDefault", selected==4, TEXT_COLOR_BLACK);
		PrintXY(11, 7, "xxOK", selected==5, TEXT_COLOR_BLACK);
		PrintXY(14, 7, "xxCancel", selected==6, TEXT_COLOR_BLACK);
		
		
		
		GetKey(&key);
		switch(key){
			case KEY_CTRL_DOWN:
				selected=(selected+1)%7;
				break;
			case KEY_CTRL_UP:
				selected=(selected+6)%7;
				break;
			case KEY_CTRL_EXIT:
			case KEY_CTRL_F5:
				megy=0;
				break;
			case KEY_CTRL_LEFT:
			case KEY_CTRL_RIGHT:
				switch(selected){
					case 0:
						tset.forgatas.mod = 1-tset.forgatas.mod;
						break;
					case 1:
					case 2:
					case 3:
						*forg[selected-1] = (*forg[selected-1]+((key==KEY_CTRL_LEFT)?1:3))%4;
						break;
					case 4:
					case 5:
					case 6:
						selected=(selected+((key==KEY_CTRL_LEFT)?6:1))%7;
						break;
				}
				break;
			case KEY_CTRL_EXE:
				switch(selected){
					case 4://default;
						tset.forgatas.mod = FORGATAS_MOD_DINAMIKUS;
						tset.forgatas.statikus.feher_terfel = le;
						tset.forgatas.statikus.feher_babu = fel;
						tset.forgatas.statikus.fekete_babu = fel;
						tset.forgatas.dinamikus.soros_terfel = le;
						tset.forgatas.dinamikus.soros_babu = fel;
						tset.forgatas.dinamikus.ellen_babu = fel;
						break;
					case 5://save
						megy=false;
						settings.forgatas.mod = tset.forgatas.mod;
						settings.forgatas.statikus.feher_terfel = tset.forgatas.statikus.feher_terfel;
						settings.forgatas.statikus.feher_babu = tset.forgatas.statikus.feher_babu;
						settings.forgatas.statikus.fekete_babu = tset.forgatas.statikus.fekete_babu;
						settings.forgatas.dinamikus.soros_terfel = tset.forgatas.dinamikus.soros_terfel;
						settings.forgatas.dinamikus.soros_babu = tset.forgatas.dinamikus.soros_babu;
						settings.forgatas.dinamikus.ellen_babu = tset.forgatas.dinamikus.ellen_babu;
						save_settings();
						break;
					case 6://cancel
						megy=false;
						break;
				}
				break;
		}
	}
	
	MsgBoxPop();
}
