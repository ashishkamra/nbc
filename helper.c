#include <helper.h>

/* get atmost n characters from iop */
char *myfgets(char *s, int n, FILE *iop) {
	register int c;
	register char *cs;

	cs = s;
	
	while (--n > 0 && (c = getc(iop)) != EOF) {
		if ((*cs++ = c) == '\n') {
			*cs--;
			break;
		}
	}

	*cs = '\0';

	
	return (c == EOF && cs == s) ? NULL : s;

}

void printarray(Pvoid_t a_toprint) {

	char index[10000];
	Word_t *value, Bytes;

	strcpy(index, "");
	JSLF(value, a_toprint, index);	

	while (value != NULL) {
            printf("Index = %s, count = %d \n", index, *value);
	    JSLN(value, a_toprint, index);   // get next string
   	}
}
