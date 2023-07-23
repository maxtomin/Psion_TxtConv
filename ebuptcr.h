// EBuP 2 Header file

#ifndef EBUP_HEADER
#define EBUP_HEADER

// Why aren't these in a standard header?
#define FALSE 0
#define TRUE 1

// Globals

#define PATH_MAX	256
#define BUFSIZE 65536

struct tagCommonData {
	unsigned char *text;		// The text file we're working on (zero-terminated)
	int textLength;			// And its length

	unsigned char dict[256][256];	// The 256 dictionary strings, zero-terminated
	int dictLength[257];		// Their lengths (for speed; always = strlen(cData->dict[])) - and a fence
	int dictTotalLength;		// The sum + 256, i.e. the size of the dictionary within the TCR file

	unsigned char freq[256][256];		// cData->freq[a][b] is # occurrences of a followed by b in the text
	int freq_valid;				// Whether the cData->freq[][] table is up-to-date

	
	//static for "merge" function
	int follows[256], sfreq[256], skip[256], seen[256];
};


/*extern void	tcr_compress(tagCommonData *cData, char *buf, long *size);
extern char *get_dictionary_item(tagCommonData *cData, int Index, char *DictLength);*/
#endif
