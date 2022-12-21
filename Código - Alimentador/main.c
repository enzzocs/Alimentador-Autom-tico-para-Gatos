#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "utils.h"
#include "spi.h"
#include "mfrc522.h"
#include "twi.h"

#define BAUD_RATE 9600
#define BAUD_PRESCALE (F_CPU / 16 / BAUD_RATE - 1)

#define set_bit(target, bit) do { (target) |= (1 << (bit)); } while(0)
#define clr_bit(target, bit) do { (target) &= ~(1 << (bit)); } while(0)
#define read_bit(source, bit) ((source) & (1 << (bit)))

#define HX711_DO_DDR DDRC
#define HX711_DO_PORT PORTC
#define HX711_DO_PIN PINC
#define HX711_DO_BIT 1
#define HX711_SK_DDR DDRC
#define HX711_SK_PORT PORTC
#define HX711_SK_BIT 0

void usart_init() {
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

int usart_send_byte(char byte, FILE *stream) {
	if (byte == '\n') {
		usart_send_byte('\r', stream);
	}
	
	while(!read_bit(UCSR0A, UDRE0));
	
	UDR0 = byte;
	
	return 0;
}

int usart_receive_byte(FILE *stream) {
	while(!read_bit(UCSR0A, RXC0));
	return UDR0;
}

FILE uart_output = FDEV_SETUP_STREAM(usart_send_byte, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, usart_receive_byte, _FDEV_SETUP_READ);

void hx711_init() {
	// DO = input, no pull up
	clr_bit(HX711_DO_PORT, HX711_DO_BIT);
	clr_bit(HX711_DO_DDR, HX711_DO_BIT);
	
	// SK = output, initial state low
	clr_bit(HX711_SK_PORT, HX711_SK_BIT);
	set_bit(HX711_SK_DDR, HX711_SK_BIT);
}

int32_t hx711_read() {
	uint32_t read_value = 0;
	
	while (read_bit(HX711_DO_PIN, HX711_DO_BIT));
	
	for (uint8_t i = 0; i < 24; i += 1) {
		set_bit(HX711_SK_PORT, HX711_SK_BIT);
		read_value <<= 1;
		clr_bit(HX711_SK_PORT, HX711_SK_BIT);
		if (read_bit(HX711_DO_PIN, HX711_DO_BIT)) {
			read_value |= 1;
		}
	}
	set_bit(HX711_SK_PORT, HX711_SK_BIT);
	read_value ^= 0x800000;
	clr_bit(HX711_SK_PORT, HX711_SK_BIT);

	return ((int32_t)read_value - 8337300) / 368;
}

int32_t hx711_read_average() {
	static uint8_t current = 0;
	int32_t values[10] = {0};
	values[current] = hx711_read();
	current = (current + 1) % 10;
	int32_t sum = 0;
	for (uint8_t i = 0; i < 10; i += 1) {
		sum += values[i];
	}
	return sum / 10;
}

volatile uint8_t flag_tempo = 0;

ISR (TIMER1_COMPA_vect)    // Timer1 ISR
{
	flag_tempo = 1;
	
}

volatile uint8_t refilling = 0;

ISR (TIMER2_COMPA_vect)
{
	if (refilling) {
		OCR0A += 1;
	}
}

uint8_t mfrc522_get_serial(uint8_t* str) {
	uint8_t baiti = mfrc522_request(PICC_REQALL, str);
	
	if(baiti == CARD_FOUND) {
		baiti = mfrc522_get_card_serial(str);
	}
	
	return baiti;
}

uint8_t tags[2][MAX_LEN] = {
	{ 0x35, 0x88, 0xCF, 0x65, 0x17, 0xB5, 0x76, 0xFF, 0x20, 0x7B, 0xEE, 0x6B, 0xD2, 0x2E, 0xEC, 0x3E },
	{ 0xBE, 0xBF, 0x65, 0x03, 0x67, 0xB5, 0x76, 0xFF, 0x20, 0x7B, 0xEE, 0x6B, 0xD2, 0x2E, 0xEC, 0x3E },
};

int32_t refill_quantities[] = { 5, 8 };
uint32_t last_feeding_time[] = { 0, 0 };

uint8_t are_tags_equal(uint8_t* tag1, uint8_t* tag2) {
	for (uint8_t i = 0; i < 5; i += 1) {
		if (tag1[i] != tag2[i]) {
			return 0;
		}
	}
	return 1;
}

typedef struct {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} time_t;

time_t get_current_time() {
	uint8_t set_ds1307_addr[1] = {0};
	uint8_t ds1307_received_data[3];

	twi_start_transceiver();
	twi_transmit_data(0x68, set_ds1307_addr, 1);
	twi_request_data(0x68, 3);
	twi_get_received_data(ds1307_received_data, 3);

	uint8_t seconds = 10 * ((ds1307_received_data[0] & 0x70) >> 4) + (ds1307_received_data[0] & 0x0F);
	uint8_t minutes = 10 * ((ds1307_received_data[1] & 0x70) >> 4) + (ds1307_received_data[1] & 0x0F);
	uint8_t is_am_pm_mode = ds1307_received_data[2] & (1 << 6);
	uint8_t hours = ds1307_received_data[2] & 0x0F;
	if (is_am_pm_mode) {
		hours += (ds1307_received_data[2] & 0x10) >> 4;
		uint8_t is_pm = ds1307_received_data[2] & (1 << 5);
		hours += is_pm ? 12 : 0;
	} else {
		hours += (ds1307_received_data[2] & 0x30) >> 4;
	}

	return (time_t){
		.hours = hours,
		.minutes = minutes,
		.seconds = seconds,
	};
}

uint32_t current_time_to_timestamp(time_t current_time) {
	return current_time.hours * 60 * 60 + current_time.minutes * 60 + current_time.seconds;
}

int main() {
	OCR1A = 31249;   // for 0.5 sec at 16 MHz

	TCCR1A = 0;
	TCCR1B = (1<<CS12) | (1<<WGM12);  // Timer mode with 256 prescaler
	TIMSK1 = (1 << OCIE1A) ;   // Enable timer1 overflow interrupt(TOIE1)

	DDRD |= _BV(PD6);
	
	TCCR0A |= _BV(COM0A1) | _BV(WGM00);
	TCCR0B |= _BV(CS00) | _BV(WGM02);

	TCCR0A |= _BV(WGM20);
	TCCR2B |= _BV(CS20) | _BV(CS21) | _BV(CS22);
	TIMSK2 |= _BV(OCIE2A);
	OCR2A = 91;

	uint8_t str[MAX_LEN];
	uint8_t baiti = CARD_NOT_FOUND;
	OCR0A = 0;
	
	spi_init();
	//_delay_ms(1000);
	mfrc522_init();
	usart_init();
	hx711_init();
	twi_master_init();
	
	stdout = &uart_output;
	_delay_ms(10);
	
	//sei();
	
	printf("Starting...\n");

	time_t time = get_current_time();
	time_t feeding_interval = (time_t){ .hours = 0, .minutes = 3, .seconds = 0 };
	uint32_t feeding_interval_timestamp = current_time_to_timestamp(feeding_interval);
	uint32_t timestamp = current_time_to_timestamp(time);
	last_feeding_time[0] = timestamp - feeding_interval_timestamp;
	last_feeding_time[1] = timestamp - feeding_interval_timestamp;

	uint8_t refill_target = 0;
	while(1){
	baiti = mfrc522_get_serial(str);
	if (baiti == CARD_FOUND) {
		printf("Card detected: [");
		for (uint8_t i = 0; i < MAX_LEN; i += 1) {
			printf("%02X", str[i]);
		}
		printf("]\n");
	}
	}
    while (1) {
		if (flag_tempo) {
			flag_tempo = 0;
			
			time_t time = get_current_time();

			printf("%02d:%02d:%02d:\n", time.hours, time.minutes, time.seconds);			

			if (baiti == CARD_FOUND) {
				printf("Card detected: [");
				for (uint8_t i = 0; i < MAX_LEN; i += 1) {
					printf("%02X", str[i]);
				}
				printf("]\n");
				for (uint8_t i = 0; i < 2; i += 1) {
					if (!are_tags_equal(tags[i], str)) {
						printf("%u fail\n", i);
						continue;
					}

					printf("%u success\n", i);

					uint32_t current_timestamp = current_time_to_timestamp(time);
					uint32_t next_feeding_timestamp = last_feeding_time[i] + current_time_to_timestamp(feeding_interval);					

					if (current_timestamp >= next_feeding_timestamp) {
						last_feeding_time[i] = current_timestamp;
						refilling = 1;
						refill_target = i;
						printf("Refilling for %u until %li grams...\n", i, refill_quantities[refill_target]);
					} else {
						printf("A refill is still not available for this cat\n");
					}

					break;
				}
			}
			
			int32_t weight = hx711_read_average();
			printf("Current weight = %li\n\n", weight);

			if (refilling && weight >= refill_quantities[refill_target]) {
				refilling = 0;
			}

			baiti = CARD_NOT_FOUND;
		}
		
		baiti = mfrc522_get_serial(str);
    }
}
