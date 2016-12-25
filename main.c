#include <nbc.h>
#include <helper.h>

/* Global Constants */
const char testcasesfilepath[] = "//home//akamra//IDRBAC//nbc//testcases//testcasesfiles.txt";
const char delimtab[] = {'\t','\0'};
const char delimspace[] = " ";

/* Global Variables */
unsigned long  numtrngrecords, numattributes, numfprecords, fpcount, fncount, numfnrecords;

FILE	*specsdatafp;
Pvoid_t	Rolecountarray = (Pvoid_t)NULL, Attrcountarray = (Pvoid_t)NULL, Columncountarray = (Pvoid_t)NULL;

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
	

