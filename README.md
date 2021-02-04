# mbed6 LCD library for HD47780

This library is for the HD47780 LCDs with 4-bit interface for use with mbed 6


## Installation

1. "Download":https://github.com/sstaub/mbedLCD/archive/master.zip the Master branch from GitHub.
2. Unzip and modify the folder name to "mbedLCD"
3. Move the modified folder on your Library folder.


## Example
Here is a simple example which shows the capabilities of the display

```cpp
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

LCD lcd(D0, D1, D2, D3, D4, D5, LCD16x2); // rs, en, d4-d7, LCD type

int main() {
	lcd.init();

	lcd.create(0, downArrow);
	lcd.create(1, upArrow);
	lcd.create(2, rightArrow);
	lcd.create(3, leftArrow);
	
	lcd.cls();
	lcd.locate(0, 0);
	lcd.printf("Hello World!\n");
	lcd.character(0, 1, 0);
	lcd.character(3, 1, 1);
	lcd.character(5, 1, 2);
	lcd.character(7, 1, 3);

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
```

# Documentation

## Display types
following display types are supported
- **LCD16x2** 16x2 LCD panel (default)
- **LCD20x2** 20x2 LCD panel
- **LCD20x4** 20x4 LCD panel
- **LCD40x2** 40x2 LCD panel

## Constructor
create a LCD object

```cpp
LCD::LCD(PinName rs, PinName en, PinName d4, PinName d5, PinName d6, PinName d7, lcd_t type = LCD16x2)
```

create a LCD object

- **rs** reset pin
- **en** enable pin
- **d4** thru **d7** data pins
- **type** display type

**Example**

```cpp
LCD lcd(D0, D1, D2, D3, D4, D5, LCD16x2); // rs, en, d4-d7, LCD type
``

## Class Functions

### Clear Screen

```cpp
void LCD::cls()
```
clears the screen, set cursor to home

**Example**

```cpp
lcd.cls();
```

### Display Modes

```cpp
void LCD::display(modes_t mode)
```

set the modes of the display

- DISPLAY_ON turn the display on
- DISPLAY_OFF turn the display off
- CURSOR_ON turn the underline cursor on
- CURSOR_OFF turn the underline cursor off
- BLINK_ON turn the blinking cursor on
- BLINK_OFF turn the blinking cursor off
- SCROLL_LEFT these command scroll the display without changing the RAM
- SCROLL_RIGHT these command scroll the display without changing the RAM
- LEFT_TO_RIGHT this is for text that flows Left to Right
- RIGHT_TO_LEFT this is for text that flows Right to Left
- AUTOSCROLL_ON this will 'right justify' text from the cursor
- AUTOSCROLL_OFF this will 'left justify' text from the cursor

**Example**

```cpp
lcd.display(CURSOR_ON);
```

### Cursor Location

```cpp
void LCD::locate(uint8_t column, uint8_t row)
```

set poition of the cursor

- **column** position column
- **row** position row

**Example**

```cpp
lcd.locate(0, 0);
```

### Cursor Home

```cpp
void LCD::home()
```
set cursor to home position (0/0)

**Example**

```cpp
lcd.home();
```

### User defined chars

```cpp
void LCD::create(uint8_t location, uint8_t charmap[])
```

Create a user defined char object allows us to fill the first 8 CGRAM locations with custom characters

- **loacation** number 0 - 7
- **charmap** array which contains the char

**Example**

```cpp
uint8_t upArrow[8] = {  
	0b00100,
	0b01010,
	0b10001,
	0b00100,
	0b00100,
	0b00100,
	0b00000,
	};

lcd.create(0, downArrow);
```

### Character

```cpp
void LCD::character(uint8_t column, uint8_t row, uint8_t c) 
```

draw a single character on given position, usefull for user chars

- **column** position column
- **row** position row
- **c** number of the user char

**Example**

```cpp
lcd.character(0, 1, 0);
```
