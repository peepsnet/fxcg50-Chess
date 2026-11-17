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

#include "hash.h"

unsigned char dir[] = "@CHESS";
unsigned char file_settings[] = "SETTINGS";
unsigned char file_game[] = "GAME";
unsigned char fejlec_hossz = 4+2+1; //hash + fullsize + version

static unsigned int lastrandom=31415926;
unsigned char code_random(unsigned int seed){
	if (seed) lastrandom=seed;
	lastrandom = ( 0x41C64E6D*lastrandom ) + 0x3039;
	return ( (unsigned char)(lastrandom >> 16) );
}

void encode(unsigned char * buffer, unsigned short len){
	unsigned short i;
	unsigned int key2;
	memcpy(&key2, buffer, 4);
	code_random(key2);
	for(i=4; i<len; i++){
		buffer[i] ^= code_random(0);
	}
}

void save_settings(){
	int size;
	unsigned short szukseges_size=7+fejlec_hossz;
	unsigned char buffer[szukseges_size];
	unsigned char *p = buffer+4;
	memcpy(p, &szukseges_size, 2); p+=2;
	*p = VERSION; p++;
	
	*p = settings.forgatas.mod; p++;
	*p = settings.forgatas.statikus.feher_terfel; p++;
	*p = settings.forgatas.statikus.feher_babu; p++;
	*p = settings.forgatas.statikus.fekete_babu; p++;
	*p = settings.forgatas.dinamikus.soros_terfel; p++;
	*p = settings.forgatas.dinamikus.soros_babu; p++;
	*p = settings.forgatas.dinamikus.ellen_babu; p++;
	
	hash_u32 hash = SuperFastHash(buffer+4, (int)szukseges_size-4);
	memcpy(buffer, &hash, sizeof(hash_u32));
	encode(buffer, szukseges_size);

	if(MCSGetDlen2(dir, file_settings, &size) != 0){
		//még nem létezik, létrehozzuk
		MCSPutVar2(dir, file_settings, szukseges_size, (void*)buffer);
	}else{
		if(size<szukseges_size || size>szukseges_size+4){
			//kisebb, mint a szükséges, vagy jóval nagyobb; törlés, majd újra létrehozás
			MCSDelVar2(dir, file_settings);
			MCSPutVar2(dir, file_settings, szukseges_size, (void*)buffer);
		}else{
			MCSOvwDat2(dir, file_settings,szukseges_size, (void*)buffer, 0); 
		}
	}
}

void load_settings(){
	int size;
	unsigned short szukseges_size=7+fejlec_hossz;
	unsigned char buffer[szukseges_size];

	if(MCSGetDlen2(dir, file_settings, &size) == 0){
		if(size>=szukseges_size){
			MCSGetData1(0, szukseges_size, (void*)buffer);
			encode(buffer, szukseges_size);
			
			hash_u32 hash = SuperFastHash(buffer+4, (int)szukseges_size-sizeof(hash_u32));
			hash_u32 hash2;
			memcpy(&hash2, buffer, sizeof(hash_u32));
			if(hash==hash2){
				settings.forgatas.mod = buffer[7];
				settings.forgatas.statikus.feher_terfel = buffer[8];
				settings.forgatas.statikus.feher_babu = buffer[9];
				settings.forgatas.statikus.fekete_babu = buffer[10];
				settings.forgatas.dinamikus.soros_terfel = buffer[11];
				settings.forgatas.dinamikus.soros_babu = buffer[12];
				settings.forgatas.dinamikus.ellen_babu = buffer[13];
				firstrun = false;
			}
		}
	}
}

void save_game(){
	int size;
	unsigned short szukseges_size=64+1+fejlec_hossz;
	unsigned char buffer[szukseges_size];
	unsigned char *p = buffer+4;
	memcpy(p, &szukseges_size, 2); p+=2;
	*p = VERSION; p++;
	
	byte x, y;
	
	for(x=0;x<8;x++){
	for(y=0;y<8;y++){
		*p = jatek.tabla[x][y].babu;//3bit
		*p <<= 1; *p |= jatek.tabla[x][y].szin & 1;//1bit
		*p <<= 4; *p |= jatek.tabla[x][y].flags & 15;//2bit, de legyen inkább 4
		*p++;
	}
	}
	*p = jatek.cp;
	*p <<=1; *p |= playing & 1;
	*p++;
	
	
	hash_u32 hash = SuperFastHash(buffer+4, (int)szukseges_size-sizeof(hash_u32));
	memcpy(buffer, &hash, sizeof(hash_u32));
	encode(buffer, szukseges_size);

	if(MCSGetDlen2(dir, file_settings, &size) != 0){
		//még nem létezik, létrehozzuk
		MCSPutVar2(dir, file_settings, szukseges_size, (void*)buffer);
	}else{
		if(size<szukseges_size || size>szukseges_size+4){
			//kisebb, mint a szükséges, vagy jóval nagyobb; törlés, majd újra létrehozás
			MCSDelVar2(dir, file_game);
			MCSPutVar2(dir, file_game, szukseges_size, (void*)buffer);
		}else{
			MCSOvwDat2(dir, file_game, szukseges_size, (void*)buffer, 0); 
		}
	}
}

void load_game(){
	int size;
	unsigned char fejlec[fejlec_hossz];

	if(MCSGetDlen2(dir, file_game, &size) == 0){
		if(size>=fejlec_hossz){
			unsigned char buffer[size];
			MCSGetData1(0, size, (void*)buffer);
			encode(buffer, size);
			unsigned short msize;
			memcpy(&msize, buffer+4, 2);
			if(buffer[6]==VERSION && size>=msize){
				hash_u32 hash = SuperFastHash(buffer+4, (int)msize-4);
				hash_u32 hash2;
				memcpy(&hash2, buffer, 4);
				if(hash==hash2){
					unsigned char *p = buffer+fejlec_hossz;
					byte x, y;
	
					for(x=0;x<8;x++){
					for(y=0;y<8;y++){
						jatek.tabla[x][y].flags = *p & 15; *p >>= 4; 
						jatek.tabla[x][y].szin = *p & 1; *p >>= 1; 
						jatek.tabla[x][y].babu = *p & 7;
						*p++;
					}
					}
					playing = *p & 1; *p >>=1;
					jatek.cp = *p & 1;
					firstrun = false;
				}else{
					jatek_init();
				}
				
			}else{
				jatek_init();
			}
		}else{
			jatek_init();
		}
	}else{
		jatek_init();
	}
}
