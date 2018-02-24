/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "time.h"
#include "ihm07_driver.h"
#include "serial_packet.h"
#include "uart.h"

uint8_t _u8;
uint16_t _u16;
uint32_t _u32;
uint64_t _u64;
int8_t _i8;
int16_t _i16;
int32_t _i32;
int64_t _i64;
float _f;
double _d;

void test_serial_packet_encode(void)
{
	_u8 = 8;
	serial_packet_encode_poll(1, sizeof(_u8), &_u8);

	_u16 = 16;
	serial_packet_encode_poll(2, sizeof(_u16), &_u16);

	_u32 = 32;
	serial_packet_encode_poll(3, sizeof(_u32), &_u32);

	_u64 = 64;
	serial_packet_encode_poll(4, sizeof(_u64), &_u64);

	_i8 = -8;
	serial_packet_encode_poll(5, sizeof(_i8), &_i8);

	_i16 = -16;
	serial_packet_encode_poll(6, sizeof(_i16), &_i16);

	_i32 = -32;
	serial_packet_encode_poll(7, sizeof(_i32), &_i32);

	_i64 = -64;
	serial_packet_encode_poll(8, sizeof(_i64), &_i64);

	_f = 0.1;
	serial_packet_encode_poll(9, sizeof(_f), &_f);

	_d = 0.2;
	serial_packet_encode_poll(10, sizeof(_d), &_d);
}

int main(void)
{
        /* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

        /* @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority */
        /*                            2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* syscfg reset, so only change interrupt you use*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

        uart_init();
        if (serial_packet_init()) {
                while (1)
                        ;
        }
        test_serial_packet_encode();

        ihm07_led_red_init();

        while (1) {
                delay(100);
                ihm07_led_red_toggle();
                serial_packet_flush();
        }
}

void serial_packet_print(uint8_t byt)
{
        uart_send_byte_poll(byt);
}
