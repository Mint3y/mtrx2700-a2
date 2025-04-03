#include "serial.h"

void init_usart() {
	// Enable clock power, system configuration clock and GPIOC (UARTs)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

// init_serial - Initialise the serial port
// baud_rate:           Port baud rate as specified by an enum
// serial_port:         Port register information
// completion_function: Function to execute when completing serial output,
//                      takes in the number of bytes sent.
void init_serial_port_16bit(uint32_t baud_rate,
				            SerialPort* serial_port,
					        void (*completion_function)(uint32_t),
							void (*read_complete)(uint8_t*, uint32_t)) {
	// Enable the GPIO on the AHB bus
	RCC->AHBENR |= serial_port->MaskAHBENR;

	// Set pin Mode, Output Speed and Alternate Function
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

	// Enable the port
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Get associated Baud Rate Register (BBR) (16 bit)
	uint16_t *baud_rate_register = (uint16_t*)&serial_port->UART->BRR;

	// Baud rate calculation from datasheet
	// replace with a mathematical function TODO
	switch(baud_rate) {
	case BAUD_9600:
		// NEED TO FIX THIS !
		*baud_rate_register = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_19200:
		// NEED TO FIX THIS !
		*baud_rate_register = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_38400:
		// NEED TO FIX THIS !
		*baud_rate_register = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_57600:
		// NEED TO FIX THIS !
		*baud_rate_register = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_115200:
		*baud_rate_register = 0x46;  // 115200 at 8MHz
		break;
	}

	// Enable transmit, receive and enable
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	serial_port->completion_function = completion_function;
	serial_port->read_complete = read_complete;
}

int serial_read_until_terminator(uint8_t     terminator,
		                         uint8_t*    buffer,
		                         uint32_t    buffer_size,
		                         SerialPort* serial_port) {
	uint8_t read = '\0';
	uint32_t i = 0;

	// Read until terminating character is read or the buffer is full
	while (read != terminator && i < buffer_size) {
		// Clear framing and overrun errors
		serial_port->UART->ICR |= USART_ICR_ORECF | USART_ICR_FECF;
		// TODO remove
//		if ((serial_port->UART->ISR & USART_ISR_FE & USART_ISR_ORE) != 0) {
//			continue;
//		}

		test = serial_port->UART->ISR;

		// Wait until the read data register is not empty
		if ((serial_port->UART->ISR & USART_ISR_RXNE) == 0) {
			continue;
		}

		// Read to the buffer from the Read Data Register
		read = serial_port->UART->RDR;
		buffer[i] = read;
		++i;

		// Echo result back out TODO remove
		serial_write_char(read, serial_port);
	}

	// Activate read completion callback and return the number of bytes read
	serial_port->read_completion(buffer, i);
	return i;
}


void serial_write_char(uint8_t data, SerialPort* serial_port) {
	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){}

	serial_port->UART->TDR = data;
}


void SerialOutputString(uint8_t* pt, SerialPort* serial_port) {
	uint32_t counter = 0;
	while(*pt) {
		serial_write_char(*pt, serial_port);
		counter++;
		pt++;
	}

	// check for NULL
	if (serial_port->completion_function != 0x00) {
		serial_port->completion_function(counter);
	}
}

void echo_read_completion(uint8_t* buffer,
						  uint32_t bytes_read) {
}

void test_serial() {
	SerialPort USART1_PORT = {
		USART1,
		GPIOC,
		RCC_APB2ENR_USART1EN, // bit to enable for APB2 bus
		0x00,	              // bit to enable for APB1 bus
		RCC_AHBENR_GPIOCEN,   // bit to enable for AHB bus
		0xA00,    // Alternate Function Mode
		0xF00,    // High Speed Clock
		0x770000, // AF on external pins
		0x00,     // AF on external pins
		0x00      // Default function pointer
	};

	uint8_t buffer[500] = {0};
	uint32_t buffer_size = 500;

	init_usart();
	init_serial_port_16bit(BAUD_9600,
						   &USART1_PORT,
						   0x00,
						   echo_read_completion);
	int read_bytes = serial_read_until_terminator('*',
								                  buffer,
												  buffer_size,
												  &USART1_PORT);
	while (1);
}
