#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include "serial_packet_handler.h"
#include "serial_packet.h"
#include "main.h"
#include "serial_packet_sent_cmd_ids.h"

static int _argc;
static char **_argv;
static char _command_names[][100] = {
"help",
"test_command_argv_separation",
"test_say_hi",
"comment_serial_packet_record",
"stop_listening_rs232_port",
"start_listening_rs232_port",
"motor",
"main_exit",
"speed_pid",
"end" /* always last name, represent names end */
};

static char _command_names_short_brief[][100] = {
"help no options, prints this str",
"test_command_argv_separation no options, displays space separated words",
"test_say_hi have options, more --help",
"comment_serial_packet_record no options",
"stop_listening_rs232_port no options",
"start_listening_rs232_port no options, default listening",
"motor have options, more --help",
"main_exit no options, closes program",
"speed_pid have options, more --help"
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
                printf("    ");
                puts(_command_names_short_brief[i]);
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
                {"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

        static const char help_str[] = {
                "name\n"
                "    required_argument\n"
                "surname\n"
                "    required_argument\n"
                "help\n"
                "    no_argument\n"
                "    prints this string"
        };

	while ((option_char = getopt_long(_argc, _argv, "a:b:h", long_options, NULL)) != EOF) {
		switch (option_char) {
		case 'a':
			strcpy(name, optarg);
			break;
		case 'b':
			strcpy(surname, optarg);
			break;
                case 'h':
                        puts(help_str);
                        break;
		}
	}

        printf("hello %s %s\n", name, surname);
}

static void comment_serial_packet_record(void)
{
        char str[500] = "";
        for (int i = 1; i < _argc; ++i) {
                strcat(str, _argv[i]);
        }

        serial_packet_handler_record_comment(str);
}

extern int _rs232_port_listening_status;
static void stop_listening_rs232_port(void)
{
        _rs232_port_listening_status = 0;
}

static void start_listening_rs232_port(void)
{
        _rs232_port_listening_status = 1;
}

static void motor(void)
{
	optind = 0;
	int option_char = EOF;

	struct option long_options[] = {
		{"set_state", required_argument, NULL, 'a'},
                {"set_pwm", required_argument, NULL, 'b'},
                {"set_direction", required_argument, NULL, 'c'},
                {"set_speed_in_rpm", required_argument, NULL, 'd'},
                {"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

        static const char help_str[] = {
                "set_state\n"
                "    required_argument\n"
                "    enable 1\n"
                "    disable 0\n"
                "set_pwm\n"
                "    required_argument\n"
                "    max 1000\n"
                "set_direction\n"
                "    required_argument\n"
                "    ccw 1\n"
                "    cw 0\n"
                "set_speed_in_rpm\n"
                "    required_argument\n"
                "    float value in rpm\n"
                "help\n"
                "    no_argument\n"
                "    prints this string"
        };

        uint8_t u8;
        uint16_t u16;
        float f;
	while ((option_char = getopt_long(_argc, _argv, "a:b:c:h", long_options, NULL)) != EOF) {
                int tmp = -1;
		switch (option_char) {
		case 'a':
                        sscanf(optarg, "%d", &tmp);
                        u8 = 2;
                        if (tmp >= 0) {
                                u8 = (uint8_t) tmp;
                        }
                        if (u8 == 0 || u8 == 1) {
                                serial_packet_encode_poll(MOTOR_SET_STATE, sizeof(u8), &u8);
                        } else {
                                puts("fail: motor --set_state invalid_argument");
                        }
			break;
		case 'b':
                        sscanf(optarg, "%d", &tmp);
                        if (tmp >= 0) {
                                u16 = (uint16_t) tmp;
                        } else {
                                puts("fail: motor --set_pwm invalid_argument");
                                break;
                        }

                        serial_packet_encode_poll(MOTOR_SET_PWM, sizeof(u16), &u16);
			break;
                case 'c':
                        sscanf(optarg, "%d", &tmp);
                        u8 = 2;
                        if (tmp >= 0) {
                                u8 = (uint8_t) tmp;
                        }
                        if (u8 == 0 || u8 == 1) {
                                serial_packet_encode_poll(MOTOR_SET_DIRECTION, sizeof(u8), &u8);
                        } else {
                                puts("fail: motor --set_direction invalid_argument");
                        }
                        break;
                case 'd':
                        sscanf(optarg, "%f", &f);
                        if (f < 0) {
                                puts("fail: motor --set_speed invalid_argument");
                        } else {
                                serial_packet_encode_poll(MOTOR_SET_SPEED_IN_RPM, sizeof(f), &f);
                        }
                        break;
                case 'h':
                        puts(help_str);
                        break;
		}
	}
}

static void speed_pid(void)
{
	optind = 0;
	int option_char = EOF;

	struct option long_options[] = {
		{"set_kp", required_argument, NULL, 'a'},
                {"set_ki", required_argument, NULL, 'b'},
                {"set_kd", required_argument, NULL, 'c'},
                {"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

        const char help_str[] = {
                "set_kp\n"
                "    required_argument\n"
                "    float\n"
                "set_ki\n"
                "    required_argument\n"
                "    float\n"
                "set_kd\n"
                "    required_argument\n"
                "    float\n"
                "help\n"
                "    no_argument\n"
                "    prints this string"
        };

        while ((option_char = getopt_long(_argc, _argv, "a:b:c:h", long_options, NULL)) != EOF) {
                float f;
                switch (option_char) {
                case 'a':
                        sscanf(optarg, "%f", &f);
                        serial_packet_encode_poll(SPEED_PID_SET_KP, sizeof(f), &f);
                        break;
                case 'b':
                        sscanf(optarg, "%f", &f);
                        serial_packet_encode_poll(SPEED_PID_SET_KI, sizeof(f), &f);
                        break;
                case 'c':
                        sscanf(optarg, "%f", &f);
                        serial_packet_encode_poll(SPEED_PID_SET_KD, sizeof(f), &f);
                        break;
                case 'h':
                        puts(help_str);
                        break;
                }
        }
}

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
                comment_serial_packet_record();
                break;
        case 4:
                stop_listening_rs232_port();
                break;
        case 5:
                start_listening_rs232_port();
                break;
        case 6:
                motor();
                break;
        case 7:
                main_exit();
                break;
        case 8:
                speed_pid();
                break;
	default:
		puts("fail: undefined command");
	}
}

