#include <avr/io.h>
#include <avr/interrupt.h>
#include "twi.h"

static volatile uint8_t twi_target_addr;
static volatile uint8_t twi_target_addr_sent;
static volatile uint8_t twi_buffer[TWI_BUFFER_SIZE];
static volatile uint8_t twi_message_size;
static volatile uint8_t twi_state = TWI_NO_STATE;

void twi_master_init() {
	TWBR = TWI_TWBR;
	TWDR = 0xFF;
	TWCR =
	(1 << TWEN) |
	(0 << TWIE) | (0 << TWINT) |
	(0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) |
	(0 << TWWC);
}

void twi_start_transceiver() {
	while(twi_is_busy());

	TWCR =
	(1 << TWEN) |
	(1 << TWIE) | (1 << TWINT) |
	(0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) |
	(0 << TWWC);
}

uint8_t twi_is_busy() {
	return (TWCR & (1 << TWIE));
}

uint8_t twi_get_state_info() {
	while(twi_is_busy());

	return twi_state;
}

void twi_transmit_data(uint8_t addr, uint8_t* msg, uint8_t msg_size) {
	while(twi_is_busy());

	twi_message_size = msg_size;
	twi_target_addr = (addr << 1);

	for (uint8_t i = 0; i < msg_size; i++) {
		twi_buffer[i] = msg[i];
	}

	TWCR =
	(1 << TWEN) |
	(1 << TWIE) | (1 << TWINT) |
	(0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) |
	(0 << TWWC);
}

void twi_request_data(uint8_t addr, uint8_t msg_size) {
	while(twi_is_busy());

	twi_message_size = msg_size;
	twi_target_addr = (addr << 1) | 1;

	TWCR =
	(1 << TWEN) |
	(1 << TWIE) | (1 << TWINT) |
	(0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) |
	(0 << TWWC);
}

void twi_get_received_data(uint8_t* msg, uint8_t msg_size) {
	while(twi_is_busy());
	
	for(uint8_t i = 0; i < msg_size; i++) {
		msg[i] = twi_buffer[i];
	}
}

ISR(TWI_vect)
{
	static uint8_t twi_buffer_index;
	
	switch (TWSR) {
		case TWI_START:
		case TWI_REP_START:
		twi_buffer_index = 0;
		twi_target_addr_sent = 0;

		case TWI_MTX_ADR_ACK:
		case TWI_MTX_DATA_ACK:
		if(twi_target_addr_sent == 0) {
			twi_target_addr_sent = 1;
			TWDR = twi_target_addr;
			TWCR =
			(1 << TWEN) |
			(1 << TWIE) | (1 << TWINT) |
			(0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) |
			(0 << TWWC);
		}
		else {
			if (twi_buffer_index < twi_message_size) {
				TWDR = twi_buffer[twi_buffer_index++];
				TWCR =
				(1 << TWEN) |
				(1 << TWIE) | (1 << TWINT) |
				(0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) |
				(0 << TWWC);
				} else {
				TWCR =
				(1 << TWEN) |
				(0 << TWIE) | (1 << TWINT) |
				(0 << TWEA) | (0 << TWSTA) | (1 << TWSTO) |
				(0 << TWWC);
			}
		}
		break;

		case TWI_MRX_DATA_ACK:
		twi_buffer[twi_buffer_index++] = TWDR;

		case TWI_MRX_ADR_ACK:
		if (twi_buffer_index < twi_message_size) {
			TWCR =
			(1 << TWEN) |
			(1 << TWIE) | (1 << TWINT) |
			(1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) |
			(0 << TWWC);
			} else {
			TWCR =
			(1 << TWEN) |
			(1 << TWIE) | (1 << TWINT) |
			(0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) |
			(0 << TWWC);
		}
		break;

		case TWI_MRX_DATA_NACK:
		twi_buffer[twi_buffer_index] = TWDR;
		TWCR =
		(1 << TWEN) |
		(0 << TWIE) | (1 << TWINT) |
		(0 << TWEA) | (0 << TWSTA) | (1 << TWSTO) |
		(0 << TWWC);
		break;

		case TWI_ARB_LOST:
		TWCR =
		(1 << TWEN) |
		(1 << TWIE) | (1 << TWINT) |
		(0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) |
		(0 << TWWC);
		break;

		case TWI_MTX_ADR_NACK:
		case TWI_MRX_ADR_NACK:
		case TWI_MTX_DATA_NACK:
		case TWI_BUS_ERROR:
		default:

		twi_state = TWSR;
		TWCR =
		(1 << TWEN) |
		(0 << TWIE) | (0 << TWINT) |
		(0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) |
		(0 << TWWC);
		break;
	}
}