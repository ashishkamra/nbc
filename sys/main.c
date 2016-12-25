#include <nbc.h>
#include <helper.h>

/*********************************************** Global variables and constants ******************************************************************/
const char testcasesfilepath[] = "//home//akamra//IDRBAC//nbc//testcases//testcasesfiles.txt";
const char delimtab[] = {'\t','\0'};
const char delimspace[] = " ";
unsigned long  numtrngrecords, numattributes, numfprecords, fpcount, fncount, numfnrecords;
FILE	*specsdatafp;
Pvoid_t	Rolecountarray = (Pvoid_t)NULL, Attrcountarray = (Pvoid_t)NULL, Columncountarray = (Pvoid_t)NULL, Attrcountarray_data = (Pvoid_t)NULL ;
/*********************************************** Global variables and constants ******************************************************************/


int main(int argc, char **argv) {

	FILE *tcasesfp, *testcasefp;
	char tcasepath[100], temp[100], *tcasevalues[5], tdatapath[100], specsdatapath[100], fndatapath[100], fpdatapath[100];
	char ttype, specline[MAXLINELENGTH], *pch, commentchar = '%';
	int count, columnindex=0, pos;
	uint8_t	*numcolumns, *columnindexbuf;
	Word_t	*columnval;
	double trngtime_ms, detecttime_ms, precision, recall;
	clock_t trngstarttime, trngendtime, detectstarttime, detectendtime;

	if ((tcasesfp = fopen(testcasesfilepath, "r")) == NULL) {
		printf("\n Error opening test cases file \n");
		return ERROR;
	}


	/* browse through all test cases */
	while (myfgets(tcasepath, sizeof(tcasepath), tcasesfp) != NULL) {

		/* check for comments */
		pch = strchr(tcasepath, commentchar);
		if ((pch != (char *)NULL)) {
			if ((pch-tcasepath+1) == 1) goto nexttestcase;
		}
		/* check for comments */

		/* Re-initialise loop and global variables */
		count = 0;
		numtrngrecords = 0;
		numfprecords = 0;
		numfnrecords = 0;
		fpcount = 0;
		fncount = 0;
		numattributes = 0;
		Rolecountarray = (Pvoid_t)NULL;
		Attrcountarray = (Pvoid_t)NULL;
		Attrcountarray_data = (Pvoid_t)NULL;
		Columncountarray = (Pvoid_t)NULL;

		if ((testcasefp = fopen(tcasepath, "r")) == NULL) {
			printf("\n Error opening test case %s file \n", tcasepath);
			return ERROR;
		}

		while (myfgets(temp, sizeof(temp), testcasefp) != NULL) {		

			/* check for comments */
			pch = strchr(temp, commentchar);
				if ((pch != (char *)NULL)) {
					if ((pch-temp+1) == 1) goto nextline;
			}
			/* check for comments */

			tcasevalues[count] = (char *)malloc(sizeof(temp)*sizeof(char));	
			strcpy(tcasevalues[count++], temp);

			nextline:
		}

		ttype = tcasevalues[0][0];
		strcpy(tdatapath, tcasevalues[1]);
		strcpy(specsdatapath, tcasevalues[2]);
		strcpy(fpdatapath, tcasevalues[3]);
		strcpy(fndatapath, tcasevalues[4]);

		if ((specsdatafp = fopen(specsdatapath, "r")) == NULL) {
			printf("\n Error opening specifications data file = %s \n", specsdatapath);
			return ERROR;
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
		/* Populate the column count array from the first line of specs file */

		printf("\n Test case = %s", tcasepath);

		/*********************************************** training phase ******************************************************************/

		trngstarttime = clock();
		trainnbc(ttype, tdatapath);
		trngendtime = clock();

		trngtime_ms = ((double)(trngendtime-trngstarttime))/CLOCKS_PER_SEC;
		trngtime_ms = 1000*trngtime_ms;
		printf("\n Triplet type %c, %d attributes, %d triplets  trngtime = %4.6f ms", ttype, numattributes, numtrngrecords, trngtime_ms);

		/*********************************************** training phase ******************************************************************/



		/*********************************************** false positives ******************************************************************/

		detectstarttime = clock();
		detectfp(ttype, fpdatapath);
		detectendtime = clock();

		detecttime_ms = ((double)(detectendtime-detectstarttime))/CLOCKS_PER_SEC;
		detecttime_ms = (1000*detecttime_ms)/numfprecords;

//		printf("\n Triplet type %c, %d attributes detection time/triplet = %4.6f ms \n",ttype, numattributes, detecttime_ms);

		printf("\n fp(%) = %3.2f%", 100*((float)fpcount/(float)numfprecords));

		/*********************************************** false positives ******************************************************************/



		/*********************************************** false negatives ******************************************************************/

		detectstarttime = clock();
		detectfn(ttype, fndatapath, fpdatapath);
		detectendtime = clock();

		detecttime_ms = ((double)(detectendtime-detectstarttime))/CLOCKS_PER_SEC;
		detecttime_ms = (1000*detecttime_ms)/numfnrecords;

//		printf("\n Triplet type %c, %d attributes detection time/triplet = %4.6f ms \n",ttype, numattributes, detecttime_ms);

		printf("\n fn(%) = %3.2f%", 100*((float)fncount/(float)numfnrecords));

		/*********************************************** false negatives ******************************************************************/



		/*********************************************** Precision and Recall *************************************************************/

		precision = 100*(numfprecords-fpcount)/numfprecords;
		recall = 100*(numfnrecords-fncount)/numfnrecords;

		printf("\n precision = %3.2f%, recall = %3.2f% \n", precision, recall);

		/*********************************************** Precision and Recall *************************************************************/

nexttestcase:

	}
}
