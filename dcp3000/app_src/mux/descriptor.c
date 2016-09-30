/*****************************************************************************
 *descriptor.c:parse and encode mpeg2 descrpitor.
 *----------------------------------------------------------------------------
 * (c) 2004-2005 mx3000
 * Authors: Davy.Lee<pbi-lz@pbi-china.com>
 *
 * change log :
 *		2004-9-1 16:12  v1.0	start to write
 *
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "descriptor.h"
#include "psi_si.h"

 /*allocate descriptor structure and initial it */
descriptor_t *createDescriptor(void)
{
 	descriptor_t *descriptor;

 	descriptor = (descriptor_t *)malloc(sizeof(descriptor_t));
 	if(descriptor == NULL)
 		printf("allocate descriptor_t space fail\n");

 	descriptor->parse_desc_flag = 0;
 	descriptor->link = NULL;
 	return (descriptor_t *)descriptor;
}

void freeAllDescriptor(descriptor_t *head_descriptor)
{
	descriptor_t *descriptor_current, *descriptor_next;

	descriptor_current = (descriptor_t *)(head_descriptor);

	while(descriptor_current != NULL) {
		descriptor_next = descriptor_current->link;
		free(descriptor_current);
		descriptor_current = descriptor_next;
	}
}


char Ac3DesTag = 0;
char S302DesTag = 0;
int streamTypeDes;
int parseDescriptor(descriptor_t *descriptor)
{
	u_char *buf;
	u_int len;
	int i;

	buf = descriptor->desc_data;

	switch(descriptor->desc_tag) {
		case	SERVICE_DESTAG:
			descriptor->parse_desc_flag = 1;
			descriptor->desc_info.descService.service_type = buf[0];
			descriptor->desc_info.descService.service_provider_name_length = buf[1];
			len = buf[1];
			buf += 2;
			i = (len >= MAX_PROGRAM_NAME_LEN) ? (MAX_PROGRAM_NAME_LEN-1) : len;
			bcopy(buf, descriptor->desc_info.descService.service_provider_name, i);
			descriptor->desc_info.descService.service_provider_name[i] = '\0';
			buf += len;
			descriptor->desc_info.descService.service_name_length = buf[0];
			len = buf[0];
			i = (len >= MAX_PROGRAM_NAME_LEN) ? (MAX_PROGRAM_NAME_LEN-1) : len;
			buf += 1;
			bcopy(buf, descriptor->desc_info.descService.service_name, i);
			descriptor->desc_info.descService.service_name[i] = '\0';
			//printf("name is %s\n", descriptor->desc_info.descService.service_name);
			return 1;
		case 	ISO_639_LANGUAGE_DESTAG:
			descriptor->parse_desc_flag = 1;
			len = descriptor->desc_len;
			i = 0;
			while(len > 3) {
				bcopy(buf, descriptor->desc_info.descISO639List.ISO_639_language_code[i], 3);
				i++;
				len -= 3;
				buf +=3;
			}
			descriptor->desc_info.descISO639List.audio_type = *buf;
			return 2;
		case	CA_DESTAG:
			descriptor->parse_desc_flag = 1;
			descriptor->desc_info.descCA.CA_system_ID = ((buf[0] << 8) & 0xff00) | (buf[1] & 0xff);
			//descriptor->desc_info.descCA.reserved = (buf[2] >> 5) & 0x7;
			descriptor->desc_info.descCA.CA_PID = ((buf[2] << 8) & 0x1f00) | (buf[3] & 0xff);
			return 3;
		case	AC3_DESTAG:
			descriptor->parse_desc_flag = 1;
			Ac3DesTag = 1;
			return 4;
		case	VBI_DATA_DESTAG:
			//descriptor->parse_desc_flag = 1;
			len = descriptor->desc_len;
			i = 0;
			while(i < len){
				if((buf[i] == 0xfe) || (buf[i] == 0xfb))	// 0xfb from neil program
					return 6;
				i++;
				i += buf[i];
				i++;
			}
			return 5;

		case	FORMAT_IDENTIFIER:
			if((buf[0] == 'B') && (buf[1] == 'S') && (buf[2] == 'S') && (buf[3] == 'D')){
				if((streamTypeDes==0x06)||(streamTypeDes==0x83)){
				//printf("stream type is pcm\n");
				S302DesTag = 1;
			}
			}
			if((buf[0] == 'A') && (buf[1] == 'C') && (buf[2] == '-') && (buf[3] == '3')){
				//printf("stream type is ac3\n");
				if((streamTypeDes==0x06)||(streamTypeDes==0x81)){
				Ac3DesTag = 1;
			}
			}
				
			return 6;
		
		default 				    :
			return 0;
	}
}



