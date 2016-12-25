/* Data Structures */
typedef struct {
	short 		rl_id;
	int 		rl_cnt;
	struct att_list	*att_hd;
	struct role 	*next;
} role;

typedef struct {
	short 			att_id;
	struct 	attval_list	*attval_hd;
	struct 	att_list	*next;
} att_list;

typedef struct {
	int	count;
	struct 	attval_list	*next;
	char 	val[];
} attval_list;

/* Global Variables */
role *rl_hd;




/* Function Declarations */
char *myfgets(char *, int, FILE *);

role *createrolenode(short);

role *checkrole(short);

int addattrnode(short,short);		// Role id, attr node id

att_list *getattrnode(short, short);	// Role id, attr node id

int addattrvalnode(short, char *);	// Attribute node id, attribute value, initialise count to 1

int incattrvalcount(short, char *);	// Attribute node id, attribute value

int getattrvalcount(short, char *);	// Attribute node id, attribute value

int trainnbc(char, int, int, char *, char *); // ttype, numtrngrecords, numattributes, tdatapath, specsdatapath

int detectfp(char, int, int, char *, char *); // ttype, numtrngrecords, numattributes, fpdatapath, specsdatapath

int detectfn(char, int, int, char *, char *); // ttype, numtrngrecords, numattributes, fndatapath, specsdatapath

void freememory();
