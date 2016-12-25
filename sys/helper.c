#include <helper.h>
#include <time.h>

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

/* Print the Judy Array contents */
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

/* count the Judy Array contents */
int countarray(Pvoid_t a_tocount) {

	char index[10000];
	Word_t *value;
	int count=0;

	strcpy(index, "");
	JSLF(value, a_tocount, index);	

	while (value != NULL) {
		count++;	
	    JSLN(value, a_tocount, index);   // get next string
   	}

	return count;
}

/* Create intrusion records for insider threat */
int createITintrecords(char *fndatapath, char *fpdatapath){

	FILE	*fndatafp, *fpdatafp;
	char	*temp, fpline[MAXLINELENGTH], *fnline, *strval, roleindex[2], *newrolechar;
	short	newrole, numroles;
	Word_t	*rolecount;

	/* open the fp data file for reading */
	if ((fpdatafp = fopen(fpdatapath, "r")) == NULL) {
		printf("\n Error opening false positives data file = %s \n", fpdatapath);
		return ERROR;
	}

	/* open the fn data file for writing */
	if ((fndatafp = fopen(fndatapath, "w")) == NULL) {
		printf("\n Error creating false negatives data file = %s \n", fndatapath);
		return ERROR;
	}

	/*Browse through the false positives data line by line*/
	while(fgets(fpline, MAXLINELENGTH, fpdatafp) != NULL) {

			fnline = (char *)malloc(MAXLINELENGTH*sizeof(char));
			newrolechar = (char *)malloc(2*sizeof(char));

			temp = strdup(fpline);	

			/* Get the original Role id */	
			strval = (char *)strtok(temp, delimtab);		

			numroles = countarray(Rolecountarray);
			
			srand(time(NULL));

			do
				newrole = rint((numroles-1)*((double)rand()/(double)RAND_MAX));
			while (newrole == atoi(strval));
			
			sprintf(newrolechar,"%d",newrole);

			strcpy(fnline, newrolechar);

			while ( (strval = (char *)strtok(NULL, delimtab)) != NULL ) {
				strcat(fnline, delimtab);
				strcat(fnline, strval);
			}

			fputs(fnline, fndatafp);

			free(fnline);
			free(newrolechar);
	}

	fclose(fpdatafp);
	fclose(fndatafp);

	return SUCCESS;
}
