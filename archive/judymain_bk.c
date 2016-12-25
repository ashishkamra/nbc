#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <Judy.h>
#include <string.h>
#include <nbc_const.h>
#include <nbc_dsjudy.h>

int main(int argc, char **argv) {

	FILE *tcasesfp, *testcasefp;
	char tcasepath[100], temp[100], *tcasevalues[5], tdatapath[100], specsdatapath[100], fndatapath[100], fpdatapath[100];
	char ttype, specline[MAXLINELENGTH];
	int count, columnindex=0;
	uint8_t	*numcolumns, *columnindexbuf;
	Word_t	*columnval;
	double trngtime_ms, detecttime_ms;
	clock_t trngstarttime, trngendtime, detectstarttime, detectendtime;

	if ((tcasesfp = fopen(testcasesfilepath, "r")) == NULL) {
		printf("\n Error opening test cases file \n");
		return -1;
	}

	//sleep(1);

	/* browse through all the test cases */
	while (myfgets(tcasepath, sizeof(tcasepath), tcasesfp) != NULL) {

		/* Re-initialise all loop variables */
		count = 0;
		numtrngrecords = 0;
		numfprecords = 0;
		fpcount = 0;
		numattributes = 0;
		Rolecountarray = (Pvoid_t)NULL;
		Attrcountarray = (Pvoid_t)NULL;
		Columncountarray = (Pvoid_t)NULL;

		if ((testcasefp = fopen(tcasepath, "r")) == NULL) {
			printf("\n Error opening test case %s file \n", tcasepath);
			return -1;
		}

		while (myfgets(temp, sizeof(temp), testcasefp) != NULL) {		
			tcasevalues[count] = (char *)malloc(sizeof(temp)*sizeof(char));	
			strcpy(tcasevalues[count++], temp);
		}

		/* First Value = Triplet type */
		ttype = tcasevalues[0][0];
		strcpy(tdatapath, tcasevalues[1]);
		strcpy(specsdatapath, tcasevalues[2]);
		strcpy(fpdatapath, tcasevalues[3]);
		strcpy(fndatapath, tcasevalues[4]);

		if ((specsdatafp = fopen(specsdatapath, "r")) == NULL) {
			printf("\n Error opening specifications data file = %s \n", specsdatapath);
			return -1;
		}
	
		/* Populate the column count array from the first line of specs file */
		fgets(specline, MAXLINELENGTH, specsdatafp);			
		numcolumns = (char *)strtok(specline, delimspace);
	
		while (numcolumns != NULL) {
			columnindexbuf = (char *)malloc(10*sizeof(char));	
			sprintf(columnindexbuf, "%d", columnindex++);
			JSLI(columnval, Columncountarray, columnindexbuf);
			*columnval = atoi(numcolumns);
			numcolumns = (char *)strtok(NULL, delimspace);
		}

	//	printf("%s \n", tcasepath);
	
		/*training phase*/

		trngstarttime = clock();
		trainnbc(ttype, tdatapath);
		trngendtime = clock();

		trngtime_ms = ((double)(trngendtime-trngstarttime))/CLOCKS_PER_SEC;
		trngtime_ms = 1000*trngtime_ms;
//		printf("\n Triplet type %c, %d attributes, %d triplets  trngtime = %4.6f ms \n", ttype, numattributes, numtrngrecords, trngtime_ms);

		/* training phase */

		/* detect false positives */
		detectstarttime = clock();
		detectfp(ttype, fpdatapath);
		detectendtime = clock();

		detecttime_ms = ((double)(detectendtime-detectstarttime))/CLOCKS_PER_SEC;
		detecttime_ms = (1000*detecttime_ms)/numfprecords;

//		printf("\n Triplet type %c, %d attributes detection time/triplet = %4.6f ms \n",ttype, numattributes, detecttime_ms);

		/* detect false positives */

		/* detect false negatives */
		detectstarttime = clock();
		detectfn(ttype, fndatapath);
		detectendtime = clock();

		detecttime_ms = ((double)(detectendtime-detectstarttime))/CLOCKS_PER_SEC;
		detecttime_ms = (1000*detecttime_ms)/numfnrecords;

//		printf("\n Triplet type %c, %d attributes detection time/triplet = %4.6f ms \n",ttype, numattributes, detecttime_ms);

		/* detect false negatives */

	}
}

/* get atmost n characters from iop*/
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

/* Train the classfier */
int trainnbc(char ttype, char *trngdatapath) {

	FILE	*trngdatafp;
	char	*temp, trngline[MAXLINELENGTH];
	Word_t 	*Rolecount=0, *attrcount=0, *Cmdcount=0, *columnval;
	short 	attrid, i;
	uint8_t	*Roleindex, *attr, *attrindex, *attridbuf, *Cmdindex, *Cmdval;		        	
	char	index[3];

	if ((trngdatafp = fopen(trngdatapath, "r")) == NULL) {
		printf("\n Error opening training data file = %s \n", trngdatapath);
		return -1;
	}

	/*Browse through the training data line by line*/
	while(fgets(trngline, MAXLINELENGTH, trngdatafp) != NULL) {

		numtrngrecords++;				/* counting no. of trng records */

		temp = strdup(trngline);	

		/* Populate Role count array */
		Roleindex = (char *)strtok(temp, delimtab);		
		JSLI(Rolecount, Rolecountarray, Roleindex);
		(*Rolecount)++;

		if (ttype == 'c' || ttype == 's') {

			attrid = 0;	/* Reinitialise attribute index */
			
			while ((attr = (char *)strtok(NULL, delimtab)) != NULL) {

				attrindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
				attridbuf = (char *)malloc(10*sizeof(char));

				/* Form the attribute index */
				strcpy(attrindex, Roleindex);
				strcat(attrindex, "_");
				sprintf(attridbuf, "%d", attrid++);
				strcat(attrindex, attridbuf);
				strcat(attrindex, "_");
				strcat(attrindex, attr);
			
				JSLI(attrcount, Attrcountarray, attrindex);
				(*attrcount)++;

				free(attrindex);
				free(attridbuf);
			}	
		}

		else {

			attrid = 0;					/* Reinitialise attribute index */
			attridbuf = (char *)malloc(10*sizeof(char));
			sprintf(attridbuf, "%d", attrid);

			/* Populate Command count in the attribute array */
			Cmdval = (char *)strtok(NULL, delimtab);		
			Cmdindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
			
			strcpy(Cmdindex, Roleindex);
			strcat(Cmdindex, "_");
			strcat(Cmdindex, attridbuf);		/* Command is attribute 0 */
			strcat(Cmdindex, "_");
			strcat(Cmdindex, Cmdval);

//			printf("\n Cmd index = %s \n", Cmdindex);

			JSLI(Cmdcount, Attrcountarray, Cmdindex);
			(*Cmdcount)++;
			
			attrid++;				/* Increment attribute counter */

			/* Start populating the Attcountarray for the fine triplet attributes */
			strcpy(index, "");
			JSLF(columnval, Columncountarray, index);
			
			while (columnval != NULL) {

				attrindex = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
				attridbuf = (char *)malloc(10*sizeof(char));
				
				strcpy(attrindex, Roleindex);
				strcat(attrindex, "_");
				sprintf(attridbuf, "%d", attrid++);
				strcat(attrindex, attridbuf);
				strcat(attrindex, "_");
							
				i = *columnval;
				while(i-- > 0) {
					attr = (char *)strtok(NULL, delimtab);
					strcat(attrindex, attr);
				}
			
				JSLI(attrcount, Attrcountarray, attrindex);
				(*attrcount)++;
				
				free(attrindex);
				free(attridbuf);

				JSLN(columnval, Columncountarray, index);		
			}
		}

		if (numattributes == 0) numattributes = attrid;
	}
}	
	
void printarray(Pvoid_t a_toprint) {

	char index[ATTRINDEXSIZE_f];
	Word_t *value, Bytes;

	strcpy(index, "");
	JSLF(value, a_toprint, index);	

	while (value != NULL) {
            printf("Index = %s, count = %d \n", index, *value);
	    JSLN(value, a_toprint, index);   // get next string
   	}
}

/* Detect false positives */
int detectfp(char ttype, char *fpdatapath) {

	FILE	*fpdatafp;
	char	*temp, fpline[MAXLINELENGTH];
	Word_t 	*Rolecount=0, *attrcount=0, *Cmdcount=0, *columnval, *value;
	short 	attrid, columnindex=0, i;
	uint8_t	Roleindex[3], *attr, *attrindex, *attridbuf, *Cmdindex, *numcolumns, *columnindexbuf, *Cmdval, *QueryRoleId;	
	char	index[3];	        	
	int 	numfp=0, maxcount, maxprobrole;
	double	ln_postprob=0.0, ln_maxprob;	
		

	if ((fpdatafp = fopen(fpdatapath, "r")) == NULL) {
		printf("\n Error opening false positives data file = %s \n", fpdatapath);
		return -1;
	}

	/*Browse through the false positives data line by line*/
	while(fgets(fpline, MAXLINELENGTH, fpdatafp) != NULL) {

		numfprecords++;

		maxcount = 0;
		ln_postprob = 0.0;
		
		/* do this for every role */
		strcpy(Roleindex, "");
		JSLF(Rolecount, Rolecountarray, Roleindex);	

		while (Rolecount != NULL) {
			
			temp = strdup(fpline);	

			/* Get current Role id */	
			QueryRoleId = (char *)strtok(temp, delimtab);		

			ln_postprob = log((double)(*Rolecount)/numtrngrecords);
	
			if (ttype == 'c' || ttype == 's') {

				attrid = 0;	/* Reinitialise attribute index */
			
				while ((attr = (char *)strtok(NULL, delimtab)) != NULL) {

					attrindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
	
					/* Form the attribute index */
					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					sprintf(attridbuf, "%d", attrid++);
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
					strcat(attrindex, attr);
			
					JSLG(attrcount, Attrcountarray, attrindex);
					if (attrcount != NULL) {
						ln_postprob = ln_postprob + log((double)(*attrcount)/(*Rolecount));
//						printf("\n %s count = %d", attrindex, *attrcount);
					}
					else 		       ln_postprob = ln_postprob + log((double)(1/2*(*Rolecount)));

					free(attrindex);
					free(attridbuf);
				}	
			}

			else {

				attrid = 0;					/* Reinitialise attribute index */
				attridbuf = (char *)malloc(10*sizeof(char));
				sprintf(attridbuf, "%d", attrid);

				/* Calculate command probability */
				Cmdval = (char *)strtok(NULL, delimtab);		
				Cmdindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
				
				strcpy(Cmdindex, Roleindex);
				strcat(Cmdindex, "_");
				strcat(Cmdindex, attridbuf);		/* Command is attribute 0 */
				strcat(Cmdindex, "_");
				strcat(Cmdindex, Cmdval);
	
				JSLG(Cmdcount, Attrcountarray, Cmdindex);
				if (Cmdcount != NULL) ln_postprob = ln_postprob + log((double)(*Cmdcount)/(*Rolecount));
				else 		      ln_postprob = ln_postprob + log((double)(1/2*(*Rolecount)));
				
				attrid++;				/* Increment attribute counter */
	
				/* Calculate attribute probabilities */
				strcpy(index, "");
				JSLF(columnval, Columncountarray, index);
				
				while (columnval != NULL) {
	
					attrindex = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
					
					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					sprintf(attridbuf, "%d", attrid++);
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
								
					i = *columnval;
					while(i-- > 0) {
						attr = (char *)strtok(NULL, delimtab);
						strcat(attrindex, attr);
					}
				
					JSLI(attrcount, Attrcountarray, attrindex);
					if (attrcount != NULL) ln_postprob = ln_postprob + log((double)(*attrcount)/(*Rolecount));
					else 		       ln_postprob = ln_postprob + log((double)(1/2*(*Rolecount)));
					
					free(attrindex);
					free(attridbuf);
	
					JSLN(columnval, Columncountarray, index);		
				}
			}

			
			/*Code for determining the max prob role*/
//			printf("\n ln_postprob = %f", ln_postprob);

			if (maxcount++ == 0) {
				ln_maxprob = ln_postprob;
				maxprobrole = atoi(Roleindex);
			}
			else if (ln_postprob > ln_maxprob) {
				ln_maxprob = ln_postprob;
				maxprobrole = atoi(Roleindex);
			}
			
			JSLN(Rolecount, Rolecountarray, Roleindex);   // get next rolecount
		}
	
//	printf("\n Maxprob role id = %d, log prob = %f", maxprobrole, ln_maxprob );

	if (maxprobrole != atoi(QueryRoleId)) fpcount++;

	}
}

/* Detect false negatives */
int detectfn(char ttype, char *fndatapath) {

	FILE	*fndatafp;
	char	*temp, fnline[MAXLINELENGTH];
	Word_t 	*Rolecount=0, *attrcount=0, *Cmdcount=0, *columnval, *value;
	short 	attrid, columnindex=0, i;
	uint8_t	Roleindex[3], *attr, *attrindex, *attridbuf, *Cmdindex, *numcolumns, *columnindexbuf, *Cmdval, *QueryRoleId;	
	char	index[3];	        	
	int 	numfp=0, maxcount, maxprobrole;
	double	ln_postprob=0.0, ln_maxprob;	
		

	if ((fndatafp = fopen(fndatapath, "r")) == NULL) {
		printf("\n Error opening false negatives data file = %s \n", fndatapath);
		return -1;
	}

	/*Browse through the false positives data line by line*/
	while(fgets(fnline, MAXLINELENGTH, fndatafp) != NULL) {

		numfnrecords++;

		maxcount = 0;
		ln_postprob = 0.0;
		
		/* do this for every role */
		strcpy(Roleindex, "");
		JSLF(Rolecount, Rolecountarray, Roleindex);	

		while (Rolecount != NULL) {
			
			temp = strdup(fnline);	

			/* Get current Role id */	
			QueryRoleId = (char *)strtok(temp, delimtab);		

			ln_postprob = log((double)(*Rolecount)/numtrngrecords);
	
			if (ttype == 'c' || ttype == 's') {

				attrid = 0;	/* Reinitialise attribute index */
			
				while ((attr = (char *)strtok(NULL, delimtab)) != NULL) {

					attrindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
	
					/* Form the attribute index */
					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					sprintf(attridbuf, "%d", attrid++);
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
					strcat(attrindex, attr);
			
					JSLG(attrcount, Attrcountarray, attrindex);
					if (attrcount != NULL) {
						ln_postprob = ln_postprob + log((double)(*attrcount)/(*Rolecount));
//						printf("\n %s count = %d", attrindex, *attrcount);
					}
					else 		       ln_postprob = ln_postprob + log((double)(1/2*(*Rolecount)));

					free(attrindex);
					free(attridbuf);
				}	
			}

			else {

				attrid = 0;					/* Reinitialise attribute index */
				attridbuf = (char *)malloc(10*sizeof(char));
				sprintf(attridbuf, "%d", attrid);

				/* Calculate command probability */
				Cmdval = (char *)strtok(NULL, delimtab);		
				Cmdindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
				
				strcpy(Cmdindex, Roleindex);
				strcat(Cmdindex, "_");
				strcat(Cmdindex, attridbuf);		/* Command is attribute 0 */
				strcat(Cmdindex, "_");
				strcat(Cmdindex, Cmdval);
	
				JSLG(Cmdcount, Attrcountarray, Cmdindex);
				if (Cmdcount != NULL) ln_postprob = ln_postprob + log((double)(*Cmdcount)/(*Rolecount));
				else 		      ln_postprob = ln_postprob + log((double)(1/2*(*Rolecount)));
				
				attrid++;				/* Increment attribute counter */
	
				/* Calculate attribute probabilities */
				strcpy(index, "");
				JSLF(columnval, Columncountarray, index);
				
				while (columnval != NULL) {
	
					attrindex = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
					
					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					sprintf(attridbuf, "%d", attrid++);
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
								
					i = *columnval;
					while(i-- > 0) {
						attr = (char *)strtok(NULL, delimtab);
						strcat(attrindex, attr);
					}
				
					JSLI(attrcount, Attrcountarray, attrindex);
					if (attrcount != NULL) ln_postprob = ln_postprob + log((double)(*attrcount)/(*Rolecount));
					else 		       ln_postprob = ln_postprob + log((double)(1/2*(*Rolecount)));
					
					free(attrindex);
					free(attridbuf);
	
					JSLN(columnval, Columncountarray, index);		
				}
			}

			
			/*Code for determining the max prob role*/
//			printf("\n ln_postprob = %f", ln_postprob);

			if (maxcount++ == 0) {
				ln_maxprob = ln_postprob;
				maxprobrole = atoi(Roleindex);
			}
			else if (ln_postprob > ln_maxprob) {
				ln_maxprob = ln_postprob;
				maxprobrole = atoi(Roleindex);
			}
			
			JSLN(Rolecount, Rolecountarray, Roleindex);   // get next rolecount
		}
	
//	printf("\n Maxprob role id = %d, log prob = %f", maxprobrole, ln_maxprob );

	if (maxprobrole == atoi(QueryRoleId)) fncount++;

	}
}


