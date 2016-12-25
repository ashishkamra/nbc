#include <nbc.h>
#include <helper.h>

int main(int argc, char **argv) {

	FILE *tcasesfp, *testcasefp;
	char tcasepath[100], temp[100], *tcasevalues[5], tdatapath[100], specsdatapath[100], fndatapath[100], fpdatapath[100];

	if ((tcasesfp = fopen(testcasesfilepath, "r")) == NULL) {
		printf("\n Error opening test cases file \n");
		return ERROR;
	}


	/* browse through all the test cases */
	while (myfgets(tcasepath, sizeof(tcasepath), tcasesfp) != NULL) {

		if ((testcasefp = fopen(tcasepath, "r")) == NULL) {
			printf("\n Error opening test case %s file \n", tcasepath);
			return ERROR;
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

		createITrecords(fndatapath, fpdatapath);
	}
}
