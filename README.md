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

## Part 2: Serial Interface

## Part 3: Timed Functionality

<H4>Part a: Continuous Timer Module</H4> The period of the timer is passed during initialisation of the module (initialise_timer) as well as a desired callback function (change_pattern).This function configures timer (TIM2) such that the duration of each clock tick is 1ms. When the elapsed period is determined by the counter reaching the value stored in auto‑reload register (ARR), an update interrupt is triggered triggering the callback allowing the leds to toggle states at regular intervals.
<br> </br>
<pre> 

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

</pre>

<H4>User Instructions</H4>
1. Enable peripherals and initialise the board
<pre> 
	enable_clocks();
	initialise_board();
</pre> 

2. Call initialise_timer with preferred timer interval, memory address of led output register and the callback function

	initialise_timer(timerinterval, led_output_register, change_pattern);

Alternatively: Call testa() in main.c 


<H4>Test Case</H4>
<H4>Testing different delays</H4>

  - Input: initialise_timer(2000, led_output_register, change_pattern)
  - Output: LED’s should alternate pattern every 2 seconds 

  - Input: initialise_timer(4000, led_output_register, change_pattern)
  - Output: LED’s should alternate pattern every 4 seconds   


<H4>Part b: Alternative Continuous Timer Module</H4> This module uses encapsulation for the timer period such that no external module can modify the hardware timer registers directly. A setter function (set_period) and getter function (get_period) updates and retrieves the static variable (period) respectively. This variable is used to set the ARR and reset the counter.
<br> </br>
<pre> 
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

</pre>


<H4>User Instructions</H4>
1. Enable peripherals and initialise the board
<pre> 
	enable_clocks();
	initialise_board();
</pre> 
2. Set period value 
<pre> 
set_period(period_value);
</pre> 
3. Call initiate_timer with preferred timer interval, and the callback function
<pre>
	initiate_timer(period, change_pattern)
</pre> 

Alternatively: Call testb() in main.c 

<H4>Test Case:</H4>
<H4>Testing different delays</H4>

  - Input:  initiate_timer(1000, change_pattern)
  - Output: LED’s should cause the LED to blink every 1 second 



<H4>Part c: One-shot Timer Module</H4> This function configures the timer (TIM2) in one-pulse mode, such that when the count reaches the capture/compare register (CCR1) value an interrupt is generated.This interrupt calls the callback function to toggle the lEDs exactly once (one-shot event) after the specified delay unless retriggered.

<br> </br>
<pre> 
Main file Modular Design


Timer module file Modular Design
Oneshot_timer_start:
	(same as part a) except;
	One pulse mode is enabled for timer

TIM2_IRQHandler:
(Same as part a)	


</pre>

<H4>User Instructions</H4>
1. Enable peripherals and initialise the board
<pre> 
	enable_clocks();
	initialise_board();
</pre> 

2. Call oneshot_timer_start with preferred delay, pointer to LED output and the callback function
<pre>
	oneshot_timer_start(delay, one_shot_pattern, led_output_register)
</pre> 
Alternatively: Call testc() in main.c 

<H4>Test Case:</H4>
<H4>Testing different delays</H4>

  - Input:  oneshot_timer_start(1000, one_shot_pattern, led_output_register)
  - Output: LED’s should cause the LED to light all LEDS once 



