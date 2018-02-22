#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "command.h"

#define MAX_ARGC 256
#define MAX_WORD_LENGTH 256

/* file for fifo */
static int _input_fifo = -1;
/* like bash */
static int _argc = 0;
static char **_argv = NULL;
/* input line */
#define LINE_LEN 1000
static char _line[LINE_LEN];

static int allocate_argv(void) {
	_argv = (char **) malloc(sizeof(char *) * MAX_ARGC);

	if (_argv == NULL) {
		puts("fail: input fifo cant allocate memory for argv pointers");
		return -1;
	}

	for (int i = 0; i < MAX_ARGC; ++i) {
		_argv[i] = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);

		if (_argv[i] == NULL) {
			puts("fail: input fifo cant allocate memory for argv processing");
			return -1;
		}
	}

	return 0;
}

static void free_argv(void)
{
        if (_argv == NULL) {
                return;
        }

        for (int i = 0; i < MAX_ARGC; ++i) {
		if (_argv[i] != NULL) {
			free(_argv[i]);
		}
	}

	if (_argv != NULL) {
		free(_argv);
	}
}

int input_fifo_open(void)
{
allocate_argv();

	mkfifo("input_fifo", 0666);

	_input_fifo = open("input_fifo", 0666);

	if (_input_fifo < 0) {
		puts("fail: input fifo not created");
		return -1;
	}


	puts("input fifo ready");

	/* read in non blocking mode */
	int flags = fcntl(_input_fifo, F_GETFL, 0);
	fcntl(_input_fifo, F_SETFL, flags | O_NONBLOCK);

	return 0;
}

static void separate_into_argv(void)
{
	_argc = 0;
	const char delim[] = " ";
	char *p = strtok(_line, delim);

	while (p != NULL) {
                if (_argc > MAX_ARGC - 1) {
                        break;
                }

                strcpy(_argv[_argc++], p);
		p = strtok(NULL, delim);
	}

}

static int _line_index = 0;
static void erase_line(void)
{
	_line_index = 0;
	_line[0] = '\0';
}

static void put_into_line(char c)
{
	if (_line_index == LINE_LEN - 3) { /* nowhere to put null char*/
		erase_line();
	} else {
		_line[_line_index++] = c;
		_line[_line_index] = '\0';
	}
}

void input_fifo_process(void)
{
	if (_input_fifo < 0) {
		return;
	}

	char c;

	while (read(_input_fifo, &c, sizeof(c)) == 1) {
		if (c == '\n') {
			separate_into_argv();
			command_run(_argc, _argv);
			erase_line();
		} else {
			put_into_line(c);
		}
	}
}

void input_fifo_close(void)
{
	if (_input_fifo >= 0) {
		close(_input_fifo);
	}

	free_argv();
}
