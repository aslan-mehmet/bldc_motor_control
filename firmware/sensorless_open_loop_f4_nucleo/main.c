/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "ihm07_driver.h"
#include "serial_driver.h"
#include "serial.h"
#include "time.h"

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

        if (serial_driver_syscheck()) {
                while (1)
                        ;
        }

        serial_driver_init();
        serial_init();
        ihm07_led_red_init();
        ihm07_led_red_on();
        ihm07_adc_init();
        uint8_t arr[3] = {0xAA};

	while (1) {
                serial_flush();
                //ihm07_led_red_on();
                uint16_t pot_val = ihm07_adc_read_single_channel(IHM07_ADC_CH_POT);
                //ihm07_led_red_off();
                safe_memory_copy(arr+1, &pot_val, 2);
                serial_driver_send_buffer_poll(arr, 3);
                delay(100);
        }
}

void serial_send_byte(uint8_t byt)
{
        serial_driver_send_byte_poll(byt);
}

void serial_payload_handler(uint8_t payload_id, uint8_t payload_size
			    , void *payload)
{
        static union VARS var;
        if (payload_size <= sizeof(var)) {
                safe_memory_copy(&var.u8, payload, payload_size);
        }
}
