#ifndef KERMIT_IO_H
#define KERMIT_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../third_party/ekermit/kermit.h"

int kermit_io_readpkt (struct k_data * k, unsigned char *p, int len);
int kermit_io_tx_data(struct k_data * k, unsigned char *p, int n);
int kermit_io_inchk(struct k_data * k);
long kermit_io_filesize(const char *filename);
int kermit_io_openfile(struct k_data * k, unsigned char * s, int mode);
int kermit_io_readfile(struct k_data * k);
int kermit_io_closefile(struct k_data * k, unsigned char c, int mode);
int kermit_io_init_string(void);

#ifdef __cplusplus
}
#endif

#endif
