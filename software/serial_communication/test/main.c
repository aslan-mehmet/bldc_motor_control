#include <getopt.h>
#include <stdio.h>

int main(int argc, char **argv)
{
        struct option long_options[] = {
		{"baudrate", required_argument, NULL, 'a'},
		{"port_name", required_argument, NULL, 'b'},
		{"stop_bits", required_argument, NULL, 'c'},
                {"record", required_argument, NULL, 'd'},
		{"help", no_argument, NULL, 'h'},
		/* You have to terminate the longopts array with an entry that is all zeros
		   https://stackoverflow.com/questions/30956582/segfault-for-invalid-long-option */
		{NULL, 0, NULL, 0}
	};

        int getopt_char;
	while ((getopt_char = getopt_long(argc, argv, "a:b:c:d:h", long_options, NULL)) != EOF) {
                int tmp;
                switch (getopt_char) {
                case 'd':

                        puts("recording");

                        puts(optarg);
                        break;
                case 'h':
                        break;
                case '?':
                        puts("unrecog");
                        break;
                }
	}
        

        return 0;
}
