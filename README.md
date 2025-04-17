# Integration Task for STM32 using C Programming Language

This project incorporates the use of serial communication to transmit and receive information from the computer to the STM32 board respectively whilst acknowledging the importance of timer configurations and peripherals/hardware update events in triggering interrupts that'll allow efficient use of our processor. 
Timer's are configured as continous and in oneshot mode controlling our LEDS, ...

### Team Members

- <H4>Jiaze Sun</H4>

  - Coding/Implementation 
  - Quality Assurance  
  - Exercise 1 
  
- <H4>Johnathan Min</H4>

  - Coding/Implementation
  - Quality Assurance
  - Exercise 2 

- <H4>Marilyn Tek</H4>

  - Coding/Implementation
  - Quality Assurance
  - Exercise 3 
 
  Documentation of each member’s part was written individually and each contributed to integration. 


## Instructions for Users

[1] Guide to access and load projects 
<br> </br>
1. Ensure the latest "ST-Link drivers" is installed on your chosen device (if needed follow this link to install driver: https://os.mbed.com/teams/ST/wiki/ST-Link-Driver)

2. Ensure the latest STM32CubeIDE software is installed on your device.
   
3. Connect the STM32 board to your device.
   
4. Launch the application and select “Import Project”.
<img width="1297" alt="Image" src="https://github.com/user-attachments/assets/e697a8a9-28e7-415b-b2b0-fa2bed895afb" />

5. Select the desired project folder from your directory.
<img width="1304" alt="Image" src="https://github.com/user-attachments/assets/acf9a7dd-7feb-41c0-b2c5-e03047777c28" />

6. Select finish.
   
<img width="1302" alt="Image" src="https://github.com/user-attachments/assets/7b752495-0f33-43a2-aa24-344c07c739d1" /> 

7. Within the project in the source folder select main.c file and click the debug button (Green beetle icon).

<img width="1304" alt="Image" src="https://github.com/user-attachments/assets/8caa2e5b-81ea-424a-b6c9-e4b2b1bbc5d8" />

8. Select either the play button to run through the entire code or step through it line by line with the arrow.

<img width="1303" alt="Image" src="https://github.com/user-attachments/assets/f6f496d1-bf48-4e23-862e-1e7456cbd516" />


Note: After launching the debug mode, connect to Cutecom.


[2]Guide to use Cutecom
<br> </br>

1. Install Cutecom
   
2. Launch the application
   
3. Select connect

<img width="1299" alt="Image" src="https://github.com/user-attachments/assets/2bad9ed9-4a8a-4c9c-a9af-48569e44843a" />

4. Select the desired device and baud rate
   
<img width="1307" alt="Image" src="https://github.com/user-attachments/assets/c29dff7e-b533-4720-9c9a-2344e9b7f796" />

5. Click open device
   
<img width="1305" alt="Image" src="https://github.com/user-attachments/assets/a277ab98-2271-429d-851e-37af17ecc750" /> 

6. Set EOL Char to "None" and optionally split screen presseing on the double arrow icom

<img width="1300" alt="Image" src="https://github.com/user-attachments/assets/2a01508d-d4da-4d74-86ff-d8931add6a51" />


## Part 1: Digital I/O

Init Module introduction:
The init module provides hardware initialization for the STM32F3 board. Its main function is to configure the GPIO peripheral clock, set PE8-PE15 to the LED-controlled output pin and PA0 to the rising-edge triggered external interrupt with a callback function.
The init module is the first module called by the main () and ensures that all peripheral components are enabled and initialized.

### Public Module Components

<details>
<summary>Initialization functions</summary>
	
```c
void enable_clocks(void)
void initialise_board(void)
void enable_interrupt(void)
void setting_callback(void(*callback)(void))
```
The `enable_clocks` represents the AHB peripheral clocks for desired peripherals (GPIOA,C and E).
The `initialise_board` Set the pins PE8-15 as the universal output. These pins are used to control the led. The MODER register is set to 01(Output).
The `enable_interrupt` represents the PA0 button is configured as an external interrupt input with rising edge logic.
The `setting_callback` set the callback function when the PA0 Button is pressed
</details>

LED Module introduction:
The led module is responsible for controlling the eight leds connected to the GPIO pins PE8-15 on the STM32F3 board. The module provides an encapsulated interface for setting the LED state using an 8-bit value, retrieving the current LED state, and performing a "chase" procedure where the LED on bit moves one bit to the left after each trigger.
The module uses the Get/Set Function to avoid direct access to hardware registers outside the module.
<details>
<summary>LED functions</summary>
	
```c
void chase_led(void)
void set_led(uint8_t state)
uint8_t get_led(void)
```
The ‘chase_led’ move the LED state one bit to the left to create a moving "chase" effect. If the state changes to 0 (the light is not on), it is reset to 0x01 and the light is restarted from PE8.
The ‘set_led’ setting the led status(PE8-15)within 8 bit value; status of 8-bit value: each bit represents one LED (ON=1,OFF=0).
The ‘get_led’ returns the 8-bit LED state value stored internally.
</details>

### Testing
Overview: The testing part tests whether each module can work normally and cooperate with each other by initializing the STM32F3 system, activating interrupts and the behavior of Chase LED

Test Case: Initial LED Setup
Input:Call the enable_clocks() and initialise_board()in the main function and upload to the STM32F3
Output:All LEDs (PE8-15) are OFF initially and the value of GPIOE->ODR is 0.

Test Case: Rise-edge trigger of button
Input: Pressing button and holding.
Output:Observing the LEDs change;Phenomenon: When the button PA0 is pressed an instant LED is on or left shift, releasing the button should not change the state of LED.

Test Case: LED chase
Input: Pressing the PA0 button.
Output: The LED can be shifted one unit to the left. Chase logic from PE8 to PE15. When the LED PE15 is on and press the button the LED will move to PE8. The value of led_register will change when LED change

Test Case: Setting LED bitmask via command
Input: Use CuteComm2 to establish serial communication with STM32F3 and send led 11111111* command.
Output:The corresponding bit LED ON, for example: led 11111111* will light all leds PE8-15.But if you enter an invalid command such as led 01Hello0*, the LED will not output.



## Part 2: Serial Communication

The serial module consists of two main sections: receive and transmit. Functions are available to perform these procedures using either a polling based approach and interrupt based approach depending on the user’s use case.

The receive part of the module uses a double buffer to allow incoming bytes while another area of the program uses the previously read buffer to perform a user defined operation. This is only accessible via the interrupt based approach.

The transmit part also uses a double buffer to allow queuing of multiple transmission messages. This is only accessible via the interrupt based approach and is especially useful when transmitting messages in quick succession.

### Public Module Components
The following structs, enums and constants are publicly available to be used with the serial module.

<details>
<summary>Structs, Enums, Constants</summary>
	
```c
SerialPortBuffer (struct _SerialPortBuffer)
SerialPort (struct _SerialPort)
enum BaudRate
SERIAL_BUFFER_SIZE (definition)
SERIAL_TERMINATOR (definition)
```
The `SerialPortBuffer` represents either the receive or transmission buffer for USART. `SerialPort` is a grouping of the actual USART port addresses. A `BaudRate` value can be passed while initialising a serial port to set a custom baud rate.

The constants SERIAL_BUFFER_SIZE and SERIAL_TERMINATOR can be defined prior to including the module to set a custom buffer size and custom serial terminating character respectively.
</details>


### Public Module Functions
The following list of serial functions are publicly available from other files/modules.

<details>
<summary>Initialisation functions</summary>
	
```c
void init_serial();
void init_usart();
void init_serial_port_16bit(
        enum BaudRate baud_rate,
        SerialPort* serial_port,
        void (*read_complete)(SerialPortBuffer*)
);
```
These should all be called prior to use of the module. When calling `init_serial_port_16bit` it is useful to pass in the usart1 port reference from `get_usart1_port()`. A custom read completion function can be passed during port initialisation to create more advanced functionality. A write completion callback can be set after port initialisation, otherwise it comes with a default callback function. Read and write callback functions regardless of the module’s approach (polling/interrupt).

The `init_serial` function is not required to be called if the module is planned to be used in a purely polling based approach.
</details>

<details>
<summary>Toggles, getters and setters</summary>
	
```c
void enable_usart1_interrupts();
void enable_usart1_receive_interrupt();
void enable_usart1_transmit_interrupt();
void disable_usart1_transmit_interrupt();
SerialPort* get_usart1_port();
SerialPortBuffer* get_open_transmit_buffer();
void set_write_completion(void write_completion(SerialPortBuffer*));
```
If the interrupt based approach will be used then the user should call `enable_usart1_interrupts` and `enable_usart1_receive_interrupt`. The transmit interrupt enable/disable will be automatically called by the module as required. See description of `Initialisation functions` for further information on the getters and setters.

`get_open_transmit_buffer` will return either `NULL (0x00)` or a reference to a `SerialPortBuffer` struct which is not actively being used for transmission. This buffer should be filled with user defined contents before beginning transmission (see `Transmitting and receiving`).
</details>

<details>
<summary>Transmitting and receiving</summary>
	
```c
void begin_transmit_ready();
void attempt_serial_transmit(char* data, uint32_t length);
int serial_read_until_terminator(
        uint8_t terminator,
        SerialPortBuffer* buf,
        SerialPort* serial_port
);
void serial_write_char(char data, SerialPort* serial_port);
void serial_write_string(char* data, uint32_t length, SerialPort* serial_port);
```
If a previous call to `enable_usart1_receive_interrupt` had been made, the module will internally receive all incoming bytes from a serial port. The user should supply a read completion callback function to access data that is received via this port.

`begin_transmit_ready` will begin transmission of the transmit ready buffer within the module. This will typically be called after filling a transmit buffer received by a call to `get_open_transmit_buffer`.

`attempt_serial_transmit` wraps the transmit operation into one neat function, but depending on use case may not be sufficient. If the interrupt based approach is to be used then this function may be too inefficient for the user’s needs as it copies an entire string into an open transmit buffer (if available) before transmitting.

`serial_read_until_terminator` is a polling-based receiving function that receives bytes until a given terminator character is read. It halts the program within itself until this terminator is read from the serial port.

`serial_write_char` is a polling-based transmission function which waits for the transmit buffer to be empty before transmitting a byte. This should not be used in conjunction with the interrupt based transmission functions to avoid undefined functionality (overwriting bytes that would have been transmitted otherwise).

`serial_write_string` is a polling-based transmission function for strings. It is a wrapper for `serial_write_char`.
</details>

<details>
<summary>Miscellaneous</summary>
	
```c
void default_write_completion(SerialPortBuffer* buf);
void test_serial();
void test_serial_interrupt();
```
`default_write_completion` can be accessed to reset the write completion function of a serial port. The other two test functions provide examples of how to use the serial module.
</details>

### Testing
Example test cases can be found in the functions referred to in the `Miscellaneous` section. These functions set USART1’s port callback to an echo function and are expected to echo back (via transmit) anything that the board receives.
```
Input:                     |   Output:
---------------------------------------------------------
Some string*               |   Some string
not a command*             |   not a command
Hello World Domination!*   |   Hello World Domination!
Incomplete receive         |   
continue*                  |   Incomplete receivecontinue
```

The double buffer implementation can be tested by attempting multiple transmissions in quick succession. Simply, this means calling `attempt_serial_transmit` multiple times with different inputs. As the double buffer is only capable of queuing two transmit messages, any sequential messages that are attempted prior to the first one being fully transmitted should fail silently. The functionality of the double buffer has been tested in the `Command Prompt` module. The double buffer implementation is identical for both transmit and receive.


## Part 3: Timed Functionality

<H4>Part a: Continuous Timer Module</H4> The period of the timer is passed during initialisation of the module (initialise_timer) as well as a desired callback function (change_pattern).This function configures timer (TIM2) such that the duration of each clock tick is 1ms. When the elapsed period is determined by the counter reaching the value stored in auto‑reload register (ARR), an update interrupt is triggered triggering the callback allowing the leds to toggle states at regular intervals.
<br> </br>

### Modular Design
	
```c
Main file Modular Design
Change_pattern:
Initialise count variable 
	Initialise array of led patterns in hex
	Increment count variable

Main:
	Enable Clock
	Initialise Board
	Initialise Timer(timerinterval)


Timer module file Modular Design

Initialise_timer:
	Sets callback to static variable 
	Configure Prescaler
	Configure Timer Period
	Generate Interrupt Request
	Enable Interrupt Request
	Start Timer Count

TIM2_IRQHandler:
	Clear Update Flag
	Call Callback Function

```



<H4>User Instructions</H4>

<details>
<summary>Enable peripherals and initialise the board</summary>
	
```c
enable_clocks();
initialise_board();
```
</details>


<details>
<summary>Call initialise_timer with preferred timer interval, memory address of led output register and the callback function</summary>
	
```c
initialise_timer(timerinterval, led_output_register, change_pattern);

```
</details>

Alternatively: Call testa() in main.c 


<H4>Test Case</H4>
<H4>Testing different delays</H4>

  - Input: initialise_timer(2000, led_output_register, change_pattern)
  - Output: LED’s should alternate pattern every 2 seconds 

  - Input: initialise_timer(4000, led_output_register, change_pattern)
  - Output: LED’s should alternate pattern every 4 seconds   


<H4>Part b: Alternative Continuous Timer Module</H4> This module uses encapsulation for the timer period such that no external module can modify the hardware timer registers directly. A setter function (set_period) and getter function (get_period) updates and retrieves the static variable (period) respectively. This variable is used to set the ARR and reset the counter.
<br> </br>

### Modular Design

```C
Main file Modular Design
Change_pattern:
	Alternate pattern based on period value
	
Main:
	Enable Clock
	Initialise Board
	Set period
	Get period
	Initiate Timer(period)

Timer module file Modular Design
Set_period:
	Set static period to parameter value
Set ARR to period
Reset timer

get_period:
Returns period

Initiate_timer:
(same as part a)

TIM2_IRQHandler:
	(same as part a)

```


<H4>User Instructions</H4>

<details>
<summary>Enable peripherals and initialise the board</summary>
	
```c
enable_clocks();
initialise_board();
```
</details>

<details>
<summary>Set period value </summary>
	
```c
set_period(period_value);
```
</details>



<details>
<summary>Call initiate_timer with preferred timer interval, and the callback function</summary>
	
```C
initiate_timer(period, change_pattern)
```
</details>

Alternatively: Call testb() in main.c 


<H4>Test Case:</H4>
<H4>Testing different delays</H4>

  - Input:  initiate_timer(1000, change_pattern)
  - Output: LED’s should cause the LED to blink every 1 second 


<H4>Part c: One-shot Timer Module</H4> This function configures the timer (TIM2) in one-pulse mode, such that when the count reaches the capture/compare register (CCR1) value an interrupt is generated.This interrupt calls the callback function to toggle the lEDs exactly once (one-shot event) after the specified delay unless retriggered.

<br> </br>

### Modular Design

```C
Main file Modular Design


Timer module file Modular Design
Oneshot_timer_start:
	(same as part a) except;
	One pulse mode is enabled for timer

TIM2_IRQHandler:
(Same as part a)	

```

<H4>User Instructions</H4>


<details>
<summary>Enable peripherals and initialise the board</summary>
	
```C
enable_clocks();
initialise_board();
```
</details>

<details>
<summary>Call oneshot_timer_start with preferred delay, pointer to LED output and the callback function</summary>
	
```C
oneshot_timer_start(delay, one_shot_pattern, led_output_register)

```
</details>

Alternatively: Call testc() in main.c 

<H4>Test Case:</H4>
<H4>Testing different delays</H4>

  - Input:  oneshot_timer_start(1000, one_shot_pattern, led_output_register)
  - Output: LED’s should cause the LED to light all LEDS once 



