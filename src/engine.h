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

typedef unsigned char bool;
typedef unsigned char byte;
#define true 1
#define false 0

#define FEHER 0
#define FEKETE 1

#define sgn(x) ((x > 0) - (x < 0))

typedef enum {fel, balra, le, jobbra, nirany} irany;
irany invert_irany[5] = {le, jobbra, fel, balra, nirany};

typedef enum {ures, paraszt, futo, lo, bastya, kiraly, kiralyno} t_babu;

typedef struct{
	byte szin; //FEHER / FEKETE
	t_babu babu;
	byte flags;	//0x01:	a mezőn álló paraszt leüthető-e en passant-tal
				//0x02:	a mezőn álló király/bástya lépett-e már: sáncolás
} mezo;

#define ENPASSANT 0x01
#define LEPETTE 0x02

typedef struct{
	struct{
		byte oszlop;
		byte sor;
	} honnan;
	
	struct{
		byte oszlop;
		byte sor;
	} hova;
	
} t_lepes;

struct {
	mezo tabla[8][8];//[oszlop][sor] pl.: a1 -> [0][0]
	byte cp; //current player FEHER/FEKETE
	unsigned short lepesID;
	unsigned short loglen;
	char * log;
} jatek;





void jatek_init();
bool ervenyes_lepes(t_lepes lepes, mezo tabla[8][8], bool sakk_ellenorzes);
bool ervenyes_lepes_paraszt(t_lepes lepes, mezo tabla[8][8]);
bool ervenyes_lepes_futo(t_lepes lepes, mezo tabla[8][8]);
bool ervenyes_lepes_lo(t_lepes lepes, mezo tabla[8][8]);
bool ervenyes_lepes_bastya(t_lepes lepes, mezo tabla[8][8]);
bool ervenyes_lepes_kiraly(t_lepes lepes, mezo tabla[8][8]);
bool sakkvane(byte kinek, mezo tabla[8][8]);//FEHER/FEKETE
bool tud_e_lepni(byte kinek, mezo tabla[8][8]);//FEHER/FEKETE
bool pattvane(byte cp, mezo tabla[8][8]);
bool getflag(mezo m, byte mask);
void setflag(mezo * m, byte mask, bool value);
void copy_tabla(mezo dest[8][8], mezo source[8][8]);
void lep(t_lepes lepes, mezo tabla[8][8]);
t_babu get_beero(byte cp);

bool ervenyes_lepes(t_lepes lepes, mezo tabla[8][8], bool sakk_ellenorzes){
	bool vissza=false;
	mezo honnan = tabla[lepes.honnan.oszlop][lepes.honnan.sor];
	mezo hova = tabla[lepes.hova.oszlop][lepes.hova.sor];
	byte player = honnan.szin;
	
	if( !(0<=lepes.hova.sor && lepes.hova.sor<=7 && 0<=lepes.hova.oszlop && lepes.hova.oszlop<=7) ){//táblán rajtavagyunk
		return false;
	}
	
	//saját bábut nem ütünk
	if(hova.babu != ures && hova.szin == honnan.szin){
		return false;
	}


	switch(honnan.babu){
		case ures:
			vissza=false;
			break;
		case paraszt:
			vissza = ervenyes_lepes_paraszt(lepes, tabla);
			break;
		case futo:
			vissza = ervenyes_lepes_futo(lepes, tabla);
			break;
		case lo:
			vissza = ervenyes_lepes_lo(lepes, tabla);
			break;
		case bastya:
			vissza = ervenyes_lepes_bastya(lepes, tabla);
			break;
		case kiralyno:
			vissza = ervenyes_lepes_bastya(lepes, tabla) || ervenyes_lepes_futo(lepes, tabla);
			break;
		case kiraly:
			vissza = ervenyes_lepes_kiraly(lepes, tabla);
			break;
		default:
			return false;
			break;
	}
	
	
	if(vissza && sakk_ellenorzes){
		mezo pre_tabla[8][8];
		copy_tabla(pre_tabla, tabla);
		lep(lepes, pre_tabla);
		vissza = !sakkvane(player, pre_tabla);
		
	}
	
	return vissza;
	
}

bool ervenyes_lepes_paraszt(t_lepes lepes, mezo tabla[8][8]){
	mezo honnan = tabla[lepes.honnan.oszlop][lepes.honnan.sor];
	mezo hova = tabla[lepes.hova.oszlop][lepes.hova.sor];
	byte player = honnan.szin;
	
	//jó irány garantálása
	signed char irany = player*(-2)+1;
	signed char dsor = (lepes.hova.sor-lepes.honnan.sor)*irany;
	if(dsor<0 || dsor>2){
		return false;
	}
	if(dsor==2 && !((irany==1 && lepes.honnan.sor==1) || (irany==-1 && lepes.honnan.sor==6))){
		return false;
	}

//lehetőségek:
	signed char dosz=lepes.honnan.oszlop-lepes.hova.oszlop;
	//előre
	if(dosz==0){
		//előre nem ütünk
		if(tabla[lepes.honnan.oszlop][lepes.honnan.sor+irany].babu != ures){
			return false;
		}
		if(hova.babu != ures){
			return false;
		}
	}
	//ütés
	else if(dsor==1 && (dosz==1 || dosz==-1)){
		if( !((hova.babu != ures ||  getflag(hova, ENPASSANT)) && hova.szin==!player) ){
			return false;
		}
	}else{
		return false;
	}
	
	return true;
}

bool ervenyes_lepes_futo(t_lepes lepes, mezo tabla[8][8]){
	signed char dsor = lepes.hova.sor-lepes.honnan.sor; //+: fel; -: le
	signed char dosz = lepes.hova.oszlop-lepes.honnan.oszlop; //+: jobbra; -: balra
	
	//átlón halad-e
	if(abs(dsor)!=abs(dosz)){
		return false;
	}

	//közbülső mezők üresek legyenek
	signed char xp = (dosz>0)? 1 : -1;
	signed char yp = (dsor>0)? 1 : -1;
	byte x, y;
	y=lepes.honnan.sor+yp;
	for(x=lepes.honnan.oszlop+xp; x*xp<lepes.hova.oszlop*xp; x+=xp){
		if(tabla[x][y].babu != ures){
			return false;
		}
		y+=yp;
	}
	
	return true;
}

bool ervenyes_lepes_bastya(t_lepes lepes, mezo tabla[8][8]){
	signed char dsor = lepes.hova.sor-lepes.honnan.sor; //+: fel; -: le
	signed char dosz = lepes.hova.oszlop-lepes.honnan.oszlop; //+: jobbra; -: balra
	
	//egy sorban/oszlopban halad-e
	if(dsor!=0 && dosz!=0){
		return false;
	}

	//közbülső mezők üresek legyenek
	signed char xp = sgn(dosz);//signum
	signed char yp = sgn(dsor);
	byte x, y;
	x=lepes.honnan.oszlop+xp;
	y=lepes.honnan.sor+yp;

	while(x*xp<lepes.hova.oszlop*xp || y*yp<lepes.hova.sor*yp){
		if(tabla[x][y].babu != ures){
			return false;
		}
		x+=xp;
		y+=yp;
	}
	return true;
}

bool ervenyes_lepes_lo(t_lepes lepes, mezo tabla[8][8]){
	signed char dsor = abs(lepes.hova.sor-lepes.honnan.sor); //+: fel; -: le
	signed char dosz = abs(lepes.hova.oszlop-lepes.honnan.oszlop); //+: jobbra; -: balra
	
	if((dsor==2 && dosz==1) || (dsor==1 && dosz==2)){
		return true;
	}
	return false;
}

bool ervenyes_lepes_kiraly(t_lepes lepes, mezo tabla[8][8]){
	signed char dsor = abs(lepes.hova.sor-lepes.honnan.sor); //+: fel; -: le
	signed char dosz = lepes.hova.oszlop-lepes.honnan.oszlop; //+: jobbra; -: balra
	signed char xp = (dosz>0)? 1 : -1;
	dosz = abs(dosz);	
	
	mezo honnan = tabla[lepes.honnan.oszlop][lepes.honnan.sor];
	byte player = honnan.szin;
	
	if(dsor>1 || dosz>1){
		//sánc
		if(dsor!=0 || dosz!=2){return false;}//király 2-őt lép
		if(lepes.hova.sor != player*7){return false;}//király a saját sorában van
		byte bastya_oszlop = (7*xp+7)/2;//-1 -> 0  1 -> 7
		if(tabla[bastya_oszlop][lepes.honnan.sor].babu != bastya){return false;}//van bástya
		if(getflag(tabla[bastya_oszlop][lepes.honnan.sor], LEPETTE)){return false;}//bástya nem lépett
		if(getflag(honnan, LEPETTE)){return false;}//király nem lépett
		byte x;
		for(x=lepes.honnan.oszlop+xp; x!=bastya_oszlop; x+=xp){//király és bástya közt üres
			if(tabla[x][lepes.honnan.sor].babu != ures){return false;}
		}
		
		if(sakkvane(player, tabla)){return false;}//nincs sakk
		mezo pre_tabla[8][8];
		copy_tabla(pre_tabla, tabla);
		pre_tabla[lepes.honnan.oszlop][lepes.honnan.sor].babu = ures;
		pre_tabla[lepes.honnan.oszlop+xp][lepes.honnan.sor].babu = kiraly;
		pre_tabla[lepes.honnan.oszlop+xp][lepes.honnan.sor].szin = player;
		if(sakkvane(player, pre_tabla)){return false;}//nincs sakk az átgyaloglásban
		
		return true;
	}
	
	return true;
}

bool sakkvane(byte kinek, mezo tabla[8][8]){
	//az esetlegesen támadott király megkeresése
	byte x=8, y=8;
	byte kiraly_x=8, kiraly_y=8;
	for(x=0;x<8;x++){
	for(y=0;y<8;y++){
		if(tabla[x][y].babu==kiraly && tabla[x][y].szin==kinek){
			kiraly_x = x;
			kiraly_y = y;
			x=8; y=8;//kilépés a rutinból
		}
	}
	}
	if(kiraly_x>=8 || kiraly_y>=8){return false;}//nincs király
	
	t_lepes lepes = {{0,0},{kiraly_x, kiraly_y}};
	for(x=0;x<8;x++){
	for(y=0;y<8;y++){
		if(tabla[x][y].szin==1-kinek && tabla[x][y].babu!=ures){
			lepes.honnan.oszlop = x;
			lepes.honnan.sor = y;
			if(ervenyes_lepes(lepes, tabla, false)){
				return true;
			}
		}
	}
	}
	return false;
}

bool tud_e_lepni(byte kinek, mezo tabla[8][8]){
	byte x1, y1, x2, y2;
	t_lepes lepes;
	for(x1=0;x1<8;x1++){
	for(y1=0;y1<8;y1++){
		if(tabla[x1][y1].babu!=ures && tabla[x1][y1].szin == kinek){
			lepes.honnan.oszlop = x1;
			lepes.honnan.sor = y1;
			for(x2=0;x2<8;x2++){
			for(y2=0;y2<8;y2++){
				lepes.hova.oszlop = x2;
				lepes.hova.sor = y2;
				if(ervenyes_lepes(lepes, tabla, true)){return true;}
			}
			}
		}
	}
	}
	return false;
}

bool pattvane(byte cp, mezo tabla[8][8]){
	if(sakkvane(cp, tabla)){return false;}
	if(!tud_e_lepni(cp, tabla)){return true;}

	byte x, y;
	byte ell = 1-cp;
	byte babu_count[2][7];
	// = {0, 0, 0, 0, 0, 0, 0};//melyik bábuból hány darabja van a játékosnak
	for(x=0;x<8;x++){babu_count[0][x]=0; babu_count[1][x]=0;}
	
	for(x=0;x<8;x++){
	for(y=0;y<8;y++){
		babu_count[tabla[x][y].szin][tabla[x][y].babu]++;
	}
	}
	
	if(babu_count[cp][bastya]>0){return false;}
	if(babu_count[cp][kiralyno]>0){return false;}
	if(babu_count[cp][paraszt]>0){return false;}
	if(babu_count[cp][futo]>1){return false;}
	if(babu_count[cp][lo]>2){return false;}
	if(babu_count[cp][futo]>0 && babu_count[cp][lo]>0){return false;}
	if(babu_count[cp][lo]==2){
		if(babu_count[ell][paraszt]>0 || babu_count[ell][lo]>0 || babu_count[ell][futo]>0 || babu_count[ell][bastya]>0 || babu_count[ell][kiralyno]>0){return false;}//ellenfélnek van bábuja
	}
	
	
	if(babu_count[ell][bastya]>0){return false;}
	if(babu_count[ell][kiralyno]>0){return false;}
	if(babu_count[ell][paraszt]>0){return false;}
	if(babu_count[ell][futo]>1){return false;}
	if(babu_count[ell][lo]>2){return false;}
	if(babu_count[ell][futo]>0 && babu_count[ell][lo]>0){return false;}
	if(babu_count[ell][lo]==2){
		if(babu_count[cp][paraszt]>0 || babu_count[cp][lo]>0 || babu_count[cp][futo]>0 || babu_count[cp][bastya]>0 || babu_count[cp][kiralyno]>0){return false;}//ellenfélnek van bábuja
	}
	
	return true;
}

void lep(t_lepes lepes, mezo tabla[8][8]){
	mezo* honnan = &(tabla[lepes.honnan.oszlop][lepes.honnan.sor]);
	mezo* hova = &(tabla[lepes.hova.oszlop][lepes.hova.sor]);
	
	//ENPASSANT ütés
	if(getflag(*hova, ENPASSANT) && honnan->babu == paraszt){
		tabla[lepes.hova.oszlop][(lepes.hova.sor+7)/3].babu = ures;//2->3 5->4
	}
	
	//ENPASSANT lehetőség 0-sa
	byte x;
	for(x=0;x<8;x++){
		setflag(&tabla[x][2], ENPASSANT, false);
		setflag(&tabla[x][5], ENPASSANT, false);
	}
	
	//ENPASSANT lehetőség
	if(honnan->babu == paraszt && abs(lepes.honnan.sor-lepes.hova.sor)==2){
		mezo * en = &tabla[lepes.honnan.oszlop][(6*lepes.honnan.sor+14)/10];//1->2 6->5
		setflag(en, ENPASSANT, true);
		en->szin = honnan->szin;
	}
	
	if(honnan->babu==kiraly && abs(lepes.honnan.oszlop-lepes.hova.oszlop)==2){//sánc
		mezo* hovaB = &tabla[(lepes.honnan.oszlop+lepes.hova.oszlop)/2][lepes.honnan.sor];
		hovaB->babu = bastya;
		hovaB->szin = honnan->szin;
		setflag(hovaB, LEPETTE, true);
		tabla[(lepes.hova.oszlop>lepes.honnan.oszlop)?7:0][lepes.honnan.sor].babu = ures;
	}
	
	
	hova->babu = honnan->babu;
	hova->szin = honnan->szin;
	honnan->babu = ures;
	
	if(hova->babu == paraszt && lepes.hova.sor==(1-hova->szin)*7){//paraszt beérés
		hova->babu = get_beero(hova->szin);
	}
	
	setflag(hova, LEPETTE, true);
	
}


bool getflag(mezo m, byte mask){
	return (m.flags & mask)? true : false;
}

void setflag(mezo * m, byte mask, bool value){
	if(value){
		m->flags |= mask;
	}else{
		m->flags &= ~mask;
	}
}

void copy_tabla(mezo dest[8][8], mezo source[8][8]){
	byte x, y;
	for(x=0; x<8; x++){
	for(y=0; y<8; y++){
		dest[x][y] = source[x][y];
	}
	}
}

void jatek_init(){
	jatek.cp = FEHER;
	unsigned char o, s;
	t_babu sor_init[8] = {bastya, lo, futo, kiralyno, kiraly, futo, lo, bastya};
	for(o=0;o<8;o++){
		jatek.tabla[o][0].szin = FEHER;
		jatek.tabla[o][0].babu = sor_init[o];
		jatek.tabla[o][0].flags = 0;
		jatek.tabla[o][1].szin = FEHER;
		jatek.tabla[o][1].babu = paraszt;
		jatek.tabla[o][1].flags = 0;
		jatek.tabla[o][7].szin = FEKETE;
		jatek.tabla[o][7].babu = sor_init[o];
		jatek.tabla[o][7].flags = 0;
		jatek.tabla[o][6].szin = FEKETE;
		jatek.tabla[o][6].babu = paraszt;
		jatek.tabla[o][6].flags = 0;
		for(s=2;s<=5;s++){
			jatek.tabla[o][s].babu = ures;
			jatek.tabla[o][s].flags = 0;
		}
	}
}
