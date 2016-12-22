#include "main.h"

// Ticker for base timer
Ticker tick(TIM16);

// Segment array (a b c d e f g)
DigitalOut segment[] = {DigitalOut(&PA4), DigitalOut(&PA6), DigitalOut(&PB15), DigitalOut(&PA11),
                        DigitalOut(&PA12), DigitalOut(&PA5), DigitalOut(&PB14)};

												
// Digit array (d4 d3 d2 d1)
DigitalOut digit[] = {DigitalOut(&PB2, GPIO_OType_OD), DigitalOut(&PB1, GPIO_OType_OD),
											DigitalOut(&PB0, GPIO_OType_OD), DigitalOut(&PA7, GPIO_OType_OD)};

// Dots
DigitalOut dots(&PA8);

// One seven segment is composed of an array of segment and one digit pin
SevenSeg sevenSeg0(segment, &digit[0]);
SevenSeg sevenSeg1(segment, &digit[1]);
SevenSeg sevenSeg2(segment, &digit[2]);
SevenSeg sevenSeg3(segment, &digit[3]);

// Create group of seven segment
SevenSegGroup sevenSegGroup;
											
// InterruptIn (right encoder)
InterruptIn encoderRightChA(&PA0);
InterruptIn encoderRightChB(&PA1);

// InterruptIn (left encoder)
InterruptIn encoderLeftChA(&PB8);
InterruptIn encoderLeftChB(&PB9);

// Rotary encoders
Encoder encoderRight(&encoderRightChA, &encoderRightChB);
Encoder encoderLeft(&encoderLeftChA, &encoderLeftChB);	
	
// Pusb buttons on encoders
Button buttonRight(PA2);
Button buttonLeft(PB5);
	
// led brightness
Brightness brightness(PA3, 1000, TIM15_CH2);

// Serial object
Serial serial(USART1, PA10, PA9);

// GPS
GPS gps(&serial);

// I2C object
I2C	i2c(&I2C_2, &PF7, &PF6);

// RTC
DS3231 ds3231(&i2c);

// Timezone
Timezone tz;

int main(void)
{
	char buffer[5] = {0};
	char result = 0;
	
	Systick_Init();

	// Callback executed every 1 ms (1000 us)
	tick.attach_us(&GlobalBaseTime, 1000);
	
	// Add seven segment into a group
	sevenSegGroup.add(&sevenSeg0);
	sevenSegGroup.add(&sevenSeg1);
	sevenSegGroup.add(&sevenSeg2);
	sevenSegGroup.add(&sevenSeg3);
	
	// InterruptIn callback
	encoderRightChA.risefall(&EncoderRightirqA);
	encoderRightChB.risefall(&EncoderRightirqB);
	encoderLeftChA.risefall(&EncoderLeftirqA);
	encoderLeftChB.risefall(&EncoderLeftirqB);
						
	// Attach brightness variable to encoder
	encoderLeft.attach(brightness.value(), BRIGHTNESS_MAX);
	
	// Set encoder circular
	encoderLeft.circular(1);
	
	// Get RTC time (ds3231)
	ds3231.get(&time);
	
  while (1)
  {		
		// Manage encoders
		encoderLeft.update();
		encoderRight.update();
		
		// Manage push buttons
		buttonLeft.update();
		buttonRight.update();
		
		// Manage GPS receiver
		gps.update();
		
		switch(state[0])
		{
			case TIME_MODE:
				// Attach sub state to right encoder
				encoderRight.attach(&state[1], 2);
			
				// Clear buffer
				memset(buffer, 0x00, 4);
				
				switch(state[1])
				{
					case TIME_MODE_TIME:
						timeout = 0;
					
						// Format display
						sprintf(buffer, "%02d", time.hours);
						sprintf(buffer+2, "%02d", time.minutes);
						break;
					
					case TIME_MODE_TEMP:
						// Dots OFF
						dots = 0;
					
						// Refresh value
						if(timeout == 0)
						{
							timeout = 100; // 100 ms
							
							// Get temperature
							temperature = ds3231.temperature();
						}
						else if(timeout > 100)
						{
							timeout = 100; // 100 ms
						}
					
						// Format display
						if(temperature < 0)
						{
							// Sign minus !
							sprintf(buffer, "%02dC", (int)temperature);
						}
						else sprintf(buffer+1, "%02dC", (int)temperature);
						break;
				}
				
				// Set seven segments
				sevenSegGroup.set(buffer, 4);
			
				// GPS Sync ?
				if(buttonLeft.get() == CONTINUOUS_PRESS)
				{
					timeout = 2000; // 2 sec
					
					sprintf(buffer, "----");
					sevenSegGroup.set(buffer, 4);
					
					state[0] = GPS_SYNC_MODE;
				}
				// Set time ?
				else if(buttonRight.get() == CONTINUOUS_PRESS)
				{
					timeout = 20000; // 20 sec
					
					state[0] = SET_TIME_MODE;
					state[1] = SET_TIME_MODE_INIT;
				}
				break;
			
			case SET_TIME_MODE:	
			
				switch(state[1])
				{
					case SET_TIME_MODE_INIT:
						// Save current time 1
						temp[0].hours = time.hours;
						temp[0].minutes = time.minutes;
					
						// Save current time 2
						temp[1].hours = temp[0].hours;
						temp[1].minutes = temp[0].minutes;
					
						// Attach minutes to right encoder
						encoderRight.attach(&temp[0].minutes, 60);
					
						// Minutes segments blinking (500 ms)
						sevenSegGroup.blink(2, 500);
						sevenSegGroup.blink(3, 500);
					
						state[1] = SET_TIME_MODE_MINUTES;
						break;
					
					case SET_TIME_MODE_MINUTES:
						if(buttonLeft.get() == SHORT_PRESS)
						{
							// Attach minutes to right encoder
							encoderRight.attach(&temp[0].hours, 24);
							
							sevenSegGroup.clear();
							
							// Hours segments blinking (500 ms)
							sevenSegGroup.blink(0, 500);
							sevenSegGroup.blink(1, 500);
							
							timeout = 20000; // 20 sec
					
							state[1] = SET_TIME_MODE_HOURS;
						}
						break;
					
					case SET_TIME_MODE_HOURS:
						if(buttonLeft.get() == SHORT_PRESS)
						{
							// Modified temporary value is different to the start value ?
							if((temp[0].hours != temp[1].hours) || (temp[0].minutes != temp[1].minutes))
							{
								time.hours = temp[0].hours;
								time.minutes = temp[0].minutes;
								
								// Reset seconds only if minutes change
								if(temp[0].minutes != temp[1].minutes)
								{
									time.seconds = 0;
								}
								
								// Set RTC time (ds3231)
								ds3231.set(time);
							}
							
							// Stop blinking
							sevenSegGroup.clear();
							
							state[0] = TIME_MODE;
							state[1] = TIME_MODE_TIME;
						}
						break;
				}
			
				// Format display
				sprintf(buffer, "%02d", temp[0].hours);
				sprintf(buffer+2, "%02d", temp[0].minutes);
				
				// Set seven segments
				sevenSegGroup.set(buffer, 4);
				
				// Dots ON
				dots = 1;
				
				// No user action in the last 20 sec ?
				if(timeout == 0)
				{
					// Stop blinking
					sevenSegGroup.clear();
					
					state[0] = TIME_MODE;
					state[1] = TIME_MODE_TIME;
				}
				break;
			
			case GPS_SYNC_MODE:
				if(timeout == 0)
				{
					// Get GPS time
					result = gps.time(&temp[0]);
				
					// GPS fix ?
					if(gps.fix())
					{
						// Get GPS position
						gps.position(&position);
						
						// Timezone search (may take several seconds!)
						if(tz.offset(&temp[1], position))
						{
							// TODO: Apply offset to current time
							
							// Set RTC time (ds3231)
							ds3231.set(time);
							
							state[0] = TIME_MODE;
						}
					}
					else
					{
						// GPS time valid ?
						if(result)
						{
							time.hours = temp[0].hours;
							time.minutes = temp[0].minutes;
							time.seconds = temp[0].seconds;
							
							// Set RTC time (ds3231)
							ds3231.set(time);
						}
						
						state[0] = TIME_MODE;
					}
				}
				break;
		}
		
		// Set brightness
		brightness.set();
	}
}

void GlobalBaseTime(void)
{
	static int millis = 0;
	
	// Manage display
	sevenSegGroup.update();	
	
	sevenSegGroup.timer();
	buttonLeft.timer();
	buttonRight.timer();
	
	if(timeout) timeout--;
	
	// Manage time
	if(millis == 0)
	{
		millis = 1000;
		
		time.seconds++;
		
		if(time.seconds >= 60)
		{
			time.seconds = 0;
			time.minutes++;
		}
		
		if(time.minutes >= 60)
		{
			time.minutes = 0;
			time.hours++;
		}
		
		if(time.hours >= 24)
		{
			time.seconds = 0;
			time.minutes = 0;
			time.hours = 0;
		}
		
		// Dots removed for temperature display
		if((state[0] == TIME_MODE) && (state[1] == TIME_MODE_TEMP))
		{
			// Dots OFF
			dots = 0;
		}
		else
		{
			// Dots ON
			dots = 1;
		}
	}
	else
	{		
		if(millis == 500)
		{
			// Dots OFF
			dots = 0;
		}
		
		millis--;
	}
}

void EncoderRightirqA(void)
{	
	encoderRight.irq();
	
	if(state[0] == SET_TIME_MODE)
	{
		timeout = 20000; // 20 sec
	}
}

void EncoderRightirqB(void)
{
	encoderRight.irq();
	
	if(state[0] == SET_TIME_MODE)
	{
		timeout = 20000; // 20 sec
	}
}

void EncoderLeftirqA(void)
{
	encoderLeft.irq();
}

void EncoderLeftirqB(void)
{
	encoderLeft.irq();
}
