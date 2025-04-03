#ifndef SERIAL_PORT_HEADER
#define SERIAL_PORT_HEADER

#include <stdint.h>
#include "stm32f303xc.h"

// We store the pointers to the GPIO and USART that are used
//  for a specific serial port. To add another serial port
//  you need to select the appropriate values.
typedef struct _SerialPort {
	USART_TypeDef *UART;
	GPIO_TypeDef  *GPIO;
	volatile uint32_t MaskAPB2ENR;	// mask to enable RCC APB2 bus registers
	volatile uint32_t MaskAPB1ENR;	// mask to enable RCC APB1 bus registers
	volatile uint32_t MaskAHBENR;	// mask to enable RCC AHB bus registers
	volatile uint32_t SerialPinModeValue;
	volatile uint32_t SerialPinSpeedValue;
	volatile uint32_t SerialPinAlternatePinValueLow;
	volatile uint32_t SerialPinAlternatePinValueHigh;
	void (*completion_function)(uint32_t);
	void (*read_complete)(uint8_t*, uint32_t);
} SerialPort;

enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};

void init_usart();

// init_serial - Initialise the serial port
// baud_rate:           Port baud rate as specified by an enum (TODO: change)
// serial_port:         Port register information
// completion_function: Function to execute when completing serial output,
//                      takes in the number of bytes sent.
void init_serial_port_16bit(uint32_t baud_rate,
				            SerialPort* serial_port,
					        void (*completion_function)(uint32_t),
							void (*read_complete)(uint8_t*, uint32_t));

int serial_read_until_terminator(uint8_t     terminator,
								  uint8_t*    buffer,
								  uint32_t    buffer_size,
								  SerialPort* serial_port);

// SerialOutputChar - output a char to the serial port
//  note: this version waits until the port is ready (not using interrupts)
// Input: char to be transferred
void serial_write_char(uint8_t data, SerialPort* serial_port);

// SerialOutputString - output a NULL TERMINATED string to the serial port
// Input: pointer to a NULL-TERMINATED string (if not null terminated, there will be problems)
void SerialOutputString(uint8_t* pt, SerialPort* serial_port);

void test_serial();

#endif // SERIAL_PORT_HEADER
