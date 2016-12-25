#include <stdio.h>
#include <stdlib.h>

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
