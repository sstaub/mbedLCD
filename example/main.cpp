// example for mbed6 LCD library

#include "mbed.h"
#include "LCD.h"

// special chars
uint8_t upArrow[8] = {  
	0b00100,
	0b01010,
	0b10001,
	0b00100,
	0b00100,
	0b00100,
	0b00000,
	};

uint8_t downArrow[8] = {
	0b00000,
	0b00100,
	0b00100,
	0b00100,
	0b10001,
	0b01010,
	0b00100,
	};

uint8_t rightArrow[8] = {
	0b00000,
	0b00100,
	0b00010,
	0b11001,
	0b00010,
	0b00100,
	0b00000,
	};

uint8_t leftArrow[8] = {
	0b00000,
	0b00100,
	0b01000,
	0b10011,
	0b01000,
	0b00100,
	0b00000,
	};

LCD lcd(D0, D1, D2, D3, D4, D5); // rs, e, d4-d7

int main() {
	// upload user chars to the display
	lcd.create(0, downArrow);
	lcd.create(1, upArrow);
	lcd.create(2, rightArrow);
	lcd.create(3, leftArrow);

	lcd.cls();
	lcd.locate(0, 0);
	lcd.printf("Hello World!\n");
	// print user chars
	lcd.character(0, 1, 0);
	lcd.character(2, 1, 1);
	lcd.character(4, 1, 2);
	lcd.character(6, 1, 3);

	ThisThread::sleep_for(2s);
	lcd.cls();
	lcd.locate(0, 0);
	lcd.printf("Hello World!\n");

	ThisThread::sleep_for(2s);
	lcd.display(DISPLAY_OFF);
	ThisThread::sleep_for(2s);
	lcd.display(DISPLAY_ON);
	ThisThread::sleep_for(2s);
	lcd.display(CURSOR_ON);
	ThisThread::sleep_for(2s);
	lcd.display(BLINK_ON);
	ThisThread::sleep_for(2s);
	lcd.display(BLINK_OFF);
	ThisThread::sleep_for(2s);
	lcd.display(CURSOR_OFF);

	while(1) {
		for (uint8_t pos = 0; pos < 13; pos++) {
			// scroll one position to left
			lcd.display(SCROLL_LEFT);
			// step time
			ThisThread::sleep_for(500ms);
			}

		// scroll 29 positions (string length + display length) to the right
		// to move it offscreen right
		for (uint8_t pos = 0; pos < 29; pos++) {
			// scroll one position to right
			lcd.display(SCROLL_RIGHT);
			// step time
			ThisThread::sleep_for(500ms);
			}

		// scroll 16 positions (display length + string length) to the left
		// to move it back to center
		for (uint8_t pos = 0; pos < 16; pos++) {
			// scroll one position to left
			lcd.display(SCROLL_LEFT);
			// step time
			ThisThread::sleep_for(500ms);
			}
 
		ThisThread::sleep_for(1s);
		}
	}
