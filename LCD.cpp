/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 * Copyright (c) 2020, sstaub
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "LCD.h"
#include "mbed.h"

LCD::LCD(PinName rs, PinName en, PinName d4, PinName d5, PinName d6, PinName d7, lcd_t type) : reset(rs), enable(en), data(d4, d5, d6, d7) {
	this->type = type;

	ThisThread::sleep_for(15ms); // Wait 15ms to ensure powered up

	// send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
	for (int i = 0; i < 3; i++) {
		writeByte(0x3);
		ThisThread::sleep_for(2ms); // this command takes 1.64ms, so wait for it
		}

	writeCommand(LCD_4BIT_MODE); // set 4bit mode
	writeCommand(0x28); // function set 001 BW N F - -
	writeCommand(0x0C);
	writeCommand(0x06); // cursor direction and display shift : 0000 01 CD S (CD 0-left, 1-right S(hift) 0-no, 1-yes

	cls();
	}

void LCD::character(uint8_t column, uint8_t row, uint8_t c) {
	int a = address(column, row);
	writeCommand(a);
	writeData(c);
	}

void LCD::cls() {
	writeCommand(0x01); // cls, and set cursor to 0
	ThisThread::sleep_for(2ms); // this command takes 1.64 ms
	locate(0, 0);
	}

void LCD::locate(uint8_t column, uint8_t row) {
	_column = column;
	_row = row;
	}

void LCD::home() {
	writeCommand(LCD_RETURN_HOME);
	ThisThread::sleep_for(2ms); // this command takes a long time!
	}

void LCD::display(modes_t mode) {
	switch(mode) {
		case DISPLAY_ON :
			displaycontrol |= LCD_DISPLAY_ON;
			writeCommand(LCD_DISPLAY_CONTROL | displaycontrol);
			break;
		case DISPLAY_OFF:
			displaycontrol &= ~LCD_DISPLAY_ON;
			writeCommand(LCD_DISPLAY_CONTROL | displaycontrol);
			break;
		case CURSOR_ON:
			displaycontrol |= LCD_CURSOR_ON;
			writeCommand(LCD_DISPLAY_CONTROL | displaycontrol);
			break;
		case CURSOR_OFF:
			displaycontrol &= ~LCD_CURSOR_ON;
			writeCommand(LCD_DISPLAY_CONTROL | displaycontrol);
			break;
		case BLINK_ON:
			displaycontrol |= LCD_BLINK_ON;
			writeCommand(LCD_DISPLAY_CONTROL | displaycontrol);
			break;
		case BLINK_OFF:
			displaycontrol &= ~LCD_BLINK_ON;
			writeCommand(LCD_DISPLAY_CONTROL | displaycontrol);
			break;
		case SCROLL_LEFT:
			writeCommand(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT);
			break;
		case SCROLL_RIGHT:
			writeCommand(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT);
			break;
		case LEFT_TO_RIGHT:
			displaymode |= LCD_ENTRY_LEFT;
			writeCommand(LCD_ENTRY_MODE_SET | displaymode);
			break;
		case RIGHT_TO_LEFT:
			displaymode &= ~LCD_ENTRY_LEFT;
			writeCommand(LCD_ENTRY_MODE_SET | displaymode);
			break;
		case SCROLL_ON:
			displaymode |= LCD_ENTRY_SHIFT_INCREMENT;
			writeCommand(LCD_ENTRY_MODE_SET | displaymode);
			break;
		case SCROLL_OFF:
			displaymode &= ~LCD_ENTRY_SHIFT_INCREMENT;
			writeCommand(LCD_ENTRY_MODE_SET | displaymode);
			break;
		}
	}

void LCD::create(uint8_t location, uint8_t charmap[]) {
	if (location > 7) return;
	location &= 0x7;
	writeCommand(LCD_SET_CGRAM_ADDR | (location << 3));
	for (int i = 0; i < 8; i++) {
		writeData(charmap[i]);
		}
	}

int LCD::_putc(int value) {
	if (value == '\n') {
		_column = 0;
		_row++;
		if (_row >= rows()) {
			_row = 0;
			}
		}
	else {
		character(_column, _row, value);
		_column++;
		if (_column >= columns()) {
			_column = 0;
			_row++;
			if (_row >= rows()) {
				_row = 0;
				}
			}
		}
	return value;
	}

int LCD::_getc() {
	return -1;
	}

void LCD::writeByte(uint8_t value) {
	data = value >> 4;
	wait_us(1); // most instructions take 40us
	enable = 0;
	wait_us(1);
	enable = 1;
	data = value;
	wait_us(1);
	enable = 0;
	wait_us(1);
	enable = 1;
	wait_us(40);
	}

void LCD::writeCommand(uint8_t command) {
	reset = 0;
	writeByte(command);
	}

void LCD::writeData(uint8_t data) {
	reset = 1;
	writeByte(data);
	}

uint8_t LCD::address(uint8_t column, uint8_t row) {
	switch (type) {
		case LCD20x4:
			switch (row) {
				case 0:
					return 0x80 + column;
				case 1:
					return 0xc0 + column;
				case 2:
					return 0x94 + column;
				case 3:
					return 0xd4 + column;
				default:
					return -1;
				}
		case LCD16x2:
		case LCD20x2:
		case LCD40x2:
		default:
			return 0x80 + (row * 0x40) + column;
		}
	}

uint8_t LCD::columns() {
	switch (type) {
		case LCD20x4:
		case LCD20x2:
		case LCD40x2:
			return 20;
		case LCD16x2:
		default:
			return 16;
		}
	}

uint8_t LCD::rows() {
	switch (type) {
		case LCD20x4:
			return 4;
		case LCD16x2:
		case LCD20x2:
		case LCD40x2:
		default:
			return 2;
		}
	}
