#include<stdio.h>
#include<string.h>
#include<nbc_const.h>
#include<nbc_ds.h>

int main(int argc, char **argv) {

	FILE *tcasesfp, *testcasefp;
	char tcasepath[100], temp[100], *tcasevalues[10], tdatapath[100], specsdatapath[100], fndatapath[100], fpdatapath[100];
	char ttype;
	int  numtrngrecords, numattributes, count;
	
	if ((tcasesfp = fopen(testcasesfilepath, "r")) == NULL) {
		printf("\n Error opening test cases file \n");
		return -1;
	}
	
	/* browse through all the test cases */
	while (myfgets(tcasepath, sizeof(tcasepath), tcasesfp) != NULL) {

		/* Re-initialise all loop variables */
		rl_hd = (role *)NULL;
		count = 0;

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

		/* Second value = number of training records */
		numtrngrecords = atoi(tcasevalues[1]);
		
		/* Third value = number of attributes */
		numattributes = atoi(tcasevalues[2]);
		
		/* Fourth value = training data file path */
		strcpy(tdatapath, tcasevalues[3]);

		/* Fifth value = specifications data file path */
		strcpy(specsdatapath, tcasevalues[4]);
		
		/* Sixth value = false positives data file path */
		strcpy(fpdatapath, tcasevalues[5]);

		/* Seventh value = false negatives data file path */
		strcpy(fndatapath, tcasevalues[6]);

		trainnbc(ttype, numtrngrecords, numattributes, tdatapath, specsdatapath);
//		detectfp(ttype, numtrngrecords, numattributes, fpdatapath, specsdatapath);
//		detectfn(ttype, numtrngrecords, numattributes, fndatapath, specsdatapath);


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
int trainnbc(char ttype, int numtrngrecords, int numattributes, char *trngdatapath, char *specsdatapath) {

	FILE *trngdatafp, *specsdatafp;
	short role_id;
	char *temp, *line;
	role *rolenode;
	
	if ((trngdatafp = fopen(trngdatapath, "r")) == NULL) {
		printf("\n Error opening training data file = %s \n", trngdatapath);
		return -1;
	}

	if ((specsdatafp = fopen(specsdatapath, "r")) == NULL) {
		printf("\n Error opening specifications data file = %s \n", specsdatapath);
		return -1;
	}
	

	if (ttype == 'c' || ttype == 's')		line = (char *)malloc((4*numattributes  + 2)*sizeof(char));
	else						line = (char *)malloc((40*numattributes + 2)*sizeof(char));

	/*Browse through the training data line by line*/
	while(fgets(line, 100000, trngdatafp) != NULL) {

		temp = strdup(line);

		/* Get the role id from the training record */
		role_id = atoi(strtok(temp, delim));		

		if ((rolenode=checkrole(role_id)) != (role *)NULL) rolenode->rl_cnt++;		/* Create role node if it doesnt exist */
		else rolenode = createrolenode(role_id);

		
		
	}

}

int printroles() {
	role *temp;
	
	temp = rl_hd;

	while (temp != NULL) {
		printf("\n Role id = %d, count = %d \n", temp->rl_id, temp->rl_cnt);
		temp = temp->next;
	}

	return ;
}
	
role *checkrole(short role_id) {

	role *temp;
	
	if (rl_hd == (role *)NULL) return (role *)NULL;
	
	temp = rl_hd;

	while (temp != (role *)NULL) {
		if (temp->rl_id == role_id) break;
		temp = temp->next;
	}
	
	return temp;
}

role *createrolenode(short role_id) {

	role 		*newrole, *temp;
	
	newrole = (role *)malloc(sizeof(role));
	newrole->rl_id = role_id;
	newrole->rl_cnt = 1;
	newrole->att_hd = (att_list *)NULL;
	newrole->next = (role *)NULL;

	if (rl_hd == (role *)NULL) rl_hd = newrole;

	else {
		temp = rl_hd;

		while (temp->next != NULL) temp = temp->next;

		temp->next = (role *)newrole;
	}

	return newrole;
}
		
		






/* Detect false positives */
int detectfp(char ttype, int numtrngrecords, int numattributes, char *fpdatapath, char *specsdatapath) {

}



