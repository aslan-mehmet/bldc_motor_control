#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_N_WORDS 100
#define MAX_WORD_LENGTH 256

/* file for fifo */
static int _fifo = -1;
/* like bash */
static int _n_words = 0;
static char **_words = NULL;
/* input line */
static int _line_size = 0;
static char *_line = NULL;

static int allocate_words(void) {
	_words = (char **) malloc(sizeof(char *) * MAX_N_WORDS);

	if (_words == NULL) {
		puts("fail: server words arr alloc error");
		return -1;
	}

	for (int i = 0; i < MAX_N_WORDS; ++i) {
		_words[i] = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
		if (_words[i] == NULL) {
			puts("fail: server word mem alloc error");
			return -1;
		}
	}

	return 0;
}

static void free_words(void)
{
	for (int i = 0; i < MAX_N_WORDS; ++i) {
		if (_words[i] != NULL) {
			free(_words[i]);
			_words[i] = NULL;
		}
	}

	if (_words != NULL) {
		free(_words);
	}
}

int server_create(void)
{
	_line_size = 1000;

	_line = (char *) malloc(sizeof(char) * _line_size);

	if (_line == NULL) {
		puts("fail: server line allocation");
		return -1;
	}

	if (allocate_words()) {
		return -1;
	}

	char my_fifo[] = "server";
	mkfifo(my_fifo, 0666);

	_fifo = open(my_fifo, 0666);

	if (_fifo < 0) {
		puts("fail: server fifo not created");
		return -1;
	}

	puts("server listening");

	/* read in non blocking mode */
	int flags = fcntl(_fifo, F_GETFL, 0);
	fcntl(_fifo, F_SETFL, flags | O_NONBLOCK);

	return 0;
}

extern void execute(int n_words, char **words);
static void separate_words(void)
{
	_n_words = 0;
	const char delim[] = " ";
	char *p = strtok(_line, delim);

	if (p == NULL) {
		return;
	}

	strcpy(_words[_n_words++], p);

	while (1) {
		p = strtok(NULL, delim);

		if (p == NULL) {
			break;
		} else {
			strcpy(_words[_n_words++], p);
		}
	}

}

static int line_index = 0;
static void erase_line(void)
{
	line_index = 0;
	_line[0] = '0';
}

static void put_into_line(char c)
{
	if (line_index == _line_size - 1) { /* nowhere to put null char*/
		erase_line();
	} else {
		_line[line_index] = c;
		line_index++;
		_line[line_index] = '\0';
	}
}

void server_loop(void)
{
	if (_fifo < 0) {
		return;
	}

	char c;

	while (read(_fifo, &c, sizeof(c)) == 1) {
		if (c == '\n') {
			separate_words();
			execute(_n_words, _words);
			erase_line();
		} else {
			put_into_line(c);
		}
	}
}

void exit_server(void)
{
	if (_fifo >= 0) {
		close(_fifo);
	}

	if (_line != NULL) {
		free(_line);
	}

	free_words();
}
