#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Acknowledgements
// LCD Method from lecture has been implemented.
// Serial IO methods from lecture has been implmented.


//For more information about this library please visit:
//http://www.arduino.cc/en/Reference/LiquidCrystal

//and

//https://github.com/arduino-libraries/LiquidCrystal

// --== WIRING ==--
// LCD GND  -> GND
// LCD VCC  -> 5V
// LCD V0   -> GND
// LCD RW   -> GND
// LCD LED Anode    -> 220 Ohm -> 5V
// LCD LED Cathode  -> GND

//	Change the values in these defines to reflect
//  how you've hooked up the screen
//	In 4-pin mode only DATA4:7 are used

#define LCD_USING_4PIN_MODE (1)

// #define LCD_DATA0_DDR (DDRD)
// #define LCD_DATA1_DDR (DDRD)
// #define LCD_DATA2_DDR (DDRD)
// #define LCD_DATA3_DDR (DDRD)
#define LCD_DATA4_DDR (DDRD)
#define LCD_DATA5_DDR (DDRD)
#define LCD_DATA6_DDR (DDRD)
#define LCD_DATA7_DDR (DDRD)


// #define LCD_DATA0_PORT (PORTD)
// #define LCD_DATA1_PORT (PORTD)
// #define LCD_DATA2_PORT (PORTD)
// #define LCD_DATA3_PORT (PORTD)
#define LCD_DATA4_PORT (PORTD)
#define LCD_DATA5_PORT (PORTD)
#define LCD_DATA6_PORT (PORTD)
#define LCD_DATA7_PORT (PORTD)

// #define LCD_DATA0_PIN (0)
// #define LCD_DATA1_PIN (1)
// #define LCD_DATA2_PIN (2)
// #define LCD_DATA3_PIN (3)
#define LCD_DATA4_PIN (4)
#define LCD_DATA5_PIN (5)
#define LCD_DATA6_PIN (6)
#define LCD_DATA7_PIN (7)


#define LCD_RS_DDR (DDRB)
#define LCD_ENABLE_DDR (DDRB)

#define LCD_RS_PORT (PORTB)
#define LCD_ENABLE_PORT (PORTB)

#define LCD_RS_PIN (1)
#define LCD_ENABLE_PIN (0)

//DATASHEET: https://s3-us-west-1.amazonaws.com/123d-circuits-datasheets/uploads%2F1431564901240-mni4g6oo875bfbt9-6492779e35179defaf4482c7ac4f9915%2FLCD-WH1602B-TMI.pdf

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

void lcd_init(void);
void lcd_write_string(uint8_t x, uint8_t y, char string[]);
void lcd_write_char(uint8_t x, uint8_t y, char val);

void lcd_clear(void);
void lcd_home(void);

void lcd_createChar(uint8_t, uint8_t[]);
void lcd_setCursor(uint8_t, uint8_t);

void lcd_noDisplay(void);
void lcd_display(void);
void lcd_noBlink(void);
void lcd_blink(void);
void lcd_noCursor(void);
void lcd_cursor(void);
void lcd_leftToRight(void);
void lcd_rightToLeft(void);
void lcd_autoscroll(void);
void lcd_noAutoscroll(void);
void scrollDisplayLeft(void);
void scrollDisplayRight(void);

size_t lcd_write(uint8_t);
void lcd_command(uint8_t);


void lcd_send(uint8_t, uint8_t);
void lcd_write4bits(uint8_t);
void lcd_write8bits(uint8_t);
void lcd_pulseEnable(void);

uint8_t _lcd_displayfunction;
uint8_t _lcd_displaycontrol;
uint8_t _lcd_displaymode;

#define SET_BIT(reg, pin)		    (reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)		  (reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)   (reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)		  (((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)	     (BIT_VALUE((reg),(pin))==1)

//uart definitions
#define BAUD (9600)
#define MYUBRR (F_CPU/16/BAUD-1)
#define  RX_BUFFER_SIZE  64
#define  TX_BUFFER_SIZE  64

// DEFINITIONS AND INITITAL STUFF. =================================

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

// Button Debouncing
volatile uint8_t pushbuttons_db = 0;
volatile uint8_t reg1, reg2; // used to keep track with 2 or more time steps.
uint8_t pb_now = 0, pb_prev = 0, pb_pressed;

//initialises ADC and UART port
void setup(void)
{

    //init uart
	uart_init(MYUBRR);

	// Enable orange LED
	SET_BIT(DDRB, 5);

    // initialise adc
	// ADC Enable and pre-scaler of 128: ref table 24-5 in datasheet
    // ADEN =1
    // ADPS2 =1, ADPS1=1, ADPS0 = 1
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

   // select channel and ref voltage
   // channel 0, PC0 (A0 on the uno)
   // MUX0=0, MUX1=0, MUX2=0, MUX3=0
   // REFS0=1
   // REFS1=0
    ADMUX = (1 << REFS0);

}


void process(void)
{
  _delay_ms(190);

   char temp_buf[64];

   // Start single conversion by setting ADSC bit in ADCSRA
	ADCSRA |= (1 << ADSC);

	// Wait for ADSC bit to clear, signalling conversion complete.
	 while ( ADCSRA & (1 << ADSC) ) {}

  	// =========================================
	// THIS IS WHAT THE ANALOG PORT READS
	// Result now available.
	 uint16_t pot = ((((double)ADC / 1024) * 5)-0.5)*100;
  	// writes to lcd screen

  	if (pot == 0)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 0");
    }
  	if (pot == 1)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 1");
    }
  	if (pot == 2)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 2");
    }
  	if (pot == 3)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 3");
    }
  	if (pot == 4)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 4");
    }
  	if (pot == 5)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 5");
    }
  	if (pot == 6)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 6");
    }
  	if (pot == 7)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 7");
    }
  	if (pot == 8)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 8");
    }
  	if (pot == 9)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 9");
    }
  	if (pot == 10)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 10");
    }
  	if (pot == 11)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 11");
    }
   	if (pot == 12)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 12");
    }
  	if (pot == 13)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 13");
    }
  	if (pot == 14)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 14");
    }
  	if (pot == 15)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 15");
    }
  	if (pot == 16)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 16");
    }
  	if (pot == 17)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 17");
    }
  	if (pot == 18)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 18");
    }
  	if (pot == 19)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 19");
    }
  	if (pot == 20)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 20");
    }
  	if (pot == 21)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 21");
    }
    if (pot == 22)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 22");
    }
  	if (pot == 23)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 23");
    }
  	if (pot == 24)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 24");
    }
  	if (pot == 25)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 25");
    }
  	if (pot == 26)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 26");
    }
  	if (pot == 27)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 27");
    }
  	if (pot == 28)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 28");
    }
  	if (pot == 29)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 29");
    }
  	if (pot == 30)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 30");
    }
  	if (pot == 31)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 31");
    }
  	if (pot == 32)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 32");
    }
  	if (pot == 33)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 33");
    }
  	if (pot == 34)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 34");
    }
  	if (pot == 35)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 35");
    }
  	if (pot == 36)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 36");
    }
  	if (pot == 37)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 37");
    }
  	if (pot == 38)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 38");
    }
  	if (pot == 39)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 39");
    }
  	if (pot == 40)
    {
      lcd_clear();
      lcd_write_string(0,0,"Current Temp 40");
    }
    if (pot > 40)
    {
      lcd_clear();
      lcd_write_string(0,0,"OUTSIDE TEMP");
      lcd_write_string(0,1,"UNBEARABLE");
    }
    if (pot < 0)
    {
      lcd_clear();
      lcd_write_string(0,0,"OUTSIDE TEMP      ");
      lcd_write_string(0,1,"UNBEARABLE    ");
    }





  	// =========================================

    //convert float to a string
    // ftoa(pot, temp_buf, 4);
    // convert uint16_t to string
     snprintf(temp_buf, sizeof(temp_buf), "%d", pot);

   //send serial data
   uart_putstring((unsigned char *) temp_buf);
   uart_putchar('\n');




}

void recieve_character()
{
  char rx_buf;
  rx_buf = uart_getchar();
  if (rx_buf > 300)
  {
    lcd_write_string(0,1,"HI");

  }
}

//INTERRUPT BASED UART FOR ANALOG INPUT

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


void debounce_BUTTON()
{

	TCCR0A = 0x02; // Timer zero - clear timer on compare.
	TCCR0B = 0x04; // 256 prescaler.
	OCR0A = 0x7D;  // 2ms period.
	TIMSK0 = 0x02; // interupt on output compare match A.
}

void setup_button()
{
  debounce_BUTTON();
  CLEAR_BIT(DDRB,5);
  SET_BIT(DDRB,4);
  //pinMode(13, INPUT);
  //pinMode(12, OUTPUT);
}

int tracker_switch = 1;
void button_loop()
{
  pb_now = pushbuttons_db;
  pb_pressed = pb_now & (pb_now ^ pb_prev);
  pb_prev = pb_now;
  //if (BIT_IS_SET(PORTB,5))
  //{
  //  SET_BIT(PORTB,4);
  //}

  if (BIT_IS_SET(PINB,5))
  {
    if (tracker_switch %2)
    {
      SET_BIT(PORTB,4);
      tracker_switch++;
    }
    else
    {
      CLEAR_BIT(PORTB,4);
      tracker_switch++;
    }

  }

}

ISR(TIMER0_COMPA_vect)
{
	uint8_t pb0;		// new value read from push buttons
	uint8_t delta;		// difference between new and present values

	pb0 = ~PINB;		// This reads the new value from the push buttons

	delta = pb0 ^ pushbuttons_db;

	pushbuttons_db ^= (reg2 & delta);

	reg2 = (reg1 & delta);
	reg1 = delta;
}

void setup_lcd(void);
void loop(void);


int main(void)
{
  CLEAR_BIT(DDRB,3);
  CLEAR_BIT(DDRB,2);
  setup_lcd();
  setup_button();
  setup();
  while(1)
  {
    //recieve_character();
    SET_BIT(PORTB,3);
    button_loop();
    process();
    //_delay_ms(10);
    SET_BIT(PORTB,2);
  }
}


void setup_lcd(void)
{
  // set up the LCD in 4-pin or 8-pin mode
  lcd_init();

  // Print a message to the LCD
  lcd_write_string(0, 0, "Getting Ready!");
  lcd_write_string(0, 1, "Hold on (0_0)");
  _delay_ms(1000);

  lcd_clear();

}


/* ********************************************/
// START LIBRARY FUNCTIOMNS

void lcd_init(void){
  //dotsize
  if (LCD_USING_4PIN_MODE){
    _lcd_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  } else {
    _lcd_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  }

  _lcd_displayfunction |= LCD_2LINE;

  // RS Pin
  LCD_RS_DDR |= (1 << LCD_RS_PIN);
  // Enable Pin
  LCD_ENABLE_DDR |= (1 << LCD_ENABLE_PIN);

  #if LCD_USING_4PIN_MODE
    //Set DDR for all the data pins
    LCD_DATA4_DDR |= (1 << 4);
    LCD_DATA5_DDR |= (1 << 5);
    LCD_DATA6_DDR |= (1 << 6);
    LCD_DATA7_DDR |= (1 << 7);

  #else
    //Set DDR for all the data pins
    LCD_DATA0_DDR |= (1 << LCD_DATA0_PIN);
    LCD_DATA1_DDR |= (1 << LCD_DATA1_PIN);
    LCD_DATA2_DDR |= (1 << LCD_DATA2_PIN);
    LCD_DATA3_DDR |= (1 << LCD_DATA3_PIN);
    LCD_DATA4_DDR |= (1 << LCD_DATA4_PIN);
    LCD_DATA5_DDR |= (1 << LCD_DATA5_PIN);
    LCD_DATA6_DDR |= (1 << LCD_DATA6_PIN);
    LCD_DATA7_DDR |= (1 << LCD_DATA7_PIN);
  #endif

  // SEE PAGE 45/46 OF Hitachi HD44780 DATASHEET FOR INITIALIZATION SPECIFICATION!

  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  _delay_us(50000);
  // Now we pull both RS and Enable low to begin commands (R/W is wired to ground)
  LCD_RS_PORT &= ~(1 << LCD_RS_PIN);
  LCD_ENABLE_PORT &= ~(1 << LCD_ENABLE_PIN);

  //put the LCD into 4 bit or 8 bit mode
  if (LCD_USING_4PIN_MODE) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    lcd_write4bits(0b0111);
    _delay_us(4500); // wait min 4.1ms

    // second try
    lcd_write4bits(0b0111);
    _delay_us(4500); // wait min 4.1ms

    // third go!
    lcd_write4bits(0b0111);
    _delay_us(150);

    // finally, set to 4-bit interface
    lcd_write4bits(0b0010);
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);
    _delay_us(4500);  // wait more than 4.1ms

    // second try
    lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);
    _delay_us(150);

    // third go
    lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);
  }

  // finally, set # lines, font size, etc.
  lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);

  // turn the display on with no cursor or blinking default
  _lcd_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  lcd_display();

  // clear it off
  lcd_clear();

  // Initialize to default text direction (for romance languages)
  _lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}


/********** high level commands, for the user! */
void lcd_write_string(uint8_t x, uint8_t y, char string[]){
  lcd_setCursor(x,y);
  for(int i=0; string[i]!='\0'; ++i){
    lcd_write(string[i]);
  }
}

void lcd_write_char(uint8_t x, uint8_t y, char val){
  lcd_setCursor(x,y);
  lcd_write(val);
}

void lcd_clear(void){
  lcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void lcd_home(void){
  lcd_command(LCD_RETURNHOME);  // set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}


// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  lcd_command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    lcd_write(charmap[i]);
  }
}


void lcd_setCursor(uint8_t col, uint8_t row){
  if ( row >= 2 ) {
    row = 1;
  }

  lcd_command(LCD_SETDDRAMADDR | (col + row*0x40));
}

// Turn the display on/off (quickly)
void lcd_noDisplay(void) {
  _lcd_displaycontrol &= ~LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}
void lcd_display(void) {
  _lcd_displaycontrol |= LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

// Turns the underline cursor on/off
void lcd_noCursor(void) {
  _lcd_displaycontrol &= ~LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}
void lcd_cursor(void) {
  _lcd_displaycontrol |= LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_noBlink(void) {
  _lcd_displaycontrol &= ~LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}
void lcd_blink(void) {
  _lcd_displaycontrol |= LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(void) {
  _lcd_displaymode |= LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(void) {
  _lcd_displaymode &= ~LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(void) {
  _lcd_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(void) {
  _lcd_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

/*********** mid level commands, for sending data/cmds */

inline void lcd_command(uint8_t value) {
  //
  lcd_send(value, 0);
}

inline size_t lcd_write(uint8_t value) {
  lcd_send(value, 1);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void lcd_send(uint8_t value, uint8_t mode) {
  //RS Pin
  LCD_RS_PORT &= ~(1 << LCD_RS_PIN);
  LCD_RS_PORT |= (!!mode << LCD_RS_PIN);

  if (LCD_USING_4PIN_MODE) {
    lcd_write4bits(value>>4);
    lcd_write4bits(value);
  } else {
    lcd_write8bits(value);
  }
}

void lcd_pulseEnable(void) {
  //Enable Pin
  LCD_ENABLE_PORT &= ~(1 << LCD_ENABLE_PIN);
  _delay_us(1);
  LCD_ENABLE_PORT |= (1 << LCD_ENABLE_PIN);
  _delay_us(1);    // enable pulse must be >450ns
  LCD_ENABLE_PORT &= ~(1 << LCD_ENABLE_PIN);
  _delay_us(100);   // commands need > 37us to settle
}

void lcd_write4bits(uint8_t value) {
  //Set each wire one at a time

  LCD_DATA4_PORT &= ~(1 << LCD_DATA4_PIN);
  LCD_DATA4_PORT |= ((value & 1) << LCD_DATA4_PIN);
  value >>= 1;

  LCD_DATA5_PORT &= ~(1 << LCD_DATA5_PIN);
  LCD_DATA5_PORT |= ((value & 1) << LCD_DATA5_PIN);
  value >>= 1;

  LCD_DATA6_PORT &= ~(1 << LCD_DATA6_PIN);
  LCD_DATA6_PORT |= ((value & 1) << LCD_DATA6_PIN);
  value >>= 1;

  LCD_DATA7_PORT &= ~(1 << LCD_DATA7_PIN);
  LCD_DATA7_PORT |= ((value & 1) << LCD_DATA7_PIN);

  lcd_pulseEnable();
}

void lcd_write8bits(uint8_t value) {
  //Set each wire one at a time

  #if !LCD_USING_4PIN_MODE
    LCD_DATA0_PORT &= ~(1 << LCD_DATA0_PIN);
    LCD_DATA0_PORT |= ((value & 1) << LCD_DATA0_PIN);
    value >>= 1;

    LCD_DATA1_PORT &= ~(1 << LCD_DATA1_PIN);
    LCD_DATA1_PORT |= ((value & 1) << LCD_DATA1_PIN);
    value >>= 1;

    LCD_DATA2_PORT &= ~(1 << LCD_DATA2_PIN);
    LCD_DATA2_PORT |= ((value & 1) << LCD_DATA2_PIN);
    value >>= 1;

    LCD_DATA3_PORT &= ~(1 << LCD_DATA3_PIN);
    LCD_DATA3_PORT |= ((value & 1) << LCD_DATA3_PIN);
    value >>= 1;

    LCD_DATA4_PORT &= ~(1 << LCD_DATA4_PIN);
    LCD_DATA4_PORT |= ((value & 1) << LCD_DATA4_PIN);
    value >>= 1;

    LCD_DATA5_PORT &= ~(1 << LCD_DATA5_PIN);
    LCD_DATA5_PORT |= ((value & 1) << LCD_DATA5_PIN);
    value >>= 1;

    LCD_DATA6_PORT &= ~(1 << LCD_DATA6_PIN);
    LCD_DATA6_PORT |= ((value & 1) << LCD_DATA6_PIN);
    value >>= 1;

    LCD_DATA7_PORT &= ~(1 << LCD_DATA7_PIN);
    LCD_DATA7_PORT |= ((value & 1) << LCD_DATA7_PIN);

    lcd_pulseEnable();
  #endif
}
