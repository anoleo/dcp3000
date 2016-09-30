#ifdef	__SIM_ENCRYPT_H__
#define	__SIM_ENCRYPT_H__

#include <sys/types.h>

int str_encrypt(u_char *ciphertext, u_char *plaintext);
int str_decrypt(u_char *plaintext, u_char *ciphertext);

int data_encrypt(u_char *ciphertext, u_char *plaintext, int textlen);
int data_decrypt(u_char *plaintext, u_char *ciphertext, int textlen);

void init_sim_encrypt(void);

#endif
