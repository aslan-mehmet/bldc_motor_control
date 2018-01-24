#include <getopt.h>
#include <string.h>
#include <stdio.h>

#define N_EXECS 3

static int _argc;
static char **_argv;
static char _exec_names[N_EXECS][100] = {"help", /* 0 */
					"test_word", /* 1 */
					"test_opt", /* 2 */
};

/* first word is name of the execution */
/* like command in bash */
int get_exec_number(char *str)
{
	int r = -1;

	for (int i = 0; i < N_EXECS; ++i) {
		if (strcmp(str, _exec_names[i]) == 0) {
			r = i;
		}
	}

	return r;
}

/* 
 * print execution help string
 * which is what you can do interactively
 */
static void print_help(void)
{
	puts("execution names");

	for (int i = 0; i < N_EXECS; ++i) {
		printf("%d->%s\n", i, _exec_names[i]);
	}
}
/* do words separated as in argv */
static void test_word(void)
{
	for (int i = 0; i < _argc; ++i) {
		puts(_argv[i]);
	}
}

/* 
 * execution is similar to command in bash 
 * it uses getopt long to parse options
 */
static void test_opt(void)
{
	optind = 0;
	int ret = EOF;

	struct option long_options[] = {
		{"name", required_argument, NULL, 'n'},
		{"salute", no_argument, NULL, 's'},
		{NULL, 0, NULL, 0}
	};

	while ((ret = getopt_long(_argc, _argv, "n:s", long_options, NULL)) != EOF) {
		switch (ret) {
		case 'n':
			printf("your name is %s\n", optarg);
			break;
		case 's':
			puts("hello world");
			break;
		}
	}
}

void execute(int n_words, char **words)
{
        /* _argc and _argv static pointers */
        _argc = n_words;
	_argv = words;

	switch (get_exec_number(words[0])) {
                /* executions know options with above static pointers */
	case 0:
		print_help();
		break;
	case 1:
		test_word();
		break;
	case 2:
		test_opt();
		break;
	default:
		puts("fail: unknown execution");
	}
}
