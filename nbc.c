#include <nbc.h>

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

