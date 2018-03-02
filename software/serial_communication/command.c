#include <getopt.h>
#include <string.h>
#include <stdio.h>

static int _argc;
static char **_argv;
static char _command_names[][100] = {
"help",
"test_command_argv_separation",
"test_say_hi",
"motor",
"end" /* always last name, represent names end */
};

/* first word is name of the command */
/* like command in bash */
static int get_command_number(char *command_name)
{
	int number = -1;

        for (int i = 0; strcmp(_command_names[i], "end") != 0; ++i) {
                if (strcmp(_command_names[i], command_name) == 0) {
                        number = i;
                        break;
                }
        }

	return number;
}

static void help(void)
{
	puts("avaliable commands");

        for (int i = 0; strcmp(_command_names[i], "end") != 0; ++i) {
                puts(_command_names[i]);
        }
}

static void test_command_argv_separation(void)
{
	for (int i = 0; i < _argc; ++i) {
		puts(_argv[i]);
	}
}

/*
 * execution is similar to command in bash
 * it uses getopt long to parse options
 */
static void test_say_hi(void)
{
	optind = 0;
	int option_char = EOF;
        char name[256] = "", surname[256] = "";

	struct option long_options[] = {
		{"name", required_argument, NULL, 'a'},
		{"surname", required_argument, NULL, 'b'},
		{NULL, 0, NULL, 0}
	};

	while ((option_char = getopt_long(_argc, _argv, "a:b:", long_options, NULL)) != EOF) {
		switch (option_char) {
		case 'a':
			strcpy(name, optarg);
			break;
		case 'b':
			strcpy(surname, optarg);
			break;
		}
	}

        printf("hello %s %s\n", name, surname);
}

static void motor(void)
{}

int command_run(int argc, char **argv)
{
        /* _argc and _argv static pointers */
        _argc = argc;
	_argv = argv;

	switch (get_command_number(_argv[0])) {
	case 0:
		help();
		break;
	case 1:
		test_command_argv_separation();
		break;
	case 2:
		test_say_hi();
		break;
        case 3:
                motor();
                break;
	default:
		puts("fail: undefined command");
	}
}

