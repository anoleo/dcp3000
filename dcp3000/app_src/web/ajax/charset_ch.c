#include "charset_ch.h" 

static int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen) 
{ 
iconv_t cd; 
int rc; 
char **pin = &inbuf; 
char **pout = &outbuf; 
printf("###################\n");
#if 0
cd = iconv_open(to_charset,from_charset); 
if (cd==0) return -1; 
memset(outbuf,0,outlen); 
if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1; 
iconv_close(cd); 
#endif
return 0; 
} 

int charset_r_fun(charset_par_t * p_charset_par) {
	return code_convert(
		p_charset_par->intype,
		p_charset_par->outtype,
		p_charset_par->inbuf,
		p_charset_par->inlen,
		p_charset_par->outbuf,
		p_charset_par->outlen
		); 
}
int charset_w_fun(charset_par_t * p_charset_par) {
	return code_convert(
		p_charset_par->outtype,
		p_charset_par->intype,
		p_charset_par->inbuf,
		p_charset_par->inlen,
		p_charset_par->outbuf,
		p_charset_par->outlen
		); 
}

