#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	FACILITY = 0,
	LEVEL,
	GNOW,
	TASKNAME,
	TASKID,
	MSG,
	LOG_ENTRY_SIZE
} LOG_ENTRY_FIELDS; 

// REGEX FOR EXTRACTING LOG FIELDS
#define LOG_PATTERN "^<[[:digit:]]+>[[:alpha:]]+[[:space:]]+[[:digit:]]+ [[:digit:]]{2}:[[:digit:]]{2}:[[:digit:]]{2} ([[:alpha:]]{,19}): ([DIWENC])?:?([[:digit:].]{,9})?[[:space:]]?\\[([[:alpha:][:space:]_]{,19})?:?([.[:space:][:digit:]]{,19})\\] (.*{,99})"
#define NGROUPS 7 //number of match groups from LOG_PATTERN regex

regex_t rgx;
regmatch_t pmatch[NGROUPS];
char *log_entry[LOG_ENTRY_SIZE];

void print_log_entry(char *le[])
{
	printf("[FACILITY:] %s [LEVEL:] %s [GNOW]: %s [TASKNAME:] %s [TASKID:] %s, [MSG:] %s\n", le[0], le[1], le[2], le[3], le[4], le[5]);
}

void extract_log_fields(char *line)
{
	if (regexec(&rgx, line, NGROUPS, pmatch, 0) == 0) {
		for ( int i = 1; i < NGROUPS; i++) {
			int slen = pmatch[i].rm_eo - pmatch[i].rm_so;
			memcpy(log_entry[i-1], line+pmatch[i].rm_so, slen); // *i -1* as pmatch[0] contains whole string
			log_entry[i-1][slen] = '\0';
		}
		print_log_entry(log_entry);
	} else  {
		printf("COULDNAE PARSE:\n%s\n\n", line);
	}
}

int main()
{
	// initialize regex and allocate memory for regex results
	if (regcomp(&rgx, LOG_PATTERN, REG_ICASE|REG_EXTENDED) != 0) {
		fprintf(stderr, "Regex compilation failed.\n");
	}
	for ( int i = 0; i < LOG_ENTRY_SIZE; i++) {
		log_entry[i] = (char*)calloc(100, sizeof(char));
	}

	printf("BOOM\n");
	char line[200];
	while (fgets(line, 199, stdin)) {
		printf("%s", line);
		extract_log_fields(line);
	}
}
