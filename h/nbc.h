#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <Judy.h>

/* Macros */
#define MAXLINELENGTH 100000
#define ATTRINDEXSIZE_cs 1000
#define ATTRINDEXSIZE_f 100000
#define SUCCESS	1
#define ERROR	-1
#define m	1

/* Global Constants */
extern const char delimtab[];
extern const char delimspace[];

/* Global Variables */
extern unsigned long  numtrngrecords, numattributes, numfprecords, fpcount, fncount, numfnrecords;

extern FILE	*specsdatafp;
extern Pvoid_t	Rolecountarray, Attrcountarray, Columncountarray, Attrcountarray_data;

/* External Functions used */
extern int trainnbc(char, char *); // ttype, tdatapath
extern int detectfp(char, char *); // ttype, fpdatapath
extern int detectfn(char, char *, char *); // ttype, fndatapath, fpdatapath
