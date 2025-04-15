#include "serial.h"

// Static module components
static SerialPort USART1_PORT = {
	USART1,
	GPIOC,
	RCC_APB2ENR_USART1EN, // bit to enable for APB2 bus
	0x00,	              // bit to enable for APB1 bus
	RCC_AHBENR_GPIOCEN,   // bit to enable for AHB bus
	0xA00,    // Alternate Function Mode
	0xF00,    // High Speed Clock
	0x770000, // AF on external pins
	0x00,     // AF on external pins
	0x00,     // Default function pointer
	0x00,     // Default function pointer
};

static struct {
	SerialPortBuffer* ready_buffer;
	SerialPortBuffer* unready_buffer;
	SerialPortBuffer buffers[2];
} USART1_RECEIVE;

static struct {
	SerialPortBuffer buffer;
} USART1_TRANSMIT;

/// Initialises the serial module. Should be called before using any serial
/// functions.
void init_serial() {
	// Initialise receive buffers
	USART1_RECEIVE.ready_buffer = USART1_RECEIVE.buffers;
	USART1_RECEIVE.unready_buffer = USART1_RECEIVE.buffers + 1;
	USART1_RECEIVE.buffers = (SerialPortBuffer[]){
	    (SerialPortBuffer){{0}, 0, SERIAL_BUFFER_SIZE, true},
	    (SerialPortBuffer){{0}, 0, SERIAL_BUFFER_SIZE, true}
	};

	// Initialise transmit buffer
	USART1_TRANSMIT.buffer = (SerialPortBuffer){{0}, 0, 0, true};
}

/// PRIVATE
/// Receives a byte from USART1 RDR. Assumes RDR is not empty and result is
/// stored in a SerialPortBuffer.
void receive_byte() {
	SerialPortBuffer* buf = &(USART1_RECEIVE.ready_buffer);

	// Clear overrun and frame errors
	USART1_PORT.UART->ICR |= USART_ICR_ORECF | USART_ICR_FECF;

	// USART1 buffer is full, ignore read
	if (buf->index >= SERIAL_BUFFER_SIZE) {
		return;
	}

	// Read to the USART1 receive buffer
	buf->buffer[buf->index] = (char)USART1_PORT.UART->RDR;
	++(buf->index);
	// TODO removeserial_write_char(buf->buffer[buf->index - 1], &USART1_PORT);

	// Next character is a terminator
	if (USART1_PORT.UART->RDR == SERIAL_TERMINATOR) {
		// Activate read completion callback
		USART1_PORT.read_complete(buf->buffer,
				                  buf->index);

		// Swap buffers
		USART1_RECEIVE.unready_buffer = buf;
		USART1_RECEIVE.ready_buffer = USART1_RECEIVE.unready_buffer;

		buf->index = 0;
	}
}

/// PRIVATE
/// Transmits a byte by writing to USART1 TDR. Assumes TDR is empty and
/// transmits from a buffer stored within a module SerialPortBuffer
void transmit_byte() {
	SerialPortBuffer* buf = &(USART1_TRANSMIT.buffer);

	// Transmit has reached the end of the string
	if (buf->index >= length) {
		// Stop transmitting
		disable_usart1_transmit_interrupt();

		// Clear buffer
		buf->index = 0;
		buf->length = 0;
		buf->ready = true;

		return;
	}

	// Write to the USART1 transmit buffer
	USART1_PORT->UART->TDR = buf->buffer[USART1_TRANSMIT.index];

	// Increment index and check if the end of the buffer has been reached
	++buf->index;
}

/// Begins transmission of a string through USART1.
/// @param data The string to transmit
/// @param length The length of the string to transmit
void transmit_string(char* data, uint32_t length) {
	// TODO copy data into a transmit buffer

	// A different transmission is currently taking place, do not transmit
	if (!USART1_TRANSMIT.buffer.ready) {
		return;
	}

	// Start transmitting
	enable_usart1_transmit_interrupt();
	transmit_byte();
}

/// PRIVATE
/// This is the function called when USART1 receives an interrupt request.
void USART1_EXTI25_IRQHandler(void) {
	// Receive buffer not empty, read a byte
	if (serial_port->UART->ISR & USART_ISR_RXNE) {
		receive_byte();
	}

	// Transmit buffer is empty, continue transmission
	if (serial_port->UART->ISR & USART_ISR_TXE) {
		transmit_byte();
	}
}

/// Enable clock power and system configuration clock.
void init_usart() {
	// Enable clock power, system configuration clock and GPIOC (UARTs)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

/// Enables interrupt requests for USART1.
void enable_usart1_interrupts() {
	// Disable all interrupt requests while changing interrupt registers
	__disable_irq();

	// Configure interrupt for USART1 (EXTI Line 25)
	// Note: USART Interrupts cannot configure rising/falling edge
	EXTI->IMR |= EXTI_IMR_MR25; // Enable bit for line 25

	// Set priority and enable interrupt requests for USART1 in the NVIC
	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);

	// Re-enable interrupt requests
	__enable_irq();
}

/// Enables triggering interrupts when USART1 receives a byte.
void enable_usart1_receive_interrupt() {
	// Enable interrupts on USART receive
	USART1->CR1 |= USART_CR1_RXNEIE;
}

/// Enables triggering interrupts when USART1 transmits a byte.
void enable_usart1_transmit_interrupt() {
	// Enable interrupts on USART transmit
	USART1->CR1 |= USART_CR1_TXNEIE;
}

/// Disables triggering interrupts when USART1 transmits a byte.
void disable_usart1_transmit_interrupt() {
	// Disable interrupts on USART transmit
	USART1->CR1 &= ~USART_CR1_RXNEIE;
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
		// NEED TO FIX THIS ! TODO
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

	// Set the completion functions
	serial_port->completion_function = completion_function;
	serial_port->read_complete = read_complete;
}

// Naive polling implementation of serial read
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
	serial_port->read_complete(buffer, i);
	return i;
}

void serial_write_char(char data, SerialPort* serial_port) {
	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){}

	serial_port->UART->TDR = data;
}

void serial_write_string(char* data, uint32_t length, SerialPort* serial_port) {
	for (int32_t i = 0; i < length; ++i) {
		serial_write_char(data[i], serial_port);
	}
}

void echo_read_completion(char* buffer,
						  uint32_t bytes_read) {
	serial_write_char((char)bytes_read, &USART1_PORT);

	// Iterate the buffer and write the characters to USART1
	for (int32_t i = 0; i < bytes_read; ++i) {
		serial_write_char(buffer[i], &USART1_PORT);
	}
}

void test_serial() {
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

void test_serial_interrupt() {
	init_usart();
	init_serial_port_16bit(BAUD_9600,
	                       &USART1_PORT,
						   0x00,
						   echo_read_completion);
	enable_usart1_interrupts();
	enable_usart1_receive_interrupt();

	while (1);
}
