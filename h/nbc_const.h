const char testcasesfilepath[] = "//home//akamra//IDRBAC//nbc//testcases//testcasesfiles.txt";
const char delimtab[] = {'\t','\0'};
const char delimspace[] = " ";
int  numtrngrecords;
int  numattributes;
int  numfprecords;
int  fpcount;
int  fncount;
int  numfnrecords;

FILE	*specsdatafp;
Pvoid_t	Rolecountarray = (Pvoid_t)NULL;
Pvoid_t	Attrcountarray = (Pvoid_t)NULL; 
Pvoid_t Columncountarray = (Pvoid_t)NULL;

#define MAXLINELENGTH 100000
#define ATTRINDEXSIZE_cs 20
#define ATTRINDEXSIZE_f 10000
#define SUCCESS	1
#define ERROR	-1

