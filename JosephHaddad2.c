#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>

// Acknowledgements
// LCD Method from lecture has been implemented.
// Serial IO methods from lecture has been implmented.

/*
 *  Setting data directions in a data direction register (DDR)
 *
 *
 *  Setting, clearing, and reading bits in registers.
 *	reg is the name of a register; pin is the index (0..7)
 *  of the bit to set, clear or read.
 *  (WRITE_BIT is a combination of CLEAR_BIT & SET_BIT)
 */

#define SET_BIT(reg, pin)		    (reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)		  (reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)   (reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)		  (((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)	     (BIT_VALUE((reg),(pin))==1)

//uart definitions
#define BAUD (9600)
#define MYUBRR (F_CPU/16/BAUD-1)

// These buffers may be any size from 2 to 256 bytes.
#define  RX_BUFFER_SIZE  64
#define  TX_BUFFER_SIZE  64


//uart definitions
unsigned char rx_buf;

static volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
static volatile uint8_t tx_buffer_head;
static volatile uint8_t tx_buffer_tail;
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_buffer_head;
static volatile uint8_t rx_buffer_tail;

//Functions declaration
void setup(void);
void process(void);
void uart_init(unsigned int ubrr);
//uart functions
void uart_putchar(uint8_t c);
uint8_t uart_getchar(void);
uint8_t uart_available(void);
void uart_putstring(unsigned char* s);
void uart_getLine(unsigned char* buf, uint8_t n);
//ADC functions
uint16_t adc_read(uint8_t channel);
void adc_init();
//string convertion functions
void ftoa(float n, char* res, int afterpoint);
int intToStr(int x, char str[], int d);
void reverse(char* str, int len);

void setup_buzzer_pwm(void);
void setup_warninglights_timer(void);

// END function declarations

// Sets the ports as an output
void setup_warning_led(void)
{
  SET_BIT(DDRD,7);
  SET_BIT(DDRB,0);
  SET_BIT(DDRB,1);
  SET_BIT(DDRB,2);
  SET_BIT(DDRB,3);
}

// Writes output data to the port
void light_warning(void)
{
  SET_BIT(PORTD,7);
  SET_BIT(PORTB,0);
  SET_BIT(PORTB,1);
  SET_BIT(PORTB,2);
  SET_BIT(PORTB,3);


}

//main loop
int main(void)
{
  	CLEAR_BIT(DDRB,4);
  	CLEAR_BIT(DDRB,5);
	setup();
  	setup_warning_led();

	for ( ;; )
    {
      	SET_BIT(PORTB,4);
		process();
		_delay_ms(50);
      	SET_BIT(PORTB,5);
	}
}

//initialises ADC and UART port
void setup(void)
{
  CLEAR_BIT(DDRD,7);
  CLEAR_BIT(DDRD,6);
  CLEAR_BIT(DDRD,5);
  CLEAR_BIT(DDRD,4);


    //init uart
	uart_init(MYUBRR);
	SET_BIT(DDRB, 5);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX = (1 << REFS0);

}

uint16_t serial_pot;
void process(void)
{
  _delay_ms(100);

   char temp_buf[64];


   // Start single conversion by setting ADSC bit in ADCSRA
	ADCSRA |= (1 << ADSC);

	// Wait for ADSC bit to clear, signalling conversion complete.
	 while ( ADCSRA & (1 << ADSC) ) {}

	// Result from the gas sensor.
	 uint16_t pot = ADC;
  	 serial_pot = pot;	// pot is the temp sensor gas level.
  	SET_BIT(PORTD, 2);

  if (pot > 350)
  {
    SET_BIT(PORTD, 3);
  }
  else
  {
    CLEAR_BIT(PORTD, 3);
  }

  if (pot > 450)
  {
    SET_BIT(PORTD, 4);
  }
  else
  {
    CLEAR_BIT(PORTD, 4);
  }

  if (pot > 600)
  {
    SET_BIT(PORTD, 5);
    setup_buzzer_pwm();
    setup_warninglights_timer();
  }
  else
  {
    CLEAR_BIT(PORTD, 5);
  }

    //convert float to a string
    // ftoa(pot, temp_buf, 4);
    // convert uint16_t to string
     snprintf(temp_buf, sizeof(temp_buf), "%d", pot);

     if (pot > 512)
       SET_BIT(PORTB,PB5);
   	 else
       CLEAR_BIT(PORTB,PB5);


   //send serial data
   uart_putstring((unsigned char *) temp_buf);
   uart_putchar('\n');


}



int toggle1 = 0;

void setup_warninglights_timer(void)
{
  SET_BIT(DDRD,6);

  cli(); // Disable Interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 2000;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // Enable Interrupts
}

// changing pwm
volatile int del = 20;
ISR(TIMER1_COMPA_vect){

  if (toggle1 == 0)
  {
    //SET_BIT(PORTD,6);
    SET_BIT(PORTD,7);
    SET_BIT(PORTB,0);
    SET_BIT(PORTB,1);
    SET_BIT(PORTB,2);
    SET_BIT(PORTB,3);
    toggle1 = 0;
    _delay_ms(del);
    CLEAR_BIT(PORTD,7);
    CLEAR_BIT(PORTB,0);
    CLEAR_BIT(PORTB,1);
    CLEAR_BIT(PORTB,2);
    CLEAR_BIT(PORTB,3);
    _delay_ms(del);
    del++;
    if (del > 150)
    {
      del = 20;
    }
  }
  else
  {
    CLEAR_BIT(PORTD,7);
    CLEAR_BIT(PORTB,0);
    CLEAR_BIT(PORTB,1);
    CLEAR_BIT(PORTB,2);
    CLEAR_BIT(PORTB,3);
    toggle1 = 1;
  }
}


int toggle2 = 0;

void setup_buzzer_pwm(void)
{
  cli(); // Disable Interrupts

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 254;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  sei(); // Enable Interrupts
}

ISR(TIMER2_COMPA_vect)
{
  if (toggle2)
  {
    SET_BIT(PORTD,6);
    toggle2 = 0;
  }
  else
  {
    CLEAR_BIT(PORTD,6);
    toggle2 = 1;
  }
}


/*  ****** serial uart definitions ************ */
/******************  interrupt based  ********/

// Initialize the UART
void uart_init(unsigned int ubrr)
{

  	cli();

	UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  	tx_buffer_head = tx_buffer_tail = 0;
	rx_buffer_head = rx_buffer_tail = 0;

	sei();

}

// Transmit a byte
void uart_putchar(uint8_t c)
{
	uint8_t i;

	i = tx_buffer_head + 1;
	if ( i >= TX_BUFFER_SIZE ) i = 0;
	while ( tx_buffer_tail == i ); // wait until space in buffer
	//cli();
	tx_buffer[i] = c;
	tx_buffer_head = i;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << UDRIE0);
	//sei();
}

// Receive a byte
uint8_t uart_getchar(void)
{
	uint8_t c, i;

	while ( rx_buffer_head == rx_buffer_tail ); // wait for character
	i = rx_buffer_tail + 1;
	if ( i >= RX_BUFFER_SIZE ) i = 0;
	c = rx_buffer[i];
	rx_buffer_tail = i;
	return c;
}


// Transmit a string
void uart_putstring(unsigned char* s)
{
    // transmit character until NULL is reached
    while(*s > 0) uart_putchar(*s++);
}


// Receive a string
void uart_getLine(unsigned char* buf, uint8_t n)
{
    uint8_t bufIdx = 0;
    unsigned char c;

    // while received character is not carriage return
    // and end of buffer has not been reached
    do
    {
        // receive character
        c = uart_getchar();

        // store character in buffer
        buf[bufIdx++] = c;
    }
    while((bufIdx < n) && (c != '\n'));

    // ensure buffer is null terminated
    buf[bufIdx] = 0;
}


uint8_t uart_available(void)
{
	uint8_t head, tail;

	head = rx_buffer_head;
	tail = rx_buffer_tail;
	if ( head >= tail ) return head - tail;
	return RX_BUFFER_SIZE + head - tail;
}


// Transmit Interrupt
ISR(USART_UDRE_vect)
{
	uint8_t i;

	if ( tx_buffer_head == tx_buffer_tail ) {
		// buffer is empty, disable transmit interrupt
		UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	}
	else {
		i = tx_buffer_tail + 1;
		if ( i >= TX_BUFFER_SIZE ) i = 0;
		UDR0 = tx_buffer[i];
		tx_buffer_tail = i;
	}
}

// Receive Interrupt
ISR(USART_RX_vect)
{
	uint8_t c, i;

	c = UDR0;
	i = rx_buffer_head + 1;
	if ( i >= RX_BUFFER_SIZE ) i = 0;
	if ( i != rx_buffer_tail ) {
		rx_buffer[i] = c;
		rx_buffer_head = i;
	}
}
