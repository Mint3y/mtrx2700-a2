#include "serial.h"

// PRIVATE
// USART1 Port Definition
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
	default_write_completion
};

// PRIVATE
// USART1 RECEIVE Component
static struct {
	SerialPortBuffer* ready_buffer;
	SerialPortBuffer* unready_buffer;
	SerialPortBuffer buffers[2];
} USART1_RECEIVE;

// PRIVATE
// USART1 TRANSMIT Component
static struct {
	SerialPortBuffer* ready_buffer;
	SerialPortBuffer* unready_buffer;
	SerialPortBuffer buffers[2];
	bool transmit_queued;
} USART1_TRANSMIT;

// PRIVATE
// Receives a byte from USART1 RDR. Assumes RDR is not empty and result is
// stored in a SerialPortBuffer.
void receive_byte() {
	// Get the ready buffer
	SerialPortBuffer* buf = &(USART1_RECEIVE.ready_buffer);

	// Neither buffer is ready, ignore all incoming bytes
	if (buf->ready == false) {
		return;
	}

	// Clear overrun and frame errors
	USART1_PORT.UART->ICR |= USART_ICR_ORECF | USART_ICR_FECF;

	// USART1 buffer is full, ignore read
	if (buf->index >= SERIAL_BUFFER_SIZE) {
		return;
	}

	// Character is a terminator
	if (USART1_PORT.UART->RDR == SERIAL_TERMINATOR) {
		// Swap buffers
		USART1_RECEIVE.unready_buffer = buf;
		USART1_RECEIVE.ready_buffer = USART1_RECEIVE.unready_buffer;

		// Activate read completion callback
		USART1_PORT.read_complete(buf);

		// Reset buffer and mark as unready
		buf->index = 0;
		buf->length = 0;
		buf->ready = false;

		return;
	}

	// Read from USART1 RDR to the receive buffer at latest index
	buf->buffer[buf->index] = (char)USART1_PORT.UART->RDR;
	++(buf->index);
	++(buf->length);
}

// PRIVATE
// Transmits a byte by writing to USART1 TDR. Assumes TDR is empty and
// transmits from a buffer stored within a module SerialPortBuffer
void transmit_byte() {
	// Get the ready buffer
	SerialPortBuffer* buf = &(USART1_TRANSMIT.buffer);

	// Transmit has reached the end of the string
	if (buf->index >= length) {
		// Stop transmitting
		disable_usart1_transmit_interrupt();

		// Swap buffers
		USART1_TRANSMIT.unready_buffer = buf;
		USART1_TRANSMIT.ready_buffer = USART1_TRANSMIT.unready_buffer;

		// Activate write completion callback
		USART1_PORT.write_complete(buf);

		// Clear buffer
		buf->buffer_ref = NULL;
		buf->index = 0;
		buf->length = 0;
		buf->ready = true;

		// Check if second (queued, now ready) buffer needs transmitting
		if (USART1_TRANSMIT.transmit_queued) {
			USART1_TRANSMIT.transmit_queued = false;
			transmit_ready_string();
		}

		return;
	}

	// Write from transmit buffer reference to the USART1 TDR
	if (buf->buffer_ref != NULL) {
		USART1_PORT->UART->TDR = buf->buffer_ref[buf->index];
	}
	
	// Write from transmit buffer to the USART1 TDR
	else {
		USART1_PORT->UART->TDR = buf->buffer_ref.buffer[buf->index];
	}

	// Increment transmit index
	++(buf->index);
}

// PRIVATE
// This is the function called when USART1 receives an interrupt request.
void USART1_EXTI25_IRQHandler(void) {
	// Receive buffer not empty, read a byte
	if (serial_port->UART->ISR & USART_ISR_RXNE) {
		receive_byte();
	}

	// Transmit buffer is empty, transmit a byte
	if (serial_port->UART->ISR & USART_ISR_TXE) {
		transmit_byte();
	}
}

// Initialises the serial module. Should be called before using any serial
// functions.
void init_serial() {
	// Initialise receive buffers
	USART1_RECEIVE.ready_buffer = USART1_RECEIVE.buffers;
	USART1_RECEIVE.unready_buffer = USART1_RECEIVE.buffers + 1;
	USART1_RECEIVE.buffers = (SerialPortBuffer[]){
	    (SerialPortBuffer){{0}, NULL, 0, SERIAL_BUFFER_SIZE, true},
	    (SerialPortBuffer){{0}, NULL, 0, SERIAL_BUFFER_SIZE, true}
	};

	// Initialise transmit buffer
	USART1_TRANSMIT.ready_buffer = USART1_TRANSMIT.buffers;
	USART1_TRANSMIT.unready_buffer = USART1_TRANSMIT.buffers + 1;
	USART1_TRANSMIT.buffers = (SerialPortBuffer[]){
	    (SerialPortBuffer){{0}, NULL, 0, SERIAL_BUFFER_SIZE, true},
	    (SerialPortBuffer){{0}, NULL, 0, SERIAL_BUFFER_SIZE, true}
	};
	USART1_TRANSMIT.transmit_queued = false;
}

// Enable clock power and system configuration clock.
void init_usart() {
	// Enable clock power, system configuration clock and GPIOC (UARTs)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

// init_serial - Initialise the serial port
// baud_rate:     Port baud rate as specified by an enum
// serial_port:   Port register information
// read_complete: Function to execute after reading from the serial port
void init_serial_port_16bit(enum BaudRate baud_rate,
				            SerialPort* serial_port,
							void (*read_complete)(SerialPortBuffer*)) {
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
	serial_port->read_complete = read_complete;
}

// Enables interrupt requests for USART1.
void enable_usart1_interrupts() {
	// Disable all interrupt requests while changing interrupt registers
	__disable_irq();

	// Configure interrupt for USART1 (EXTI Line 25)
	// Note: USART Interrupts cannot configure rising/falling edge
	EXTI->IMR |= EXTI_IMR_MR25; // Enable bit for interrupt line 25

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

// Gets a pointer to the next open transmit buffer. A transmission buffer is 
// considered open if it is not filled nor immediately transmitting. Marks the
// obtained buffer as unready (i.e closed).
// Returns the open transmit buffer or NULL if none were available
SerialPortBuffer* get_open_transmit_buffer() {
	// Both transmission buffers are in use, fail
	if (USART1_TRANSMIT.transmit_queued) {
		return NULL;
	}

	// First buffer is ready for transmission
	if (USART1_TRANSMIT.ready_buffer->ready) {
		// Mark first buffer as unready and return it
		USART1_TRANSMIT.ready_buffer->ready = false;
		return USART1_TRANSMIT.ready_buffer;
	}

	// Mark second buffer as unready and that transmit is queued
	USART1_TRANSMIT.unready_buffer->ready = false;
	USART1_TRANSMIT.transmit_queued = true;

	// Return second buffer
	return USART_TRANSMIT.unready_buffer;
}

// Sets the transmit completion function for the USART1 Port
// write_completion: The completion function
void set_write_completion(void write_completion(SerialPortBuffer*)) {
	USART1_PORT.write_complete = write_completion;
}

// Begins transmission of the string stored in the ready transmission buffer.
// Via USART1
void begin_transmit_ready() {
	// Enable transmission and start transmitting
	enable_usart1_transmit_interrupt();
	transmit_byte();
}

// Default callback for write completion
// buf: The transmit buffer that finished transmitting
void default_write_completion(SerialPortBuffer* buf) {
	// Transmit was writing from a buffer ref
	if (buf->buffer_ref != NULL) {
		// Allow further use of the buffer ref
		buf->buffer_ref.ready = true;
	}
}

// Naive polling implementation of serial read.
// terminator:  The terminating character to read until
// buf:         The buffer to read into
// serial_port: The port to receive from
int serial_read_until_terminator(uint8_t     	  terminator,
								 SerialPortBuffer buf,
		                         SerialPort* 	  serial_port) {
	uint8_t read = '\0';

	// Read until terminating character is read or the buffer is full
	while (read != terminator && buf->index < SERIAL_BUFFER_SIZE) {
		// Clear framing and overrun errors
		serial_port->UART->ICR |= USART_ICR_ORECF | USART_ICR_FECF;

		// Wait until the read data register is not empty
		if ((serial_port->UART->ISR & USART_ISR_RXNE) == 0) {
			continue;
		}

		// Read to the buffer from the Read Data Register
		read = serial_port->UART->RDR;
		buf->buffer[buf->index] = read;
		++(buf->index);

		// Echo result back out TODO remove
		serial_write_char(read, serial_port);
	}

	// Update buffer length
	buf->length = buf->index;

	// Activate read completion callback and return the number of bytes read
	serial_port->read_complete(buf);
	return buf->length;
}

// Naively writes a char to a serial port. Waits for transmit buffer to be
// empty with a while loop.
// data: 		The char to write
// serial_port: The port to write to
void serial_write_char(char data, SerialPort* serial_port) {
	while((serial_port->UART->ISR & USART_ISR_TXE) == 0) {}

	serial_port->UART->TDR = data;
}

// Naively writes a string to a serial port.
// data:        The string to write
// length:      The length of the string
// serial_port: The port to write to
void serial_write_string(char* data, uint32_t length, SerialPort* serial_port) {
	for (int32_t i = 0; i < length; ++i) {
		serial_write_char(data[i], serial_port);
	}
}

// PRIVATE
// An example read completion function which echoes back what was read.
// buf: The transmit buffer that finished transmitting
void echo_read_completion(SerialPortBuffer* buf) {
	// Prepare the transmit buffer
	SerialPortBuffer* transmit_buf = get_open_transmit_buffer();
	transmit_buf->buffer_ref = buf;
	transmit_buf->length = buf->length;

	// Begin transmission
	begin_transmit_ready();
}

// PRIVATE
// An example (naive) read completion function which echoes back what was read.
// buf: The transmit buffer that finished transmitting
void naive_echo(SerialPortBuffer* buf) {
	serial_write_string(buf->buffer, buf->length, *USART1_PORT);
}

void test_serial() {
	SerialPortBuffer buf = (SerialPortBuffer){{0}, 0, 0, true};

	// Initialisation with naive echo
	init_usart();
	init_serial_port_16bit(BAUD_9600, &USART1_PORT, naive_echo);

	while (1) {
		// Read from USART1 (polling) and perform completion function (echo)
		int read_bytes = serial_read_until_terminator('*', buf, &USART1_PORT);

		// Transmit number of bytes written after transmitting what was just read
		serial_write_char(read_bytes, &USART1_PORT);
	}
}

void test_serial_interrupt() {
	// Serial module initialisation
	init_serial();
	init_usart();
	init_serial_port_16bit(BAUD_9600,
	                       &USART1_PORT,
						   echo_read_completion);
	enable_usart1_interrupts();
	enable_usart1_receive_interrupt();

	while (1);
}
