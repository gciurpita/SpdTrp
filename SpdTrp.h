#ifndef SPD_TRP_H
# define SPD_TRP_H

extern const char *Version;
extern int   scale;
extern float distX10Inch;

#define SizeS   80
extern char s [SizeS];

void  setConversion (void);
#endif
