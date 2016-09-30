
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#include "sim_encrypt.h"

#define SIMKEY	"4cfa29e3061d8b75"

#define SIM_STR_S	0x21
#define SIM_STR_E	0x7e

#define SIM_BUFSIZE		0x100000

typedef struct {
	//char str_span, key_size;
	//char str_s, str_e;
	char enkey[16];
	char dekey[16];
	u_char enkey_map[256];
	u_char dekey_map[256];
	u_char str_enkey_map[256];
	u_char str_dekey_map[256];
}simkey_st;

static simkey_st default_simkey;

int char_compare(const void *arg1, const void *arg2)
{
	return (*(char *)arg1 - *(char *)arg2);
}

simkey_st *init_simkey(simkey_st *simkey, char *key)

{

	int i, count;

	char hi, lo;
	char str_s, str_e;
	//simkey_st *simkey;

	u_char strkey_map[128];



	if((key==NULL) || (strlen(key)<16)){
		key = SIMKEY;
	}

	if(simkey == NULL){
		simkey = xMalloc(sizeof(simkey_st));
	}

	
	str_s = SIM_STR_S;
	str_e = SIM_STR_E;

	
	//simkey->str_s = str_s;
	//simkey->str_e = str_e;

	for(i=0; i<16; i++){
		simkey->enkey[i] = ascii2num(key[i]);
		simkey->dekey[ascii2num(key[i])] = i;
	}
	/*
	for(i=0; i<16; i++){

		printf("enkey[%#.2x] : %#.2x , dekey[%#.2x] : %#.2x \n", i, simkey->enkey[i], i, simkey->dekey[i]);

	}
	//*/

	for(i=0; i<256; i++){
		#if 0
		//hi = i >> 4;
		//lo = i & 0xf;
		
		simkey->enkey_map[i] = (simkey->enkey[hi]<<4) | simkey->enkey[lo];
		simkey->dekey_map[i] = (simkey->dekey[hi]<<4) | simkey->dekey[lo];
		#else
		
		hi = (i >> 6)|(i<<2)&0xf;
		lo = (i >> 2) & 0xf;
		simkey->enkey_map[i] = (simkey->enkey[hi]<<4) | simkey->enkey[lo];
		
		hi = simkey->dekey[i >> 4];
		lo = simkey->dekey[i & 0xf];
		simkey->dekey_map[i] = ((hi<<6)&0xc0) | ((lo << 2)&0x3c) | ((hi>>2)&0x3);
		#endif
	}


	for(i=str_s; i<=str_e; i++){
		strkey_map[i] = simkey->enkey_map[i];
	}


	/*
	printf("---------------------strkey_map before sort-----------------\n");
	for(i=0; i<128; i++){
		if((i%16) == 0){
			printf("\n");		
		}

		printf("%#.2x ", strkey_map[i]);

	}
	printf("\n=======================================================\n");
	//*/


	count = str_e - str_s + 1;

	qsort(&strkey_map[str_s], count, 1, char_compare);


	/*
	printf("---------------------strkey_map after sort-----------------\n");
	for(i=0; i<128; i++){
		if((i%16) == 0){
			printf("\n");		
		}

		printf("%#.2x ", strkey_map[i]);

	}
	printf("\n=======================================================\n");
	//*/

	
	for(i=0; i<str_s; i++){
		simkey->str_enkey_map[i] = i;
		simkey->str_dekey_map[i] = i;
	}
	
	for(i=str_e+1; i<256; i++){
		simkey->str_enkey_map[i] = i;
		simkey->str_dekey_map[i] = i;
	}
	
	for(i=str_s; i<=str_e; i++){
		simkey->str_enkey_map[simkey->dekey_map[strkey_map[i]]] = i;
		simkey->str_dekey_map[i] = simkey->dekey_map[strkey_map[i]];

	}
	

	/*
	printf("---------------------simkey->enkey_map -----------------\n");
	for(i=0; i<256; i++){
		if((i%16) == 0){
			printf("\n");		
		}

		printf("%#.2x ", simkey->enkey_map[i]);

	}
	printf("\n=======================================================\n");
	//*/
	/*
	printf("---------------------simkey->dekey_map -----------------\n");
	for(i=0; i<256; i++){
		if((i%16) == 0){
			printf("\n");		
		}

		printf("%#.2x ", simkey->dekey_map[i]);

	}
	printf("\n=======================================================\n");
	//*/
	/*
	printf("---------------------simkey->str_enkey_map -----------------\n");
	for(i=0; i<256; i++){
		if((i%16) == 0){
			printf("\n");		
		}

		printf("%#.2x ", simkey->str_enkey_map[i]);

	}
	printf("\n=======================================================\n");
	//*/
	/*
	printf("---------------------simkey->str_dekey_map -----------------\n");
	for(i=0; i<256; i++){
		if((i%16) == 0){
			printf("\n");		
		}

		printf("%#.2x ", simkey->str_dekey_map[i]);

	}
	printf("\n=======================================================\n");
	//*/


	return simkey;

}

void sim_encrypt(simkey_st *simkey, u_char *ciphertext, u_char *plaintext, int textlen)
{
	int i;
	//char hi, lo;

	if(simkey==NULL){
		memcpy(ciphertext, plaintext, textlen);
		return;
	}

	for(i=0; i<textlen; i++){
		ciphertext[i] = simkey->enkey_map[plaintext[i]];
	}

}

void sim_decrypt(simkey_st *simkey, u_char *plaintext, u_char *ciphertext, int textlen)
{
	int i;
	//char hi, lo;

	if(simkey==NULL){
		memcpy(plaintext, ciphertext, textlen);
		return;
	}


	for(i=0; i<textlen; i++){
		plaintext[i] = simkey->dekey_map[ciphertext[i]] ;
	}

}

int sim_str_encrypt(simkey_st *simkey, u_char *ciphertext, u_char *plaintext)
{
	int i;

	if(simkey==NULL){
		strcpy(ciphertext, plaintext);
		return strlen(ciphertext);
	}

	for(i=0; plaintext[i]; i++){
		ciphertext[i] = simkey->str_enkey_map[plaintext[i]] ;
		//printf("%#.2x :en: %#.2x \n", plaintext[i], ciphertext[i]);
	}

	ciphertext[i] = 0;

	return i;
}

int sim_str_decrypt(simkey_st *simkey, u_char *plaintext, u_char *ciphertext)
{
	int i;
	
	if(simkey==NULL){
		strcpy(plaintext, ciphertext);
		return strlen(plaintext);
	}

	for(i=0; ciphertext[i]; i++){
		plaintext[i] = simkey->str_dekey_map[ciphertext[i]] ;
		//printf("%#.2x :de: %#.2x \n", plaintext[i], ciphertext[i]);
	}

	plaintext[i] = 0;

	return i;
}


#define SFCH1		0x60
#define SFCH2		0x7c

#define	SCCH_AS_IT_IS_BIT	(0x01<<5)
#define	SCCH_REASSEMBLE_BIT	(1<<6)
#define	SCCH_IDENTICAL_BIT	(1<<5)


#define	SCH_MAX_CONT_COUNT	(0x1f)
#define	SCH_IDENTICAL_FLAG	1
#define	SCH_INCREASE_FLAG		2

/*
ctl_char	:	bit[7]0, bit[6] 0:as it is, 1:reassemble
bit[6]  0
	bit[5] 1
	bit[4:0]
bit[6]  1	consecutive
	bit[5] 0 : increase, 1 identical
	bit[4:0] count
*/
int sim_str_reassemble(u_char *destdata, u_char *srcdata, int datalen)
{
	int i, count;
	int d_offs, s_offs;
	int consecutive_flag, last_flag;		// 0 none, 1 identical, 2 increase
	u_char *pdchar, *pschar;

	pdchar = destdata;
	pschar = srcdata;

	if(pschar[0] == pschar[1]){
		consecutive_flag = SCH_IDENTICAL_FLAG;
	}
	/*
	else if(pschar[0] == pschar[1]-1){
		consecutive_flag = SCH_INCREASE_FLAG;
	}
	//*/
	else{
		consecutive_flag = 0;
	}

	last_flag = consecutive_flag;

	while((pschar-srcdata)<datalen){

		if((*pschar<SIM_STR_S) || (*pschar>SIM_STR_E)){
			*pdchar++ = *pschar++;
			continue;
		}
		
		pdchar[0] = SFCH1;
		pdchar[1] = SFCH2;
		//pdchar[2] = 0;

		consecutive_flag = last_flag;

		for(s_offs=0; s_offs<SCH_MAX_CONT_COUNT; s_offs++){
			if((pschar+s_offs+1-srcdata)>=datalen){
				break;			
			}
			if(pschar[s_offs] == pschar[s_offs+1]){
				if(consecutive_flag != SCH_IDENTICAL_FLAG){
					last_flag = SCH_IDENTICAL_FLAG;
					break;
				}
			}
			/*
			else if(pschar[s_offs] == pschar[s_offs+1]-1){
				if(consecutive_flag != SCH_INCREASE_FLAG){
					last_flag = SCH_INCREASE_FLAG;
					break;
				}
			}
			//*/
			else{
				if(consecutive_flag != 0){
					last_flag = 0;
					break;
				}
			}

		}
		
		
			
		if(consecutive_flag == SCH_IDENTICAL_FLAG){
			s_offs++;
			pdchar[2] = SCCH_REASSEMBLE_BIT | SCCH_IDENTICAL_BIT | s_offs;
			pdchar[3] = pschar[0];
			pdchar += 4;
		}
		/*
		else if(consecutive_flag == SCH_INCREASE_FLAG){
			s_offs++;
			pdchar[2] = SCCH_REASSEMBLE_BIT | s_offs;
			pdchar[3] = pschar[0];
			pdchar += 4;
		}
		//*/
		else{
			pdchar[2] = SCCH_AS_IT_IS_BIT | s_offs;
			pdchar += 3;
			for(i=0; i<s_offs; i++){
				*pdchar++ = pschar[i];
			}
		}

		pschar += s_offs;
		
		printf("(pschar-srcdata) = %d \n", (pschar-srcdata));
	}

	return (pdchar-destdata);
}

int sim_str_restore(u_char *destdata, u_char *srcdata, int datalen)
{
	int i, count;
	u_char cntl_char;
	u_char *pdchar, *pschar;

	pdchar = destdata;
	pschar = srcdata;

	while((pschar-srcdata)<datalen){

		if((*pschar<SIM_STR_S) || (*pschar>SIM_STR_E)){
			*pdchar++ = *pschar++;
			continue;
		}

		
		if((pschar[0] == SFCH1) && (pschar[1] == SFCH2)){

			cntl_char = pschar[2];
			count = cntl_char & 0x1f;
			
			if(cntl_char & SCCH_REASSEMBLE_BIT){
				//if(cntl_char & SCCH_IDENTICAL_BIT){
					for(i=0; i<count; i++){
						*pdchar++ = pschar[3];
					}
				/*
				}else{
					for(i=0; i<count; i++){
						*pdchar++ = pschar[3]+i;
					}
				}
				//*/
				pschar += 4;
			}else{
				pschar += 3;
				for(i=0; i<count; i++){
					*pdchar++ = *pschar++;
				}
				
			}
		}else{
			*pdchar++ = *pschar++;
		}
		printf("(pschar-srcdata) = %d \n", (pschar-srcdata));
						
	}

	return (pdchar-destdata);
}

int sim_compress(u_char *destdata, u_char *srcdata, int datalen)
{

	return 0;
}

int sim_extract(u_char *destdata, u_char *srcdata, int datalen)
{

	return 0;
}

int str_encrypt(u_char *ciphertext, u_char *plaintext)
{
	int len;
	u_char databuf[SIM_BUFSIZE];

	len = sim_str_reassemble(databuf, plaintext, strlen(plaintext));
	databuf[len] = 0;
	sim_str_encrypt(&default_simkey, ciphertext, databuf);

	return len;
}


int str_decrypt(u_char *plaintext, u_char *ciphertext)
{
	int len;
	u_char databuf[SIM_BUFSIZE];

	len = sim_str_decrypt(&default_simkey, databuf, ciphertext);
	len = sim_str_restore(plaintext, databuf, len);
	plaintext[len] = 0;

	return len;
}


int data_encrypt(u_char *ciphertext, u_char *plaintext, int textlen)
{
	int len;
	u_char databuf[SIM_BUFSIZE];

	if(textlen>SIM_BUFSIZE){
		textlen = SIM_BUFSIZE;
	}

	len = sim_compress(databuf, plaintext, textlen);
	databuf[len] = 0;
	sim_encrypt(&default_simkey, ciphertext, databuf, len);

	return len;
}


int data_decrypt(u_char *plaintext, u_char *ciphertext, int textlen)
{
	int len;
	u_char databuf[SIM_BUFSIZE];

	if(textlen>SIM_BUFSIZE){
		textlen = SIM_BUFSIZE;
	}

	sim_decrypt(&default_simkey, databuf, ciphertext, textlen);
	len = sim_extract(plaintext, databuf, textlen);
	plaintext[len] = 0;

	return len;
}

void init_sim_encrypt(void)
{
	init_simkey(&default_simkey, SIMKEY);
}

