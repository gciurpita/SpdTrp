#ifndef PC_READ_H
# define PC_READ_H

enum {
    EE_Scale = 0,
    EE_Dist  = 2,
};

uint16_t eeRead16 (int adr);
void     pcRead (void);

#endif
