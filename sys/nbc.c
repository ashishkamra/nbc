#include <nbc.h>

/*************************************************************************************************************************************************


							Train the classifier


*************************************************************************************************************************************************/

int trainnbc(char ttype, char *trngdatapath) {

	FILE	*trngdatafp;
	char	*temp, trngline[MAXLINELENGTH];
	Word_t 	*Rolecount=0, *attrcount=0, *Cmdcount=0, *columnval, *attrcount_data=0, *Cmdcount_data=0;
	short 	attrid, i, j;
	uint8_t	*Roleindex, *attr, *attrindex, *attrindex_data, *attridbuf, *Cmdindex, *Cmdindex_data, *Cmdval;		        	
	char	index[3];

	if ((trngdatafp = fopen(trngdatapath, "r")) == NULL) {
		printf("\n Error opening training data file = %s \n", trngdatapath);
		return ERROR;
	}

	/* Browse through the training data line by line */
	while(fgets(trngline, MAXLINELENGTH, trngdatafp) != NULL) {

		numtrngrecords++;				/* counting no. of trng records */

		temp = strdup(trngline);	

		/* Populate Role count array */
		Roleindex = (char *)strtok(temp, delimtab);		
		JSLI(Rolecount, Rolecountarray, Roleindex);
		(*Rolecount)++;

		if (ttype == 'c' || ttype == 's') {

			attrid = 0;				/* Reinitialise attribute index */
			
			while ((attr = (char *)strtok(NULL, delimtab)) != NULL) {

				attrindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
				attrindex_data = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
				attridbuf = (char *)malloc(10*sizeof(char));

				sprintf(attridbuf, "%d", attrid++);

				/* Form the attribute index */
				strcpy(attrindex, Roleindex);
				strcat(attrindex, "_");
				strcat(attrindex, attridbuf);
				strcat(attrindex, "_");
				strcat(attrindex, attr);

				JSLI(attrcount, Attrcountarray, attrindex);
				(*attrcount)++;

				/* Form the attribute count index for the entire dataset */
				strcpy(attrindex_data, attridbuf);
				strcat(attrindex_data, "_");
				strcat(attrindex_data, attr);

				JSLI(attrcount_data, Attrcountarray_data, attrindex_data);
				(*attrcount_data)++;

				free(attrindex);
				free(attrindex_data);
				free(attridbuf);
			}	
		}

		else {

			attrid = 0;					/* Reinitialise attribute index */
			attridbuf = (char *)malloc(10*sizeof(char));
			sprintf(attridbuf, "%d", attrid);

			/* Populate Command count in the attribute array and attribute data array*/
			Cmdval = (char *)strtok(NULL, delimtab);		
			Cmdindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
			Cmdindex_data = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
			
			strcpy(Cmdindex, Roleindex);
			strcat(Cmdindex, "_");
			strcat(Cmdindex, attridbuf);			/* Command is attribute 0 */
			strcat(Cmdindex, "_");
			strcat(Cmdindex, Cmdval);

			JSLI(Cmdcount, Attrcountarray, Cmdindex);
			(*Cmdcount)++;

			strcpy(Cmdindex_data, attridbuf);		/* Command is attribute 0 */
			strcat(Cmdindex_data, "_");
			strcat(Cmdindex_data, Cmdval);
			
			JSLI(Cmdcount_data, Attrcountarray_data, Cmdindex_data);
			(*Cmdcount_data)++;

			attrid++;					/* Increment attribute counter */

			/* Start populating the Attcountarray for the fine triplet attributes */
			strcpy(index, "");
			JSLF(columnval, Columncountarray, index);
			
			while (columnval != NULL) {

				attrindex = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
				attrindex_data = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
				attridbuf = (char *)malloc(10*sizeof(char));
				sprintf(attridbuf, "%d", attrid++);

				strcpy(attrindex, Roleindex);
				strcat(attrindex, "_");
				strcat(attrindex, attridbuf);
				strcat(attrindex, "_");

				strcpy(attrindex_data, attridbuf);
				strcat(attrindex_data, "_");

				i = *columnval;
				while(i-- > 0) {
					attr = (char *)strtok(NULL, delimtab);
					strcat(attrindex, attr);
					strcat(attrindex_data, attr);
				}
			
				JSLI(attrcount, Attrcountarray, attrindex);
				(*attrcount)++;

				JSLI(attrcount_data, Attrcountarray_data, attrindex_data);
				(*attrcount_data)++;
				
				free(attrindex);
				free(attrindex_data);
				free(attridbuf);

				JSLN(columnval, Columncountarray, index);		
			}
		}

		if (numattributes == 0) numattributes = attrid;
	}
}	




/*************************************************************************************************************************************************


													Detect False Positives


*************************************************************************************************************************************************/

int detectfp(char ttype, char *fpdatapath) {

	FILE	*fpdatafp;
	char	*temp, fpline[MAXLINELENGTH];
	Word_t 	*Rolecount=0, *attrcount=0, *attrcount_data=0, *Cmdcount=0, *Cmdcount_data=0,*columnval, *value;
	short 	attrid, columnindex=0, i;
	uint8_t	Roleindex[3], *attr, *attrindex, *attrindex_data, *attridbuf, *Cmdindex, *Cmdindex_data, *numcolumns, *columnindexbuf, *Cmdval, 		*QueryRoleId;	
	char	index[3];	        	
	int 	numfp=0, maxcount, maxprobrole;
	double	ln_postprob=0.0, ln_maxprob, attrprob_data=0.0, Cmdprob_data=0.0, tempprob=0.0, meta=1.0;	
		

	if ((fpdatafp = fopen(fpdatapath, "r")) == NULL) {
		printf("\n Error opening false positives data file = %s \n", fpdatapath);
		return ERROR;
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

			tempprob = (double)(*Rolecount)/numtrngrecords;
			ln_postprob = log(tempprob);
		
			if (ttype == 'c' || ttype == 's') {

				attrid = 0;					/* Reinitialise attribute index */
			
				while ((attr = (char *)strtok(NULL, delimtab)) != NULL) {

					attrindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
					attrindex_data = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
					sprintf(attridbuf, "%d", attrid++);

					/* Form the attribute index */
					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
					strcat(attrindex, attr);
			
					/* Form the attribute index for the entire dataset*/
					strcpy(attrindex_data, attridbuf);
					strcat(attrindex_data, "_");
					strcat(attrindex_data, attr);

					JSLG(attrcount, Attrcountarray, attrindex);
					JSLG(attrcount_data, Attrcountarray_data, attrindex_data);

/*
					// m estimate of probability
					if (attrcount_data != NULL) attrprob_data = (double)(*attrcount_data)/(double)numtrngrecords;
					else attrprob_data = 0.0;

					if (attrprob_data == 0.0)
						tempprob = 1.0/(meta*(*Rolecount));
					else if (attrcount != NULL)
						tempprob = (double)((*attrcount) + m*attrprob_data)/(*Rolecount + m);
					else
						tempprob = (double)(m*attrprob_data)/(*Rolecount + m);
*/
			
					if (attrcount != NULL)
						tempprob = (double)(*attrcount)/(*Rolecount);
					else 
						tempprob = 1.0/(meta*(*Rolecount) + m);

					ln_postprob = ln_postprob + log(tempprob);

					free(attrindex);
					free(attrindex_data);
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
				Cmdindex_data = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));

				strcpy(Cmdindex, Roleindex);
				strcat(Cmdindex, "_");
				strcat(Cmdindex, attridbuf);			/* Command is attribute 0 */
				strcat(Cmdindex, "_");
				strcat(Cmdindex, Cmdval);

				strcpy(Cmdindex_data, attridbuf);		/* Command is attribute 0 */
				strcat(Cmdindex_data, "_");
				strcat(Cmdindex_data, Cmdval);

				JSLG(Cmdcount, Attrcountarray, Cmdindex);
				JSLG(Cmdcount_data, Attrcountarray_data, Cmdindex_data);

/*
				// m estimate
				if (Cmdcount_data != NULL) Cmdprob_data = (double)(*Cmdcount_data)/numtrngrecords;
				else Cmdprob_data = 0.0;

				if (Cmdprob_data == 0.0)
					tempprob = 1.0/(meta*(*Rolecount));
				else if (Cmdcount != NULL)
					tempprob = (double)((*Cmdcount) + m*Cmdprob_data)/(*Rolecount + m);
				else
					tempprob = (double)(m*Cmdprob_data)/(*Rolecount + m);
*/
				if (Cmdcount != NULL)
					tempprob = (double)(*Cmdcount)/(*Rolecount);
				else 
					tempprob = 1.0/(meta*(*Rolecount) + m);

				ln_postprob = ln_postprob + log(tempprob);

				attrid++;					/* Increment attribute counter */
		
				/* Calculate attribute probabilities */
				strcpy(index, "");
				JSLF(columnval, Columncountarray, index);
				
				while (columnval != NULL) {
	
					attrindex = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
					attrindex_data = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
					sprintf(attridbuf, "%d", attrid++);

					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
					
					strcpy(attrindex_data, attridbuf);
					strcat(attrindex_data, "_");
					
					i = *columnval;
					while(i-- > 0) {
						attr = (char *)strtok(NULL, delimtab);
						strcat(attrindex, attr);
						strcat(attrindex_data, attr);
					}
				
					JSLG(attrcount, Attrcountarray, attrindex);
					JSLG(attrcount_data, Attrcountarray_data, attrindex_data);

/*
					// m estimate
					if (attrcount_data != NULL) attrprob_data = (double)(*attrcount_data)/(double)numtrngrecords;
					else attrprob_data = 0.0;

					if (attrprob_data == 0.0)
						tempprob = 1.0/(meta*(*Rolecount));
					else if (attrcount != NULL)
						tempprob = (double)((*attrcount) + m*attrprob_data)/(*Rolecount + m);
					else
						tempprob = (double)(m*attrprob_data)/(*Rolecount + m);
*/

					if (attrcount != NULL)
						tempprob = (double)(*attrcount)/(*Rolecount);
					else 
						tempprob = 1.0/(meta*(*Rolecount) + m);

					ln_postprob = ln_postprob + log(tempprob);

					free(attrindex);
					free(attrindex_data);
					free(attridbuf);
	
					JSLN(columnval, Columncountarray, index);		
				}
			}

			
			/*Code for determining the max prob role*/

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

//	printf("\n %d. maxprobrole = %d, maxprob = %f, QueryRole = %d", numfprecords, maxprobrole, ln_maxprob, atoi(QueryRoleId));

	if (maxprobrole != atoi(QueryRoleId)) fpcount++;

	}
}



/*************************************************************************************************************************************************


														Detect False Negatives


*************************************************************************************************************************************************/
int detectfn(char ttype, char *fndatapath, char *fpdatapath) {

	FILE	*fndatafp;
	char	*temp, fnline[MAXLINELENGTH];
	Word_t 	*Rolecount=0, *attrcount=0, *Cmdcount=0, *columnval, *value, *attrcount_data=0, *Cmdcount_data=0;
	short 	attrid, columnindex=0, i, j=0;
	uint8_t	Roleindex[3], *attr, *attrindex, *attrindex_data, *attridbuf, *Cmdindex, *Cmdindex_data, *numcolumns, *columnindexbuf, *Cmdval, *QueryRoleId;
	char	index[3];	        	
	int 	numfp=0, maxcount, maxprobrole;
	double	ln_postprob=0.0, ln_maxprob, attrprob_data=0.0, Cmdprob_data=0.0, tempprob=0.0, meta=1.0;	
		

	/* create the FN data file */
	if ((fndatafp = fopen(fndatapath, "r")) == NULL) {
		createITintrecords(fndatapath, fpdatapath);
		if ((fndatafp = fopen(fndatapath, "r")) == NULL) {
			printf("\n Error opening false negatives data file = %s \n", fndatapath);
			return ERROR;
		}
	}
	/* create the FN data file */

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

			tempprob = (double)(*Rolecount)/numtrngrecords;
			ln_postprob = log(tempprob);
	
			if (ttype == 'c' || ttype == 's') {

				attrid = 0;	/* Reinitialise attribute index */
			
				while ((attr = (char *)strtok(NULL, delimtab)) != NULL) {

					attrindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
					attrindex_data = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
					sprintf(attridbuf, "%d", attrid++);

					/* Form the attribute index */
					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
					strcat(attrindex, attr);
			
					/* Form the attribute index for the entire dataset*/
					strcpy(attrindex_data, attridbuf);
					strcat(attrindex_data, "_");
					strcat(attrindex_data, attr);

					JSLG(attrcount, Attrcountarray, attrindex);
					JSLG(attrcount_data, Attrcountarray_data, attrindex_data);
/*
					// m estimate
					if (attrcount_data != NULL) attrprob_data = (double)(*attrcount_data)/numtrngrecords;
					else attrprob_data = 0.0;

					if (attrprob_data == 0.0)
						tempprob = 1.0/(meta*(*Rolecount));
					else if (attrcount != NULL)
						tempprob = (double)((*attrcount) + m*attrprob_data)/(*Rolecount + m);
					else
						tempprob = (double)(m*attrprob_data)/(*Rolecount + m);
*/

					if (attrcount != NULL)
						tempprob = (double)(*attrcount)/(*Rolecount);
					else 
						tempprob = 1.0/(meta*(*Rolecount) + m);

					ln_postprob = ln_postprob + log(tempprob);

					free(attrindex);
					free(attrindex_data);
					free(attridbuf);
				}	
			}

			else {

				attrid = 0;								/* Reinitialise attribute index */
				attridbuf = (char *)malloc(10*sizeof(char));
				sprintf(attridbuf, "%d", attrid);

				/* Calculate command probability */
				Cmdval = (char *)strtok(NULL, delimtab);		
				Cmdindex = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));
				Cmdindex_data = (char *)malloc(ATTRINDEXSIZE_cs*sizeof(char));

				strcpy(Cmdindex, Roleindex);
				strcat(Cmdindex, "_");
				strcat(Cmdindex, attridbuf);			/* Command is attribute 0 */
				strcat(Cmdindex, "_");
				strcat(Cmdindex, Cmdval);

				strcpy(Cmdindex_data, attridbuf);		/* Command is attribute 0 */
				strcat(Cmdindex_data, "_");
				strcat(Cmdindex_data, Cmdval);

				JSLG(Cmdcount, Attrcountarray, Cmdindex);
				JSLG(Cmdcount_data, Attrcountarray_data, Cmdindex_data);

/*
				// m estimate
				if (Cmdcount_data != NULL) Cmdprob_data = (double)(*Cmdcount_data)/numtrngrecords;
				else Cmdprob_data = 0.0;

				if (Cmdprob_data == 0.0)
					tempprob = 1.0/(meta*(*Rolecount));
				else if (Cmdcount != NULL)
					tempprob = (double)((*Cmdcount) + m*Cmdprob_data)/(*Rolecount + m);
				else
					tempprob = (double)(m*Cmdprob_data)/(*Rolecount + m);
*/
				if (Cmdcount != NULL)
					tempprob = (double)(*Cmdcount)/(*Rolecount);
				else 
					tempprob = 1.0/(meta*(*Rolecount) + m);

				ln_postprob = ln_postprob + log(tempprob);

				attrid++;					/* Increment attribute counter */
	
				/* Calculate attribute probabilities */
				strcpy(index, "");
				JSLF(columnval, Columncountarray, index);
				
				while (columnval != NULL) {
	
					attrindex = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
					attrindex_data = (char *)malloc(ATTRINDEXSIZE_f*sizeof(char));
					attridbuf = (char *)malloc(10*sizeof(char));
					sprintf(attridbuf, "%d", attrid++);

					strcpy(attrindex, Roleindex);
					strcat(attrindex, "_");
					strcat(attrindex, attridbuf);
					strcat(attrindex, "_");
					
					strcpy(attrindex_data, attridbuf);
					strcat(attrindex_data, "_");
					
					i = *columnval;
					while(i-- > 0) {
						attr = (char *)strtok(NULL, delimtab);
						strcat(attrindex, attr);
						strcat(attrindex_data, attr);
					}
				
					JSLG(attrcount, Attrcountarray, attrindex);
					JSLG(attrcount_data, Attrcountarray_data, attrindex_data);

/*
					// m estimate
					if (attrcount_data != NULL) attrprob_data = (double)(*attrcount_data)/(double)numtrngrecords;
					else attrprob_data = 0.0;

					if (attrprob_data == 0.0)
						tempprob = 1.0/(meta*(*Rolecount));
					else if (attrcount != NULL)
						tempprob = (double)((*attrcount) + m*attrprob_data)/(*Rolecount + m);
					else
						tempprob = (double)(m*attrprob_data)/(*Rolecount + m);
*/
					if (attrcount != NULL)
						tempprob = (double)(*attrcount)/(*Rolecount);
					else 
						tempprob = 1.0/(meta*(*Rolecount) + m);

					ln_postprob = ln_postprob + log(tempprob);

					free(attrindex);
					free(attrindex_data);
					free(attridbuf);

					JSLN(columnval, Columncountarray, index);		
				}
			}

			
			/*Code for determining the max prob role*/

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

	if (maxprobrole == atoi(QueryRoleId)) fncount++;

	}
}

