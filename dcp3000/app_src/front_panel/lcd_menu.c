#include "../appref.h"
#include "front_panel.h"
//#include "../web/webserver.h"
#include "../rtc.h"

const char *Switch_Menu[] = {
	"OFF",
	"ON"
};

void Display_Error_Menu(char *first_line, char *second_line)
{
	int key_value;
	u_int result = 0;

	LcdDisplay(FIRST_LINE, first_line);
	LcdDisplay(SECOND_LINE, second_line);
	while(1) { 	   
		
		key_value = GetKeyValue(1000);
		if(key_value==EXIT_KEY)
			break;
		result++;
		if(result>2)
			break;
	}
}


u_int LcdMenux(int *cur_menu, int total_menu, const char *menu_title, const char **menu_struct)
{
	int key_value;

	if(*cur_menu > total_menu)
		*cur_menu = 1;
	
	LcdDisplay(FIRST_LINE, menu_title);

	do {
		LcdDisplay(SECOND_LINE, menu_struct[*cur_menu - 1]);

		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value) {
			case    NO_KEY:
			case    KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case    EXIT_KEY:
				return BACKBEFORE;
			case    ENTER_KEY:
				return *cur_menu;
			case    LEFT_KEY:
				break;
			case    UP_KEY:
				if(--(*cur_menu) == 0)
					*cur_menu = total_menu;
				break;
			case    DOWN_KEY:
				if(++(*cur_menu) > total_menu)
					*cur_menu = 1;
				break;
			case    RIGHT_KEY:
				break;
			default :
				break;
		}
	} while (1);
}

int LcdEditOption(int lcd_line, int *option, int min, int max, const char *menu_struct[], const char *prefix, const char *suffix)
{
	u_char buf[25];
	u_int index  = 0;
	int key_value;
	int temp;

	temp = *option;
	if((temp < min) || (temp > max))
		temp = min;
	
	do{
		sprintf(buf, "%s%s%s", prefix, menu_struct[temp], suffix);

		LcdDisplay(lcd_line, buf);
		LcdFlashCursor(lcd_line, index);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value){
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				if ( *option == temp )
				{					
					return BACKBEFORE;
				}
				else
				{
					*option = temp;
					return SAVERESULT;
				}
			case	DOWN_KEY:
				temp++;
				if(temp > max)	
					temp = min;				
				break;
			case	UP_KEY:
				temp--;
				if(temp < min)	
					temp = max;
				break;
		}	
	}while(1);
}

int LcdEditHexString(int lcd_line, u_char *buffer, int start, int end)
{
	int key_value;
	u_int index = start;
	
	int num = 0;
	int i = 0;
	num = strlen(buffer);

	for( i = num;i< 20;i++)
	{
		buffer[i] = ' ';	
	}
	
	do{
		LcdDisplay(lcd_line, buffer);
		LcdFlashCursor(lcd_line, index);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		
		switch(key_value) {
			case	LEFT_KEY:
				if(--index < start)
					index = end;
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				return SAVERESULT;
			case    DOWN_KEY:
				buffer[index]--;
				if(buffer[index] < 0x20){
					buffer[index] = 0x46;
				}else if((buffer[index] < 0x30) && (buffer[index] > 0x20)){
					buffer[index] = 0x20;
				}else if((buffer[index] < 0x41) && (buffer[index] > 0x39)){
					buffer[index] = 0x39;
				}
				break;
			case	UP_KEY:
				buffer[index]++;
				if(buffer[index] > 0x46){
					buffer[index] = 0x20;
				}else if((buffer[index] < 0x41) && (buffer[index] > 0x39)){
					buffer[index] = 0x41;
				}else if((buffer[index] < 0x30) && (buffer[index] > 0x20)){
					buffer[index] = 0x30;
				}
				break;
			case	RIGHT_KEY:
				if(++index > end)
					index = start;
				break;
		}		
	}while(1);
}


int LcdEditString(int lcd_line, const char *Title, char *buffer, int start, int end)
{
	int key_value;
	u_int index = start;
	
	int num = 0;
	int i = 0;
	num = strlen(buffer);

	for( i = num;i< 20;i++)
	{
		buffer[i] = ' ';	
	}
	LcdDisplay(FIRST_LINE, Title);
	
	do{
		LcdDisplay(lcd_line, buffer);
		LcdFlashCursor(lcd_line, index);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		
		switch(key_value) {
			case	LEFT_KEY:
				if(--index < start)
					index = end;
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				return SAVERESULT;
			case    DOWN_KEY:
				buffer[index]--;
				if(buffer[index] < 0x20)
					buffer[index] = 0x7e;
				break;
			case	UP_KEY:
				buffer[index]++;
				if(buffer[index] > 0x7e){
					buffer[index] = 0x20;
				}
				break;
			case	RIGHT_KEY:
				if(++index > end)
					index = start;
				break;
		}		
	}while(1);
}

int LcdEditNameString(int lcd_line, char *buffer, int start, int end)
{
	int key_value;
	u_int index = start;
	
	int num = 0;
	int i = 0;
	num = strlen(buffer);

	for( i = num;i< 20;i++)
	{
		buffer[i] = ' ';	
	}
	
	do{
		LcdDisplay(lcd_line, buffer);
		LcdFlashCursor(lcd_line, index);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		
		switch(key_value) {
			case	LEFT_KEY:
				if(--index < start)
					index = end;
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				return SAVERESULT;
			case    DOWN_KEY:
				buffer[index]--;
				if(buffer[index] > 0x5f && buffer[index] < 0x61){
					buffer[index] = 0x5f;
				}else if(buffer[index] > 0x5a && buffer[index] < 0x5f){
					buffer[index] = 0x5a;
				}else if(buffer[index] > 0x39 && buffer[index] < 0x41){
					buffer[index] = 0x39;
				}else if(buffer[index] > 0x2d && buffer[index] < 0x30){
					buffer[index] = 0x2d;
				}else if(buffer[index] > 0x20 && buffer[index] < 0x2d){
					buffer[index] = 0x20;
				}else if(buffer[index] <0x20){
					buffer[index] = 0x7a;
				}
				break;
			case	UP_KEY:
				buffer[index]++;
				if(buffer[index] > 0x7a){
					buffer[index] = 0x20;
				}else if(buffer[index] > 0x5f && buffer[index] < 0x61){
					buffer[index] = 0x61;
				}else if(buffer[index] > 0x5a && buffer[index] < 0x5f){
					buffer[index] = 0x5f;
				}else if(buffer[index] > 0x39 && buffer[index] < 0x41){
					buffer[index] = 0x41;
				}else if(buffer[index] > 0x2d && buffer[index] < 0x30){
					buffer[index] = 0x30;
				}else if(buffer[index] > 0x20 && buffer[index] < 0x2d){
					buffer[index] = 0x2d;
				}
				break;
			case	RIGHT_KEY:
				if(++index > end)
					index = start;
				break;
		}		
	}while(1);
}


int LcdEditPasswd(const u_char *Title, u_char *buffer, u_int start, u_int end)
{
	int key_value;
	u_int index = start;
	char pwbuf[21];
	
	int num = 0;
		int i = 0;
		num = strlen(buffer);
	
		for( i = num;i< 19;i++)
		{
			buffer[i] = ' ';	
		}
	LcdDisplay(FIRST_LINE, Title);

	sprintf(pwbuf, "%0.*s", end-start+1, "********************");
	do{
		pwbuf[index] = buffer[index];
		LcdDisplay(SECOND_LINE, pwbuf);
		LcdFlashCursor(SECOND_LINE, index);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		
		switch(key_value) {
			case	LEFT_KEY:
				pwbuf[index] = '*';
				if(--index > end)
					index = end;
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				
				return SAVERESULT;
			case    DOWN_KEY:
				buffer[index]--;
				if(buffer[index] < 0x20)
					buffer[index] = 0x7e;
				break;
			case	UP_KEY:
				buffer[index]++;
				if(buffer[index] > 0x7e){
					buffer[index] = 0x20;
				}
				break;
			case	RIGHT_KEY:
				pwbuf[index] = '*';
				if(++index > end)
					index = start;
				break;
		}		
	}while(1);
}


int LcdEditIntVarible(int lcd_line, int *value, int precision, int min_value, int max_value, const char *prefix, const char *suffix)
{
	u_char buf[25];
	u_char min_val[24];
	u_char max_val[24];
	u_char *p_buf = buf;
	u_int index, start, end;
	int low_idx=0, top_idx=0;
	int isSigned, v_s32;
	int key_value;
	int i;

	if(min_value>=max_value){
		return -1;
	}
	
	if(prefix){
		start = strlen(prefix);
	}else{
		prefix = "";
		start = 0;		
	}

	if(suffix==NULL){
		suffix = "";
	}

	end = start + precision - 1;
	
	
	if(min_value<0){
		isSigned = 1;
		end++;
	}else{
		isSigned = 0;
	}
	
	index = start;
	low_idx = start+isSigned;
	top_idx = start+isSigned;
	
	sprintf(&min_val[start], "%.*d", precision, min_value);
	if(max_value>=0){
		sprintf(&max_val[start], "%.*d", precision+isSigned, max_value);
	}else{
		sprintf(&max_val[start], "%.*d", precision, max_value);
	}
	if(*value>=0){
		sprintf(buf, "%s%.*d%s", prefix, precision+isSigned, *value, suffix);
	}else{
		sprintf(buf, "%s%.*d%s", prefix, precision, *value, suffix);
	}
	
	#if 0
	printf("Lcd modify int val : start = %d, end = %d \n", start, end);
	for(i=start; i<=end; i++){
		putchar(min_val[i]);
	}
	putchar('\n');
	for(i=start; i<=end; i++){
		putchar(max_val[i]);
	}
	printchar('\n');
	#endif
	
	do{
		LcdDisplay(lcd_line, buf);
		LcdFlashCursor(lcd_line, index);

		
		//printf("Lcd modify int val : index = %d, low_idx = %d, top_idx = %d \n", index, low_idx, top_idx);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);


		switch(key_value){
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				v_s32 = atoi(p_buf + start);
				if((v_s32<=max_value) && (v_s32>=min_value)){
					*value = v_s32;
					return SAVERESULT;
				}else{
					index = start;
					low_idx = start+isSigned;
					top_idx = start+isSigned;
					if(*value>=0){
						sprintf(buf, "%s%.*d%s", prefix, precision+isSigned, *value, suffix);
					}else{
						sprintf(buf, "%s%.*d%s", prefix, precision, *value, suffix);
					}
					break;
				}
			case	LEFT_KEY:
				index--;
				if(index < start)
					index = end;
				break;
			
			case	RIGHT_KEY:
				index++;
				if(index > end)
					index = start;
				break;
			case	DOWN_KEY:
				
				if(isSigned && (index==start)){
					if(buf[start] == 0x30){
						buf[start]=0x2d;
						top_idx = start + isSigned;
					}else if(buf[start] == 0x2d){
						if(max_value>=0){
							buf[start]=0x30;
							low_idx = start + isSigned;
						}
					}
					
				}else{

					buf[index]--;
					
					if(buf[start] == 0x2d){

						if(max_value<0){
							if(index < top_idx){
								if(buf[index] < max_val[index]){
									buf[index]++;
									break;
								}
							}else if(index == top_idx){
								if(buf[index] <= max_val[index]){
									buf[index] = max_val[index];
									while(top_idx<end){
										top_idx++;
										if(buf[top_idx] > max_val[top_idx]){
											break;
										}else if(buf[top_idx] < max_val[top_idx]){
											buf[top_idx] = max_val[top_idx];
										}
									}
									
								}
							}
							
						}
						
						
						if(index <= low_idx){
							if(buf[index] < min_val[index]){
								low_idx = index;
							}
							if(buf[index] >= min_val[index]){
								buf[index] = min_val[index];
								while(low_idx<end){
									low_idx++;
									if(buf[low_idx] < min_val[low_idx]){
										break;
									}else if(buf[low_idx] > min_val[low_idx]){
										buf[low_idx] = min_val[low_idx];
									}
								}
							}
						}
				
						if(buf[index] < 0x30){
							buf[index] = 0x30;
						}
						
					}else{
						if(isSigned){
							if(buf[index] < 0x30){
								buf[index] = 0x30;
							}
						}else{
							
							if(index == low_idx){
								if(buf[index] <= min_val[index]){
									buf[index] = min_val[index];
									while(low_idx<end){
										low_idx++;
										if(buf[low_idx] > min_val[low_idx]){
											break;
										}else if(buf[low_idx] < min_val[low_idx]){
											buf[low_idx] = min_val[low_idx];
										}
									}
									
								}
							}else if(index > low_idx){
								if(buf[index] <  0x30){
									buf[index] = 0x30;
								}
							}else{
								buf[index]++;
							}
						}

						if(index < top_idx){
							if(buf[index] < max_val[index]){
								top_idx = index;
							}
						}else if(index == top_idx){
							if(buf[index] >= max_val[index]){
								buf[index] = max_val[index];
								while(top_idx<end){
									top_idx++;
									if(buf[top_idx] < max_val[top_idx]){
										break;
									}else if(buf[top_idx] > max_val[top_idx]){
										buf[top_idx] = max_val[top_idx];
									}
								}
							}
						}
					}
					
				
				}
				
				break;
			case	UP_KEY:
				
				if(isSigned && (index==start)){
					if(buf[start] == 0x30){
						buf[start]=0x2d;
						top_idx = start + isSigned;
					}else if(buf[start] == 0x2d){
						if(max_value>=0){
							buf[start]=0x30;
							low_idx = start + isSigned;
						}
					}
					
				}else{
				
					if(buf[start] == 0x2d){
						
						if(index < low_idx){
							break;
						}
						
						buf[index]++;
					
						if(index == low_idx){
							if(buf[index] >= min_val[index]){
								buf[index] = min_val[index];
								while(low_idx<end){
									low_idx++;
									if(buf[low_idx] < min_val[low_idx]){
										break;
									}else if(buf[low_idx] > min_val[low_idx]){
										buf[low_idx] = min_val[low_idx];
									}
								}
							}
						}else if(index > low_idx){
							if(buf[index] > 0x39){
								buf[index] = 0x39;
							}
						}

						if(max_value<0){
							if(index < top_idx){
								if(buf[index] > max_val[index])
									top_idx = index;
							}
							if(buf[index] > 0x39){
								buf[index] = 0x39;
							}
						}
						
					}else{
					
						if(index < top_idx){
							break;
						}
						
						buf[index]++;

						if(index == top_idx){
							if(buf[index] >= max_val[index]){
								buf[index] = max_val[index];
								while(top_idx<end){
									top_idx++;
									if(buf[top_idx] < max_val[top_idx]){
										break;
									}else if(buf[top_idx] > max_val[top_idx]){
										buf[top_idx] = max_val[top_idx];
									}
								}
							}
						}else if(index > top_idx){
							if(buf[index] > 0x39){
								buf[index] = 0x39;
							}
						}

						if(index < low_idx){
							if(buf[index] > min_val[index]){
								low_idx = index;
							}
						}
						
					}
						
				}
				break;
		}	
	}while(1);
}

int LcdEditFloatVarible(int lcd_line, double *value, int numwidth, int precision, double min_value, double max_value, const char *prefix, const char *suffix)
{
	u_char buf[24];
	u_char min_val[24];
	u_char max_val[24];
	u_char *p_buf = buf;
	u_int index, dot_idx, start, end;
	int low_idx=0, top_idx=0;
	int isSigned;
	double  v_lf;
	int key_value;
	int i;

	if(min_value>=max_value){
		return -1;
	}
	
	if(prefix){
		start = strlen(prefix);
	}else{
		prefix = "";
		start = 0;		
	}

	if(suffix==NULL){
		suffix = "";
	}

	end = start + numwidth - 1;
	
	
	if(min_value<0){
		isSigned = 1;
		numwidth++;
		end++;
	}else{
		isSigned = 0;
	}
	
	index = start;
	low_idx = start+isSigned;
	top_idx = start+isSigned;



	if(precision > 0){
		dot_idx = end - precision;
	}else{
		dot_idx = start+numwidth;
	}

	
	sprintf(&min_val[start], "%0*.*f", numwidth, precision, min_value);

	sprintf(&max_val[start], "%0*.*f", numwidth, precision, max_value);
	sprintf(p_buf, "%s%0*.*f%s", prefix, numwidth, precision, *value, suffix);

	#if 0
	printf("Lcd modify float val : start = %d, end = %d \n", start, end);
	for(i=start; i<=end; i++){
		putchar(min_val[i]);
	}
	putchar('\n');
	for(i=start; i<=end; i++){
		putchar(max_val[i]);
	}
	printchar('\n');
	#endif
	
	do{
		LcdDisplay(lcd_line, buf);
		LcdFlashCursor(lcd_line, index);

		//printf("Lcd modify float val : index = %d, low_idx = %d, top_idx = %d \n", index, low_idx, top_idx);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value){
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				v_lf = atof(p_buf + start);
				if((v_lf<=max_value) && (v_lf>=min_value)){
					*value = v_lf;
					return SAVERESULT;
				}else{
					index = start;
					low_idx = start+isSigned;
					top_idx = start+isSigned;

					sprintf(p_buf, "%s%0*.*f%s", prefix, numwidth, precision, *value, suffix);

					break;
				}
				return SAVERESULT;
			case	LEFT_KEY:
				index--;
				if(index < start){
					index = end;
				}else if(index == dot_idx){
					index--;
				}
				break;
			case	RIGHT_KEY:
				index++;
				if(index > end){
					index = start;
				}else if(index == dot_idx){
					index++;
				}
				break;
			case	DOWN_KEY:
				
				if(isSigned && (index==start)){
					if(buf[start] == 0x30){
						buf[start]=0x2d;
						top_idx = start + isSigned;
					}else if(buf[start] == 0x2d){
						if(max_value>=0){
							buf[start]=0x30;
							low_idx = start + isSigned;
						}
					}
					
				}else{

					buf[index]--;
					
					if(buf[start] == 0x2d){

						if(max_value<0){
							if(index < top_idx){
								if(buf[index] < max_val[index]){
									buf[index]++;
									break;
								}
							}else if(index == top_idx){
								if(buf[index] <= max_val[index]){
									buf[index] = max_val[index];
									while(top_idx<end){
										top_idx++;
										if(buf[top_idx] > max_val[top_idx]){
											break;
										}else if(buf[top_idx] < max_val[top_idx]){
											buf[top_idx] = max_val[top_idx];
										}
									}
									
								}
							}
							
						}
						
						
						if(index <= low_idx){
							if(buf[index] < min_val[index]){
								low_idx = index;
							}
							if(buf[index] >= min_val[index]){
								buf[index] = min_val[index];
								while(low_idx<end){
									low_idx++;
									if(buf[low_idx] < min_val[low_idx]){
										break;
									}else if(buf[low_idx] > min_val[low_idx]){
										buf[low_idx] = min_val[low_idx];
									}
								}
							}
						}
				
						if(buf[index] < 0x30){
							buf[index] = 0x30;
						}
						
					}else{
						if(isSigned){
							if(buf[index] < 0x30){
								buf[index] = 0x30;
							}
						}else{
							
							if(index == low_idx){
								if(buf[index] <= min_val[index]){
									buf[index] = min_val[index];
									while(low_idx<end){
										low_idx++;
										if(buf[low_idx] > min_val[low_idx]){
											break;
										}else if(buf[low_idx] < min_val[low_idx]){
											buf[low_idx] = min_val[low_idx];
										}
									}
									
								}
							}else if(index > low_idx){
								if(buf[index] <  0x30){
									buf[index] = 0x30;
								}
							}else{
								buf[index]++;
							}
						}

						if(index < top_idx){
							if(buf[index] < max_val[index]){
								top_idx = index;
							}
						}else if(index == top_idx){
							if(buf[index] >= max_val[index]){
								buf[index] = max_val[index];
								while(top_idx<end){
									top_idx++;
									if(buf[top_idx] < max_val[top_idx]){
										break;
									}else if(buf[top_idx] > max_val[top_idx]){
										buf[top_idx] = max_val[top_idx];
									}
								}
							}
						}
					}
					
				
				}
				
				break;
			case	UP_KEY:
				
				if(isSigned && (index==start)){
					if(buf[start] == 0x30){
						buf[start]=0x2d;
						top_idx = start + isSigned;
					}else if(buf[start] == 0x2d){
						if(max_value>=0){
							buf[start]=0x30;
							low_idx = start + isSigned;
						}
					}
					
				}else{
				
					if(buf[start] == 0x2d){
						
						if(index < low_idx){
							break;
						}
						
						buf[index]++;
					
						if(index == low_idx){
							if(buf[index] >= min_val[index]){
								buf[index] = min_val[index];
								while(low_idx<end){
									low_idx++;
									if(buf[low_idx] < min_val[low_idx]){
										break;
									}else if(buf[low_idx] > min_val[low_idx]){
										buf[low_idx] = min_val[low_idx];
									}
								}
							}
						}else if(index > low_idx){
							if(buf[index] > 0x39){
								buf[index] = 0x39;
							}
						}

						if(max_value<0){
							if(index < top_idx){
								if(buf[index] > max_val[index])
									top_idx = index;
							}
							if(buf[index] > 0x39){
								buf[index] = 0x39;
							}
						}
						
					}else{
					
						if(index < top_idx){
							break;
						}
						
						buf[index]++;

						if(index == top_idx){
							if(buf[index] >= max_val[index]){
								buf[index] = max_val[index];
								while(top_idx<end){
									top_idx++;
									if(buf[top_idx] < max_val[top_idx]){
										break;
									}else if(buf[top_idx] > max_val[top_idx]){
										buf[top_idx] = max_val[top_idx];
									}
								}
							}
						}else if(index > top_idx){
							if(buf[index] > 0x39){
								buf[index] = 0x39;
							}
						}

						if(index < low_idx){
							if(buf[index] > min_val[index]){
								low_idx = index;
							}
						}
						
					}
						
				}
				break;
		}	
	}while(1);
}


#define Lcd_Wait_Menu(line, index, str_buf, condition, timeout_sec) \
	do{	\
		sprintf(str_buf, "Please Wait ... ...");	\
		index= 12;	\
		do{	\
			str_buf[index] = 0;	\
			LcdDisplay(line, str_buf);	\
			sleep(1);	\
			str_buf[index] = '.';	\
			index++;	\
			if(index==15){	\
				index++;	\
			}else if(index>19){	\
				index=12;	\
			}	\
			if(condition){printf("Lcd_Wait_Menu : finished \n");break;} \
			if(timeout_sec==1){ \
				timeout_sec = -1; \
printf("Lcd_Wait_Menu: timout \n"); \
				break; \
			}else if(timeout_sec>0){	\
				timeout_sec--; \ 
			} \
		}while(1);	\
	}while(0)

int Lcd_Pause_Menu(const char *line1_str, const char *line2_str)
{
	int key_value;

	do{
		LcdDisplay(FIRST_LINE, line1_str);
		LcdDisplay(SECOND_LINE, line2_str);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);	
		if(key_value == EXIT_KEY)
		{
			return BACKBEFORE;
		}
		else if(key_value == ENTER_KEY)
		{
			return SAVERESULT;
		}
	}while(1);
}

int Lcd_Confirm_Menu(const char *first_line_str)
{
	int key_value;

	do{
		LcdDisplay(FIRST_LINE, first_line_str);
		LcdDisplay(SECOND_LINE, "ENTER=YES  EXIT=NO");
		
		key_value = GetKeyValue(TIME_OUT_COUNT);	
		if(key_value == EXIT_KEY)
		{
			return BACKBEFORE;
		}
		else if(key_value == ENTER_KEY)
		{
			return SAVERESULT;
		}
	}while(1);
}

// less than billion
int Lcd_ConfirmPassword(u_int pw, int pwlen)
{
	u_char buf[20];
	u_int index = 0;
	u_int value = 0;
	int key_value;

		LcdDisplay(FIRST_LINE, "Password");
		sprintf(buf, "%.*d", pwlen, value);
	
	do {		
		LcdDisplay(SECOND_LINE, buf);
		LcdFlashCursor(SECOND_LINE, index);

		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value) {
			case    NO_KEY:
			case 	   KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				
				value = strtoul(buf, NULL, 10);
				
				if((ADMIN_PASSWD == value) || (pw == value))
					return 1;
				else
					return 0;
			case	LEFT_KEY:
				if(--index < 0)
					index = 5;
				break;
			case	DOWN_KEY:
				buf[index]--;
				if(buf[index] < 0x30)	
					buf[index] = 0x39;
				break;
			case	UP_KEY:
				buf[index]++;
				if(buf[index] > 0x39)	
					buf[index] = 0x30;
				break;
			case	RIGHT_KEY:
				if(++index > pwlen)
					index = 0;
				break;
		}	
	} while(1);
}

int LcdMenu_Encrypt(u_int pw)
{  
	int a, b, c, d;
	int key_value;
	int result = 0;

	do {    
		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value) {
			case    NO_KEY:
			case 	   KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case    EXIT_KEY:
				return BACKBEFORE;
			case    ENTER_KEY:
				//printf("Encrypt result == %d\n", result);
				return (result == pw)?1:0;
			case    LEFT_KEY:
				result++;
				//printf("result == %d\n", result);
				break;
			case    DOWN_KEY:
				break;
			case    UP_KEY:
				break;
			case    RIGHT_KEY:
				result = result * 2;
				//printf("result == %d\n", result);
				break;
			default :
				break;
		}
	} while (1);

	return result;
}


u_int Lcd_Set_Mac_Menu(const char *Menu_Title, u_char *p_mac)
{
	u_char buf[24];
	//u_char para[80];
	u_int index = 0;
	int i;
	int key_value;
	int result;

	
	mac_arr2str(p_mac, buf);

	//sprintf(buf, "%s", mac);

	LcdDisplay(FIRST_LINE, Menu_Title);

	do { 	   
		LcdDisplay(SECOND_LINE, buf);
		LcdFlashCursor(SECOND_LINE, index);

		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value) {
			case    NO_KEY:
			case 	   KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case    EXIT_KEY:
				return BACKBEFORE;
			case    ENTER_KEY:
				for(i = 0; i < 17; i++){
					if((i == 2) || (i == 5) || (i == 8)|| (i == 11)|| (i == 14))
						continue;
					else if((buf[i] >= 0x30) && (buf[i] <0x40))
						buf[i] -= 0x30;
					else if((buf[i] > 0x40) && (buf[i] < 0x47))
						buf[i] -= 0x37;
				}

				mac_str2arr(buf, p_mac);
				
				return SAVERESULT;
			case    LEFT_KEY:
				index--;
				if((index == 2) || (index == 5) || (index == 8)|| (index == 11)|| (index == 14))
					index--;
				else if(index > 16)
					index = 16;
				break;
			case    DOWN_KEY:
				buf[index]--;

				if((buf[index] < 0x30) ||(buf[index] > 0x46))   buf[index] = 0x46;
				else if((buf[index] < 0x41) && (buf[index] > 0x39)) buf[index] = 0x39;
				break;
			case    UP_KEY:
				buf[index]++;

				if((buf[index] < 0x30) ||(buf[index] > 0x46))   buf[index] = 0x30;
				else if((buf[index] < 0x41) && (buf[index] > 0x39)) buf[index] = 0x41;
				break;
			case    RIGHT_KEY:
				index++;
				if((index == 2) || (index == 5) || (index == 8)|| (index == 11)|| (index == 14))
					index++;
				else if(index == 17)
					index = 0;
				break;
			default :
				break;
		}
	} while (1);
}

u_int Lcd_Set_IpAddr_Menu(const char *menu_title,u_int *p_addr)
{
	u_char buf[24];
	int index = 0;
	u_char l1, l2, l3, l4;
	int i;
	int key_value;
	u_int result;

	ip_i2strxxx(*p_addr, buf);

	LcdDisplay(FIRST_LINE, menu_title);

	do { 	   
		LcdDisplay(SECOND_LINE, buf);
		LcdFlashCursor(SECOND_LINE, index);

		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value) {
			case    NO_KEY:
			case 	   KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case    EXIT_KEY:
				return BACKBEFORE;
			case    ENTER_KEY:

				*p_addr = ip_a2i(buf);
	
				return SAVERESULT;
			case    LEFT_KEY:
				index--;
				if(index%4 == 3 )
					index--;
				else if(index < 0)
					index = 14;
				break;
			case    DOWN_KEY:
				buf[index]--;
				if(buf[index] < 0x30){
					if(index%4 == 0 ) {
						buf[index] = 0x32;
					}else if((index%4 == 1 ) && (buf[index-1] == 0x32)) {
						buf[index] = 0x35;
					}else if((index%4 == 2 ) && (buf[index-2] == 0x32) &&(buf[index-1] == 0x35)) {
						buf[index] = 0x35;
					}else{
						buf[index] = 0x39;
					}
				}
				break;
			case    UP_KEY:
				buf[index]++;
				if(index%4 == 0 ) {
					if(buf[index] > 0x32)   buf[index] = 0x30;
				}else if((index%4 == 1 ) && (buf[index-1] == 0x32)) {
					if(buf[index] > 0x35)   buf[index] = 0x30;
				}else if((index%4 == 2 ) && (buf[index-2] == 0x32) &&(buf[index-1] == 0x35)) {
					if(buf[index] > 0x35)   buf[index] = 0x30;
				}

				if(buf[index] > 0x39)   buf[index] = 0x30;
				
				break;
			case    RIGHT_KEY:
				index++;
				if(index%4 == 3)
					index++;
				else if(index >= 15)
					index = 0;
				break;
			default :
				break;
		}
	} while (1);

	
}

u_int Version_Menu(void)
{
	int key_value, result;
	u_char buf[24],buf_first_line[24];
	int c_menu = 1;
	int total_menu = 3;
	
	
	do{
		
		if(c_menu == 1){
			sprintf(buf_first_line, "Main Ver. %#x", sysinfo->main_version);
			sprintf(buf, "Firmware Ver. %#x", sysinfo->firmware_version);
		}else if(c_menu == 2){
			sprintf(buf_first_line, "App Ver. %#x", sysinfo->app_version);
			sprintf(buf, "FPGA Ver. %#x", sysinfo->fpga_version);
		}else if(c_menu == 3){
			sprintf(buf_first_line, "Kernel Ve. %#x", sysinfo->kernel_version);
			sprintf(buf, "Web Ver. %#x", sysinfo->web_version);
		}

		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);	
			
		key_value = GetKeyValue(500);

	
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

			
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);

}

u_int NetWork_Menu(void)
{
	
	u_int v_u32;
	int status;
	int key_value;
	static int cur_menu = 1;
	int total_menu = 4;
	char buf[24];

	static const char *NetworkMenu[] = {
		"M. Net/IP Address",
		"M. Net/Subnet Mask",
		"M. Net/Gateway",
		"M. Net/MAC Address"
	};

	while(1){
		
		if(cur_menu == 1){
			ip_i2strxxx(mng_network->ipaddr, buf);
		}else if(cur_menu == 2){
			ip_i2strxxx(mng_network->netmask, buf);
		}else if(cur_menu == 3){
			ip_i2strxxx(mng_network->gateway, buf);
		}else if(cur_menu == 4) {
			mac_arr2str(mng_network->macaddr, buf);
		}
		
		LcdDisplay(FIRST_LINE, NetworkMenu[cur_menu - 1]);
		LcdDisplay(SECOND_LINE, buf);

		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value) {
			case    NO_KEY:
			case 	   KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case    EXIT_KEY:
				return BACKBEFORE;
			case    ENTER_KEY:

				if(cur_menu == 1){
					v_u32 = mng_network->ipaddr;
					status = Lcd_Set_IpAddr_Menu(NetworkMenu[0], &mng_network->ipaddr);
					if(status == TIME_OUT_RETURN_FLAG){
						return TIME_OUT_RETURN_FLAG;
					}else if(status == SAVERESULT){
						status = check_ipaddr_network_isSame() >> 8;
						printf("check_ipAddr_network result : %#x \n", status);
						if(status){
							sprintf(buf, "The ipaddr & SFP-%.1d's", status&1);
							printf("%s are same network Addr\n", buf);
							mng_network->ipaddr = v_u32;
							Lcd_Pause_Menu(buf, "are same network Addr");
							break;
						}
						set_ip_addr(sysinfo->netif_name, mng_network->ipaddr, mng_network->netmask);
						save_mngnetwork_ipaddr(mng_network->ipaddr);
						//save_Network_parameter_flag = 1;
					}
				}else if(cur_menu == 2){
					status = Lcd_Set_IpAddr_Menu(NetworkMenu[1], &mng_network->netmask);
					if(status == TIME_OUT_RETURN_FLAG){ 
						return TIME_OUT_RETURN_FLAG;
					}else if(status == SAVERESULT){
						set_ip_addr(sysinfo->netif_name, mng_network->ipaddr, mng_network->netmask);
						save_mngnetwork_netmask(mng_network->netmask);
						//save_Network_parameter_flag = 1;
					}
				}else if(cur_menu == 3){
					status = Lcd_Set_IpAddr_Menu(NetworkMenu[2], &mng_network->gateway);
					if(status == TIME_OUT_RETURN_FLAG){
						return TIME_OUT_RETURN_FLAG;
					}else if(status == SAVERESULT){

						if(mng_network->old_gw != mng_network->gateway){
							del_default_gw(mng_network->old_gw);
							set_default_gw(mng_network->gateway);
							mng_network->old_gw = mng_network->gateway;
							save_mngnetwork_gateway(mng_network->gateway);
						}
						//save_Network_parameter_flag = 1;
					}
				}else if(cur_menu == 4) {

					#if 1
					status = Lcd_ConfirmPassword(sysinfo->admin_passwd, 9);
					#else
					status = LcdMenu_Encrypt(8);
					//printf("status1 = %d\n",status);
					#endif
					if(status == TIME_OUT_RETURN_FLAG) return TIME_OUT_RETURN_FLAG;
					else if(status == 1)
					{
						status = Lcd_Set_Mac_Menu(NetworkMenu[3], mng_network->macaddr);
						//printf("status2 = %d\n",status);
						if(status == TIME_OUT_RETURN_FLAG){
							return TIME_OUT_RETURN_FLAG;
						}else if(status == SAVERESULT){
							set_mac_addr(sysinfo->netif_name, mng_network->macaddr);
							save_macaddr(0, mng_network->macaddr);
							//save_Machine_parameter_flag = 1;
						}
					}

				}
				break;
			case    LEFT_KEY:
				break;
			case    UP_KEY:
				if(--cur_menu == 0)
					cur_menu = total_menu;
				break;
			case    RIGHT_KEY:
				//return cur_menu;
				break;
			case    DOWN_KEY:
				if(++cur_menu > total_menu)
					cur_menu = 1;
				break;
			default :
				break;
		}

		

	}
}

u_int Tsoip_NetWork_Menu(int nif_no)
{
	
	u_int v_u32;
	int status;
	int key_value;
	static int cur_menu = 1;
	int total_menu = 4;
	char buf[24];

	static const char *NetworkMenu[] = {
		"M. Net/IP Address",
		"M. Net/Subnet Mask",
		"M. Net/Gateway",
		"M. Net/MAC Address"
	};

	while(1){
		
		if(cur_menu == 1){
			ip_i2strxxx(tsoip_network[nif_no].ipaddr, buf);
		}else if(cur_menu == 2){
			ip_i2strxxx(tsoip_network[nif_no].netmask, buf);
		}else if(cur_menu == 3){
			ip_i2strxxx(tsoip_network[nif_no].gateway, buf);
		}else if(cur_menu == 4) {
			mac_arr2str(tsoip_network[nif_no].macaddr, buf);
		}
		
		LcdDisplay(FIRST_LINE, NetworkMenu[cur_menu - 1]);
		LcdDisplay(SECOND_LINE, buf);

		key_value = GetKeyValue(TIME_OUT_COUNT);

		switch(key_value) {
			case    NO_KEY:
			case 	   KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case    EXIT_KEY:
				return BACKBEFORE;
			case    ENTER_KEY:

				if(cur_menu == 1){
					v_u32 = tsoip_network[nif_no].ipaddr;
					status = Lcd_Set_IpAddr_Menu(NetworkMenu[0], &tsoip_network[nif_no].ipaddr);
					if(status == TIME_OUT_RETURN_FLAG){
						return TIME_OUT_RETURN_FLAG;
					}else if(status == SAVERESULT){
						status = check_ipaddr_network_isSame() >> 8;
						printf("check_ipAddr_network result : %#x \n", status);
						if(status){
							sprintf(buf, "The ipaddr & SFP-%.1d's", status&1);
							printf("%s are same network Addr\n", buf);
							tsoip_network[nif_no].ipaddr = v_u32;
							Lcd_Pause_Menu(buf, "are same network seg");
							break;
						}
						set_tsoip_network(nif_no);
						//save_Network_parameter_flag = 1;
					}
				}else if(cur_menu == 2){
					status = Lcd_Set_IpAddr_Menu(NetworkMenu[1], &tsoip_network[nif_no].netmask);
					if(status == TIME_OUT_RETURN_FLAG){ 
						return TIME_OUT_RETURN_FLAG;
					}else if(status == SAVERESULT){
						set_tsoip_network(nif_no);
						//save_Network_parameter_flag = 1;
					}
				}else if(cur_menu == 3){
					status = Lcd_Set_IpAddr_Menu(NetworkMenu[2], &tsoip_network[nif_no].gateway);
					if(status == TIME_OUT_RETURN_FLAG){
						return TIME_OUT_RETURN_FLAG;
					}else if(status == SAVERESULT){

						
						//save_Network_parameter_flag = 1;
					}
				}else if(cur_menu == 4) {

					#if 1
					status = Lcd_ConfirmPassword(sysinfo->admin_passwd, 9);
					#else
					status = LcdMenu_Encrypt(8);
					//printf("status1 = %d\n",status);
					#endif
					if(status == TIME_OUT_RETURN_FLAG) return TIME_OUT_RETURN_FLAG;
					else if(status == 1)
					{
						status = Lcd_Set_Mac_Menu(NetworkMenu[3], tsoip_network[nif_no].macaddr);
						//printf("status2 = %d\n",status);
						if(status == TIME_OUT_RETURN_FLAG){
							return TIME_OUT_RETURN_FLAG;
						}else if(status == SAVERESULT){
							set_mac_addr(sysinfo->netif_name, tsoip_network[nif_no].macaddr);
							save_macaddr(0, tsoip_network[nif_no].macaddr);
							//save_Machine_parameter_flag = 1;
						}
					}

				}
				break;
			case    LEFT_KEY:
				break;
			case    UP_KEY:
				if(--cur_menu == 0)
					cur_menu = total_menu;
				break;
			case    RIGHT_KEY:
				//return cur_menu;
				break;
			case    DOWN_KEY:
				if(++cur_menu > total_menu)
					cur_menu = 1;
				break;
			default :
				break;
		}

		

	}
}


u_int Device_Temperature_Menu(void )
{
	int key_value, result;
	u_char buf[24],buf_first_line[24];
	int c_menu = 1;
	int total_menu = 4;
	double v_lf;
	int iTemperature_val;


	//printf("c_menu = %d\n",c_menu);
	do {

		//key_response.key_flag = 1;
		
		sprintf(buf_first_line, "Temperature (deg c)");
		if(c_menu == 1){
			iTemperature_val = read_temperature(0);
			sprintf(buf, "ARM : %.2f", (float)iTemperature_val/4);
		}else if(c_menu == 2){
			iTemperature_val = read_temperature(1);
			sprintf(buf, "FPGA : %.2f", (float)iTemperature_val/4);
		}else if(c_menu == 3){
			sprintf(buf, "Upper Limit: %.2f", (float)sysinfo->temperature_high/4);
		}else if(c_menu == 4){
			sprintf(buf, "Lower Limit: %.2f", (float)sysinfo->temperature_low/4);
		}

		
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(500);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 3){
					v_lf = (double)sysinfo->temperature_high/4;
					 result = LcdEditFloatVarible(2, &v_lf, 6, 2, -40.0, 150.0,"Upper Limit: ", "");
					 if(result == SAVERESULT){
						set_temperature_high(v_lf*4);
					 }
				}else if(c_menu == 4){
					v_lf = (double)sysinfo->temperature_low/4;
					 result = LcdEditFloatVarible(2, &v_lf, 6, 2, -40.0, 150.0, "Lower Limit: ", "");
					 if(result == SAVERESULT){
						set_temperature_low(v_lf*4);
					 }
				}

				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
	
}

u_int Device_Power_Supply_Menu(void )
{
	int key_value, result;
	u_char buf[24],buf_first_line[24];
	int c_menu = 1;
	int total_menu = 1;
	double v_lf;

	static char *status_menu[] = {
		"Error",
		"Ok"
	};

	//printf("c_menu = %d\n",c_menu);
	do {

		//key_response.key_flag = 1;

		check_power_supply();

		sprintf(buf_first_line, "Power Supply  1 : %s", status_menu[sysinfo->power_supply[0]]);
		sprintf(buf, "Power Supply  2 : %s", status_menu[sysinfo->power_supply[1]]);

		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(1000);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

	
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
	
}


u_int Display_DateTime_Status_Menu(void )
{
	int key_value;
	u_char buf[24],buf_first_line[24];
	u_int total_menu;
	int c_menu = 1;
	
	struct tm *pst_time;
	
	
	//get_rtc_datetime(NULL);

	do {			
		sprintf(buf_first_line, "Date And Time");

		key_response.key_flag = 1;
		
		pst_time = get_systime();
		
		
		sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d", pst_time->tm_year, pst_time->tm_mon,
			pst_time->tm_mday, pst_time->tm_hour, pst_time->tm_min, pst_time->tm_sec);

		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(1000);
		switch(key_value) {
			case	NO_KEY:
				break;
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}


u_int Setting_DateTime_Menu(void )
{
	int key_value;
	u_char buf[24],buf_first_line[24],buf_val[10],err_second_line[24];
	u_int total_menu;
	int time_out_count;
	int enterkey_press_flag = 0;
	int index;
	struct tm time_temp;
	struct tm *pst_time;
	
	time_out_count = 0;

	//get_rtc_datetime(NULL);
 
	do {			
		sprintf(buf_first_line, "Date And Time");
		if(enterkey_press_flag==0)	{
			pst_time = get_systime();
			sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d", pst_time->tm_year, pst_time->tm_mon,
				pst_time->tm_mday, pst_time->tm_hour, pst_time->tm_min, pst_time->tm_sec);
		}
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		if(enterkey_press_flag==1)
			LcdFlashCursor(SECOND_LINE, index);
		key_value = GetKeyValue(1000);
		switch(key_value) {
			case    NO_KEY:
			case 	   KEY_TIMEOUT:
				time_out_count++;
				if(time_out_count>120)
					return TIME_OUT_RETURN_FLAG;
				else
					break;
			case    EXIT_KEY:
				return EXIT_KEY_PRESS_FLAG;
			case    ENTER_KEY:
				if(enterkey_press_flag==0){
					index = 2;
					enterkey_press_flag = 1;						
				}else{
					enterkey_press_flag = 0;
					memcpy(buf_val,buf,4);
					buf_val[4] = 0x00;
					time_temp.tm_year = atoi(buf_val);
					
					memcpy(buf_val,&buf[5],2);
					buf_val[2] = 0x00;
					time_temp.tm_mon = atoi(buf_val);

					memcpy(buf_val,&buf[8],2);
					buf_val[2] = 0x00;
					time_temp.tm_mday = atoi(buf_val);

					memcpy(buf_val,&buf[11],2);
					buf_val[2] = 0x00;
					time_temp.tm_hour = atoi(buf_val);

					memcpy(buf_val,&buf[14],2);
					buf_val[2] = 0x00;
					time_temp.tm_min = atoi(buf_val);

					memcpy(buf_val,&buf[17],2);
					buf_val[2] = 0x00;
					time_temp.tm_sec = atoi(buf_val);
					
					if(check_tm_valid(&time_temp)!=0){
						enterkey_press_flag = 1;
						sprintf(err_second_line,"Invalid Date Or Time");
					
						Display_Error_Menu(buf_first_line,err_second_line);
						break;
					}

					update_all_board_systime(&time_temp);
					
				}	
				time_out_count = 0;
				break;
			case    LEFT_KEY:
				time_out_count = 0;
				if(enterkey_press_flag==1){
					index--;
					if(index<2)
						index = 18;
					if((index==4)||(index==7)||(index==10)||(index==13)||(index==16))
						index--;
				}
				break;
			case    UP_KEY:
				time_out_count = 0;
				if(enterkey_press_flag==1){
					buf[index]++;
					
					if(index == 5){
						if(buf[index] > 0x31)
							buf[index] = 0x30;
					}else if(index == 8){
						if(buf[index] > 0x33)	
							buf[index] = 0x30;
					}else if(index == 11){
						if(buf[index] > 0x32)	
							buf[index] = 0x30;
					}else if(index == 14){
						if(buf[index] > 0x35)	
							buf[index] = 0x30;
					}else if(index == 17){
						if(buf[index] > 0x35)	
							buf[index] = 0x30;
					}else {
						if(buf[index] > 0x39)	
							buf[index] = 0x30;
					}
				}
				break;
			case    RIGHT_KEY:
				time_out_count = 0;
				if(enterkey_press_flag==1)	{
					index++;
					if(index>18)
						index = 2;
					if((index==4)||(index==7)||(index==10)||(index==13)||(index==16))
						index++;
				}
				break;
			case    DOWN_KEY:
				time_out_count = 0;
				if(enterkey_press_flag==1){
					buf[index]--;
					
					if(index == 5){
						if(buf[index] < 0x30)
							buf[index] = 0x31;
					}else if(index == 8){
						if(buf[index] < 0x30)	
							buf[index] = 0x33;
					}else if(index == 11){
						if(buf[index] < 0x30)	
							buf[index] = 0x32;
					}else if(index == 14){
						if(buf[index] < 0x30)	
							buf[index] = 0x35;
					}else if(index == 17)	{
						if(buf[index] < 0x30)	
							buf[index] = 0x35;
					}else {
						if(buf[index] < 0x30)	
							buf[index] = 0x39;
					}
				}
				break;
	
			default :
				break;
		}
	} while (1);
}

u_int factory_reset_menu(void)
{
	int key_value, result;
	//u_char buf[24],buf_first_line[24];
	int c_menu = 1;
	int total_menu = 1;
		
	//sprintf(buf_first_line, "%s", "Factory Settings");
	
	do {
		result = Lcd_Confirm_Menu("Factory Settings");
		if(result == SAVERESULT){
			printf("****************restore factory settings***********\n");
			factory_setting();
			//reset_slot_boards(0xff);
			system("reboot");
		}else if(result == BACKBEFORE){
			return EXIT_KEY_PRESS_FLAG; 
		}
		
	} while (1);
}

//*
u_int Set_device_sn(void)
{
	int key_value, result, status, temp;
	u_char buf[24],buf_first_line[24];
	int c_menu = 1;
	int total_menu = 1;

	

	c_menu = 1;
	printf("c_menu = %d\n",c_menu);
	
	sprintf(buf_first_line, "%s", "Device SN");
	
	do {

	
		if(c_menu == 1){
			sprintf(buf, "%s", sysinfo->device_sn);
		}
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value) {
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return EXIT_KEY_PRESS_FLAG;
			case	ENTER_KEY:

				if(c_menu == 1){
					result = LcdEditNameString(2, buf, 0, 19);
					if(result == SAVERESULT){
						memcpy(sysinfo->device_sn, buf, 19);
						sysinfo->device_sn[19] = '\0';
						save_devsn(sysinfo->device_sn);
						printf("****************Device SN = %s***************\n", sysinfo->device_sn);
					}
				}

				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu == 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}
//*/

u_int Set_Device_Name(void)
{
	int key_value, result, status, temp;
	u_char buf[24],buf_first_line[24];
	int c_menu = 1;
	int total_menu = 1;

	

	c_menu = 1;
	printf("c_menu = %d\n",c_menu);
	
	sprintf(buf_first_line, "%s", "Device Name");
	
	do {

	
		if(c_menu == 1){
			sprintf(buf, "%s", sysinfo->device_name);
		}
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value) {
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return EXIT_KEY_PRESS_FLAG;
			case	ENTER_KEY:

				if(c_menu == 1){
					result = LcdEditNameString(2, buf, 0, 19);
					if(result == SAVERESULT){
						memcpy(sysinfo->device_name, buf, 19);
						sysinfo->device_name[19] = '\0';
						save_devicename(sysinfo->device_name);
						printf("****************Device Name = %s***************\n", sysinfo->device_name);
					}
				}

				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu == 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}

u_int Set_Web_Account_Menu(void)
{
	int key_value, result = 0;
	u_char buf[24],buf_first_line[24];
	int c_menu = 1;
	int total_menu = 2;

	

	c_menu = 1;
	printf("c_menu = %d\n",c_menu);
	
	sprintf(buf_first_line, "%s", "Web Account");
	
	do {

	
		if(c_menu == 1){
			sprintf(buf_first_line, "%s", "Login Name");
			sprintf(buf, "%s", sysinfo->loginuser);
			LcdDisplay(SECOND_LINE, buf);
		}else if(c_menu == 2){
			sprintf(buf_first_line, "%s", "Login Password");
			sprintf(buf, "%s", sysinfo->loginpasswd);
			LcdDisplay(SECOND_LINE, "**********");
		}
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		//LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value) {
			case	NO_KEY:
			case	KEY_TIMEOUT:
				//return TIME_OUT_RETURN_FLAG;
				return result;
			case	EXIT_KEY:
				return result;
			case	ENTER_KEY:

				if(c_menu == 1){
					result = LcdEditNameString(2, buf, 0, 9);
					if(result == SAVERESULT){
						buf[10] = 0;
						strcpy(sysinfo->loginuser, (const void *)strtrim(buf));
						save_loginuser(sysinfo->loginuser);
						
						printf("****************login Name = %s***************\n", sysinfo->loginuser);
					}
				}else if(c_menu == 2){
					result = LcdEditPasswd(buf_first_line, buf, 0, 9);
					if(result == SAVERESULT){
						buf[10] = 0;
						strcpy(sysinfo->loginpasswd, (const void *)strtrim(buf));						
						save_loginpasswd(sysinfo->loginpasswd);
						printf("****************login password = %s***************\n", sysinfo->loginpasswd);
					}
				}

				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu == 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}

u_int Reset_slot_board_Menu(void)
{
	int key_value, result, status, temp;
	u_char buf[24],buf_first_line[24];
	u_char *subfix = "", prefix = "";
	int c_menu = 1;
	int total_menu = 2;
	int v_s32, i, v_time = 0;
	static int slot = 1;
	static int timeout = 60 * 5;

	//printf("c_menu = %d\n",c_menu);
	do {
		if(sysinfo->slot_status[slot-1] == SLOT_STATUS_NULL){
			subfix = "don't exist";
		}else if(v_time<0){
			subfix = "Timeout";		
		}else{
			subfix = "";
		}
						
		if(c_menu == 1){
			sprintf(buf, "slot : %.1d %s", slot, subfix);
		}else if(c_menu == 2){
			sprintf(buf, "timeout : %.3d sec", timeout);
		}
		
		LcdDisplay(FIRST_LINE, "Reset Module");
		LcdDisplay(SECOND_LINE, buf);

		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					 v_s32 = slot;
					 result = LcdEditIntVarible(2, &v_s32, 1, 1, 7, "slot : ", "");
					 if(result == SAVERESULT){
						slot = v_s32;
						v_s32 = slot-1;
						v_time = timeout;
						if(sysinfo->slot_status[v_s32] == SLOT_STATUS_NULL){
							break;
						}
						reset_slot_board(v_s32);
						sprintf(buf_first_line, "Reseting %.1d Board :", slot);
						LcdDisplay(FIRST_LINE, buf_first_line);
						Lcd_Wait_Menu(SECOND_LINE, i, buf, (sysinfo->slot_info[v_s32].run_flag >= 2), v_time);

					 }
				}else if(c_menu == 2){
					 v_s32 = timeout;
					 result = LcdEditIntVarible(2, &v_s32, 3, 0, 600, "timeout : ", " sec");
					 if(result == SAVERESULT){
						timeout = v_s32;
						printf("reset board timeout : %.3d second\n", timeout);
					 }
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}


u_int Switch_Uart_slot_board_Menu(void)
{
	int key_value, result, status, temp;
	u_char buf[24],buf_first_line[24];
	u_char *subfix = "";
	int c_menu = 1;
	int total_menu = 1;
	int v_s32;
	static int slot = 1;


	//printf("c_menu = %d\n",c_menu);
	do {

		sprintf(buf, "Module %.1d %s", slot, subfix);
		
		LcdDisplay(FIRST_LINE, "Switch USB-RS232");
		LcdDisplay(SECOND_LINE, buf);

		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					 v_s32 = slot;
					 result = LcdEditIntVarible(2, &v_s32, 1, 1, 7, "Module ", "");
					 if(result == SAVERESULT){
						slot = v_s32;
						v_s32 = slot-1;
						if(sysinfo->slot_status[v_s32] == SLOT_STATUS_NULL){
							subfix = "don't exist";
							break;
						}else{
							subfix = "";
						}
						switch_slot_uart(v_s32);
					 }
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}

u_int Switch_Watchdog_Menu(void)
{
	int key_value, result, status, temp;
	u_char buf[24],buf_first_line[24];
	u_char *subfix = "";
	int c_menu = 1;
	int total_menu = 1;
	int v_s32;
	static int slot = 1;


	//printf("c_menu = %d\n",c_menu);
	do {

		LcdDisplay(FIRST_LINE, "Watchdog");
		LcdDisplay(SECOND_LINE, Switch_Menu[sysinfo->watchdog_enable]);
		

		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				 v_s32 = sysinfo->watchdog_enable;
				 result = LcdEditOption(2, &v_s32, 0, 1, Switch_Menu, "", "");
				 if(result == SAVERESULT){
					if(v_s32){
						watchdog_open();
					}else{
						watchdog_close();
					}
				 }
	
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}

u_int Debug_buzzer_Menu(void)
{
	int key_value, result, status, temp;
	u_char buf[24],buf_first_line[24];
	u_char str_temp[20];
	int c_menu = 1;
	int total_menu = 1;
	int v_s32;
	static int enable = 0;

	sprintf(buf_first_line, "%s", "Buzzer");


	//printf("c_menu = %d\n",c_menu);
	do {

		
		/*
		if(c_menu == 1){
			sprintf(buf, "%s", Switch_Menu[enable]);
  		}
		//*/

		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, Switch_Menu[enable]);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					 v_s32 = enable;
					 result = LcdEditOption(2, &v_s32, 0, 1, Switch_Menu, "", "");
					 if(result == SAVERESULT){
						enable = v_s32;
						if(enable){
							gpio_set(BUZZER_GPIO);
						}else{
							gpio_clean(BUZZER_GPIO);
						}
						printf("turn %s buzzer \n", Switch_Menu[enable]);
					 }
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}

u_int Debug_gpio_Menu(void)
{
	int key_value, result, status;
	u_char buf[24],buf_first_line[24];
	static int c_menu = 1;
	int total_menu = 6;
	int v_s32;
	static int gpio_idx = 0;
	int value = 0;

	static const char *MenuList[] = {
		"Select gpio : ",
		"get gpio",
		"set gpio",
		"clean gpio",
		"free gpio",
		"init gpio"
	};

	
	sprintf(buf_first_line, "Debug GPIO");

	//printf("c_menu = %d\n",c_menu);
	do {

		
		
	
		
		//*
		if(c_menu == 1){
			sprintf(buf, "%s%.3d", MenuList[c_menu-1], gpio_idx);
  		}else if(c_menu == 2){
			sprintf(buf, "%s %d : %.1d", MenuList[c_menu-1], gpio_idx, value);
  		}else{
			sprintf(buf, "%s %d", MenuList[c_menu-1], gpio_idx);
  		}
		//*/
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					 v_s32 = gpio_idx;
					 result = LcdEditIntVarible(2, &v_s32, 3, 0, 127, MenuList[0], "");
					 if(result == SAVERESULT){
						gpio_idx = v_s32;
						printf(" gpio is selected %d\n", gpio_idx);
					 }
				}else if(c_menu == 2){
					value = gpio_get(gpio_idx);
					printf(" %d gpio get input value : %d\n", gpio_idx, value);
				}else if(c_menu == 3){
					gpio_set(gpio_idx);
					printf(" %d gpio set output value : 1\n", gpio_idx);
				}else if(c_menu == 4){
					gpio_clean(gpio_idx);
					printf(" %d gpio set output value : 0\n", gpio_idx);
				}else if(c_menu == 5){
					gpio_free(gpio_idx);
					printf(" free %d gpio \n", gpio_idx);
				}else if(c_menu == 6){
					gpio_init(gpio_idx, "test_gpio");
					printf(" request %d gpio \n", gpio_idx);
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}

u_int set_fan_tempctrl_menu(void)
{
	int key_value, result, status;
	u_char buf[24],buf_first_line[24];
	static int c_menu = 1;
	int total_menu = 1;
	int v_s32 = 1;
	int speed = 0;

	LcdDisplay(FIRST_LINE, "Fan Temperature Ctrl");
	
	do {

		if(sysinfo->fan_tempctrl){
			v_s32 = 1;
		}else{
			v_s32 = 0;
		}
					
		//*
		if(c_menu == 1){
			sprintf(buf, "%s", Switch_Menu[v_s32]);
  		}
		//*/
		
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					
					status = LcdEditOption(2, &v_s32, 0, 1, Switch_Menu, "", "");
					if(SAVERESULT == status){
						set_fan_tempctrl(v_s32);
					}
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);

}

u_int set_fan_speed_menu(void)
{
	int key_value, result, status;
	u_char buf[24],buf_first_line[24];
	static int c_menu = 1;
	int total_menu = 1;
	int v_s32 = 1;
	int speed = 0;

	speed = sysinfo->fan_pwm*10;

	LcdDisplay(FIRST_LINE, "Set Fan Speed : ");
	
	do {
		
		//*
		if(c_menu == 1){
			sprintf(buf, "%.5d", speed);
  		}
		//*/
		
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					 v_s32 = speed;
					 result = LcdEditIntVarible(2, &v_s32, 5, 0, 20000, "", "");
					 if(result == SAVERESULT){
						speed = v_s32;
						set_fans_speed(speed);
						printf(" set fun speed %d\n", speed);
					 }
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);

}


u_int show_fan_speed_menu(void)
{
	int key_value, result, status;
	u_char buf[24],buf_first_line[24];
	static int c_menu = 1;
	int total_menu = 1;
	int v_s32 = 1;
	int speed = 0;

	total_menu = (sysinfo->fan_qty+1)/2;

	do {
		
		//*
		v_s32 = (c_menu-1)*2;
		sprintf(buf_first_line, "Fan %d : %.5d", v_s32+1, sysinfo->fan_speed[v_s32]);

		v_s32++;
  		if(v_s32<sysinfo->fan_qty){	
			sprintf(buf, "Fan %d : %.5d", v_s32+1, sysinfo->fan_speed[v_s32]);
		}else{
			buf[0] = 0;
		}
		//*/
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(500);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				break;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
		
	} while (1);

}


u_int Debug_fan_Menu(void)
{
	int key_value, result, status;
	u_char buf[24],buf_first_line[24];
	static int c_menu = 1;
	int total_menu = 3;
	int v_s32;

	static const char *MenuList[] = {
		"Show Fan Speed    ->",
		"Set Fan TempCtrl  ->",
		"Set Fan Speed     ->"
	};

	
	while(1) {
		
		if(sysinfo->fan_tempctrl){
			total_menu = 2;
		}else{
			total_menu = 3;
		}
		
		result = LcdMenux(&c_menu, total_menu, "Fan", MenuList);
		switch(result) {

			case BACKBEFORE: // exit
			case TIME_OUT_RETURN_FLAG: // time out
				return TIME_OUT_RETURN_FLAG;
			case 1:
				status = show_fan_speed_menu();
				break;
			case 2: 
				status = set_fan_tempctrl_menu();
				break;
			case 3: 
				status = set_fan_speed_menu();
				break;
			
		}
		
		if(status==TIME_OUT_RETURN_FLAG)
			return status;
	} 
	

	//printf("c_menu = %d\n",c_menu);
	
}

u_int Debug_Int_modify_Menu(void)
{
	int key_value, result;
	u_char buf[24],buf_first_line[24];
	static int c_menu = 1;
	int total_menu = 5;
	static int intval = 0;
	int v_s32 = 0;

	
	sprintf(buf_first_line, "Edit Int");

	//printf("c_menu = %d\n",c_menu);
	do {
		if(c_menu == 1){
			intval = -2031;
			sprintf(buf, "-+ : %.4d", intval);
		}else if(c_menu == 2){
			intval = -2031;
			sprintf(buf, "-- : %.4d", intval);
		}else if(c_menu == 3){
			intval = 2031;
			sprintf(buf, "++ : %.4d", intval);
		}else if(c_menu == 4){
			intval = 2031;
			sprintf(buf, "0+ : %.4d", intval);
		}else if(c_menu == 5){
			intval = -2031;
			sprintf(buf, "-0 : %.4d", intval);
		}
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					 v_s32 = intval;
					 result = LcdEditIntVarible(2, &v_s32, 4, -4271, 8153, "-+ : ", "");
					 if(result == SAVERESULT){
						intval = v_s32;

						printf(" set ........ intval %d\n", intval);
					 }
				}else if(c_menu == 2){
					 v_s32 = intval;
					 result = LcdEditIntVarible(2, &v_s32, 4, -4271, -1536, "-- : ", "");
					 if(result == SAVERESULT){
						intval = v_s32;

						printf(" set ........ intval %d\n", intval);
					 }
				}else if(c_menu == 3){
					 v_s32 = intval;
					 result = LcdEditIntVarible(2, &v_s32, 4, 1536, 8153, "++ : ", "");
					 if(result == SAVERESULT){
						intval = v_s32;

						printf(" set ........ intval %d\n", intval);
					 }
				}else if(c_menu == 4){
					 v_s32 = intval;
					 result = LcdEditIntVarible(2, &v_s32, 4, 0, 8153, "0+ : ", "");
					 if(result == SAVERESULT){
						intval = v_s32;

						printf(" set ........ intval %d\n", intval);
					 }
				}else if(c_menu == 5){
					 v_s32 = intval;
					 result = LcdEditIntVarible(2, &v_s32, 4, -4271, 0, "-0 : ", "");
					 if(result == SAVERESULT){
						intval = v_s32;

						printf(" set ........ intval %d\n", intval);
					 }
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}


u_int Debug_float_modify_Menu(void)
{
	int key_value, result, status;
	u_char buf[24],buf_first_line[24];
	static int c_menu = 1;
	int total_menu = 5;
	static double lfval = 0;
	double v_lf = 0;

	
	sprintf(buf_first_line, "Edit Float");

	//printf("c_menu = %d\n",c_menu);
	do {
		if(c_menu == 1){
			lfval = -320.31;
			sprintf(buf, "-+ : %.2f", lfval);
		}else if(c_menu == 2){
			lfval = -320.31;
			sprintf(buf, "-- : %.2f", lfval);
		}else if(c_menu == 3){
			lfval = 320.31;
			sprintf(buf, "++ : %.2f", lfval);
		}else if(c_menu == 4){
			lfval = 320.31;
			sprintf(buf, "0+ : %.2f", lfval);
		}else if(c_menu == 5){
			lfval = -320.31;
			sprintf(buf, "-0 : %.2f", lfval);
		}
		
		LcdDisplay(FIRST_LINE, buf_first_line);
		LcdDisplay(SECOND_LINE, buf);
		
		key_value = GetKeyValue(TIME_OUT_COUNT);
		switch(key_value){
			case	NO_KEY:
			case	KEY_TIMEOUT:
				return TIME_OUT_RETURN_FLAG;
			case	EXIT_KEY:
				return BACKBEFORE;
			case	ENTER_KEY:

				if(c_menu == 1){
					 v_lf = lfval;
					 result = LcdEditFloatVarible(2, &v_lf, 6, 2, -642.71, 581.53, "-+ : ", "");
					 if(result == SAVERESULT){
						lfval = v_lf;

						printf(" set ........ lfval %d\n", lfval);
					 }
				}else if(c_menu == 2){
					 v_lf = lfval;
					 result = LcdEditFloatVarible(2, &v_lf, 6, 2, -642.71, -215.36, "-- : ", "");
					 if(result == SAVERESULT){
						lfval = v_lf;

						printf(" set ........ lfval %d\n", lfval);
					 }
				}else if(c_menu == 3){
					 v_lf = lfval;
					 result = LcdEditFloatVarible(2, &v_lf, 6, 2, 215.36, 581.53, "++ : ", "");
					 if(result == SAVERESULT){
						lfval = v_lf;

						printf(" set ........ lfval %d\n", lfval);
					 }
				}else if(c_menu == 4){
					 v_lf = lfval;
					 result = LcdEditFloatVarible(2, &v_lf, 6, 2, 0, 581.53, "0+ : ", "");
					 if(result == SAVERESULT){
						lfval = v_lf;

						printf(" set ........ lfval %d\n", lfval);
					 }
				}else if(c_menu == 5){
					 v_lf = lfval;
					 result = LcdEditFloatVarible(2, &v_lf, 6, 2, -642.71, 0, "-0 : ", "");
					 if(result == SAVERESULT){
						lfval = v_lf;

						printf(" set ........ lfval %d\n", lfval);
					 }
				}
				
				break;
			case	LEFT_KEY:
				break;
			case	UP_KEY:
				if(--c_menu <= 0)
					c_menu = total_menu;
				break;
			case	RIGHT_KEY:
				break;
			case	DOWN_KEY:
				if(++c_menu > total_menu)
					c_menu = 1;
				break;
			default :
				break;
		}
	} while (1);
}

u_int Debug_Tools_Menu(void)
{
	int result, status;
	int total_menus = 3;
	static u_int cur_menu = 1;

	static const char *DebugToolsMenuList[] = {
		"Debug GPIO        ->",
		"Edit Int          ->",
		"Edit Float        ->",
		
	};

	while(1) {
		result = LcdMenux(&cur_menu, total_menus, "Debug Tools", DebugToolsMenuList );
		switch(result){

			case BACKBEFORE: // exit
				return BACKBEFORE;
			case TIME_OUT_RETURN_FLAG: // time out
				return TIME_OUT_RETURN_FLAG;

			case 1: 
				status = Debug_gpio_Menu();
				break;
			case 2: 
				status = Debug_Int_modify_Menu();
				break;
			case 3:
				status = Debug_float_modify_Menu();
				break;

		}
		if(status==TIME_OUT_RETURN_FLAG)
			return TIME_OUT_RETURN_FLAG;
	}
}

u_int Admin_Privileges_Menu(void)
{
	int result, status;
	int total_menus = 8;
	static u_int cur_menu = 1;
	int v_s32;

	static const char *MenuList[] = {	
		"Admin Password    ->",
		"Web Account       ->",
		"Device SN         ->",
		"Factory Settings  ->",
		"M. Net/MAC Address",
		"TSoIP.1/MAC Address",
		"TSoIP.2/MAC Address"	,
		"Debug Tools       ->"
	};

	result = Lcd_ConfirmPassword(sysinfo->admin_passwd, 9);
	if(result != 1)
		return BACKBEFORE;
	
	while(1) {
		result = LcdMenux(&cur_menu, total_menus, "Admin Privileges", MenuList );
		switch(result){

			case BACKBEFORE: // exit
				return BACKBEFORE;
			case TIME_OUT_RETURN_FLAG: // time out
				return TIME_OUT_RETURN_FLAG;
			case 1: 
				v_s32 = sysinfo->admin_passwd;
				 result = LcdEditIntVarible(2, &v_s32, 9, 0, 999999999,"", "");
				 if(result == SAVERESULT){
					sysinfo->admin_passwd = v_s32;
					save_adminpasswd(sysinfo->admin_passwd);
					printf(" set system admin passwd : %.9d\n", sysinfo->admin_passwd);
				 }
				break;
			case 2: 
				status = Set_Web_Account_Menu();
				if(status == SAVERESULT){
					//set_web_user(NULL, sysinfo->loginuser, sysinfo->loginpasswd);
				}
				break;
			case 3: 
				status = Set_device_sn();
				break;
			case 4: 
				status = factory_reset_menu();
				break;
			case 5: 
				status = Lcd_Set_Mac_Menu(MenuList[result-1], mng_network->macaddr);
				if(status == SAVERESULT){
					set_mac_addr(sysinfo->netif_name, mng_network->macaddr);
					save_macaddr(0, mng_network->macaddr);
				}
				break;
			case 6: 
				status = Lcd_Set_Mac_Menu(MenuList[result-1], tsoip_network[0].macaddr);
				if(status == SAVERESULT){
					set_mac_addr(tsoip_network[0].nif_name, tsoip_network[0].macaddr);
					save_macaddr(1, tsoip_network[0].macaddr);
				}
				break;
			case 7: 
				status = Lcd_Set_Mac_Menu(MenuList[result-1], tsoip_network[1].macaddr);
				if(status == SAVERESULT){
					set_mac_addr(tsoip_network[1].nif_name, tsoip_network[1].macaddr);
					save_macaddr(2, tsoip_network[1].macaddr);
				}
				break;
			case 8: 
				status = Debug_Tools_Menu();
				break;

				
				
		}
		if(status==TIME_OUT_RETURN_FLAG)
			return TIME_OUT_RETURN_FLAG;
	}
}

void MainMenu(void)
{
	int result, status;
	static int cur_menu;
	int total_menu = 4;

	
	static const char *MainMenuList[] = {
		"Manage NetWork    ->",
		"RTC Time          ->",
		"Watchdog          ->",
		"Temperature       ->",
		"Fan               ->",
		"Power Supply      ->",
		"Device Name       ->",
		"Switch USB-RS232  ->",
		"Reset Module      ->",
		"Admin Privileges  ->",
		"Version             ->"
	};

	total_menu = sizeof(MainMenuList)/sizeof(char *);

	cur_menu = 1;
	while(1) {
		
		result = LcdMenux(&cur_menu, total_menu, "Main Menu", MainMenuList);
		switch(result) {

			case BACKBEFORE: // exit
			case TIME_OUT_RETURN_FLAG: // time out
				return;
			case 1:
				status = NetWork_Menu();
				break;
			case 2: 
				status = Setting_DateTime_Menu();
				break;
			case 3:
				status = Switch_Watchdog_Menu();
				break;
			case 4: 
				status = Device_Temperature_Menu();
				break;
			case 5: 
				status = Debug_fan_Menu();
				break;
			case 6: 
				status = Device_Power_Supply_Menu();
				break;
			case 7: 
				status = Set_Device_Name();
				break;
			case 8: 
				status = Switch_Uart_slot_board_Menu();
				break;
			case 9: 
				status = Reset_slot_board_Menu();
				break;
			case 10: 
				status = Admin_Privileges_Menu();
				break;
			case 11: 
				status = Version_Menu();
				break;
			
		}
		
		if(status==TIME_OUT_RETURN_FLAG)
			return;
	} 

}

#ifdef USE_CMD_LINE
int debug_led(int argc, char *argv[])
{
	int i;
	int color;

	if(argc<3){
		return -1;
	}

	color = atoi(argv[2]);

	if(strcmp("a", argv[1]) == 0){
		Led_Alarm_Set(color);
	}else if(strcmp("w", argv[1]) == 0){
		Led_Warning_Set(color);
	}else{
		i = atoi(argv[1]);
		Extend_Led_Set(i, color);
	}
	

	return 0;
}
#endif

void *Lcd(void *arg)
{
	int key_value;

	unsigned char buf1[24],buf2[24];

	int i = 0, retval;

	

	lcd_brightness(3);
	
	memset(buf1,0x20,24);
	memset(buf2,0x20,24);

	
	LcdDisplay(FIRST_LINE, "Device");
		
	//*
	do{

		SPRINT_WAIT_STR(buf2, "Initializing ",i++);
		LcdDisplay(SECOND_LINE, buf2);
		
		if(i>4){i=0;}
		
		if(sysinfo->run_flag>4){
			break;
		}

		sleep(1);
	}while(1);
	//*/

	
	do {	
		

		LcdDisplay(FIRST_LINE, sysinfo->device_name);
		LcdDisplay(SECOND_LINE, ip_i2str(mng_network->ipaddr, buf2));
		
		key_value = GetKeyValue(2500);
#if 0
			if (key_value != ENTER_KEY){// && key_value != KEY_TIMEOUT && key_value != NO_KEY) {
				continue;
			}
#else
			if (key_value != KEY_TIMEOUT && (key_value < 0 && (key_value > 6))) {
				continue;
			}
#endif
		


		if (key_value == ENTER_KEY){
			lcd_brightness(3);
			printf("----ENTER_KEY---------------\n");
			MainMenu();
			//NetWork_Menu();
			lcd_brightness(0);

		}else if(key_value == EXIT_KEY){;
			printf("----EXIT_KEY---------------\n");

		}else if(key_value == LEFT_KEY){
			printf("----LEFT_KEY-------\n");
		}else if(key_value == RIGHT_KEY){
			printf("----RIGHT_KEY-------\n");
		}else if(key_value == DOWN_KEY){
			printf("----DOWN_KEY-------\n");

			
		}else if(key_value == UP_KEY){

			printf("----UP_KEY-------\n");

			
		}

		sched_yield();
		usleep(100000);

	} while (1);

	return arg;
}

