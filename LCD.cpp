/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 * Copyright (c) 2020, sstaub
 * Copyright (c) 2021, pilotak
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

LCD::LCD(PinName rs, PinName en, PinName d4, PinName d5, PinName d6, PinName d7, PinName rw, lcd_type_t type):
    _rs(rs), _en(en), _data(d4, d5, d6, d7), _type(type) {
    if (rw != NC) {
        _rw = new DigitalInOut(rw);
        _rw->write(1);
    }
}

LCD::~LCD() {
    if (_rw) {
        delete _rw;
    }
}

void LCD::init() {
    _rs.write(0);
    _en.write(0);

    if (_rw) {
        _rw->write(0); // write mode
    }

    for (auto i = 0; i < 3; i++) {
        writeByte(0x3);
        ThisThread::sleep_for(5ms);
    }

    writeCommand(0x02); // 4-bit interface
    uint8_t display_function = FN_4BIT_MODE | FN_5x8DOTS;

    if (rows() == 1) {
        display_function |= FN_1LINE;

    } else if (rows() == 2) {
        display_function |= FN_2LINE;
    }

    writeCommand(CMD_FUNCTION_SET | display_function);

    _display_control = CTRL_DISPLAY_OFF | CTRL_CURSOR_OFF | CTRL_BLINK_OFF;
    writeCommand(CMD_DISPLAY_CONTROL | _display_control);

    cls();

    _display_mode = ENTRY_MODE_LEFT | ENTRY_MODE_SHIFT_DECREMENT;
    writeCommand(CMD_ENTRY_MODE_SET | _display_mode);
}

void LCD::character(uint8_t column, uint8_t row, uint8_t c) {
    uint8_t addr = getAddress(column, row);
    writeCommand(addr);
    writeData(c);
}

void LCD::cls() {
    writeCommand(CMD_CLEAR_DISPLAY);
    locate(0, 0);
    ThisThread::sleep_for(3ms); // this command takes a long time!
}

void LCD::locate(uint8_t column, uint8_t row) {
    _column = column;
    _row = row;
}

void LCD::home() {
    writeCommand(CMD_RETURN_HOME);
    ThisThread::sleep_for(3ms); // this command takes a long time!
}

void LCD::display(lcd_mode_t mode) {
    switch (mode) {
        case DISPLAY_ON :
            _display_control |= CTRL_DISPLAY_ON;
            writeCommand(CMD_DISPLAY_CONTROL | _display_control);
            break;

        case DISPLAY_OFF:
            _display_control &= ~CTRL_DISPLAY_ON;
            writeCommand(CMD_DISPLAY_CONTROL | _display_control);
            break;

        case CURSOR_ON:
            _display_control |= CTRL_CURSOR_ON;
            writeCommand(CMD_DISPLAY_CONTROL | _display_control);
            break;

        case CURSOR_OFF:
            _display_control &= ~CTRL_CURSOR_ON;
            writeCommand(CMD_DISPLAY_CONTROL | _display_control);
            break;

        case BLINK_ON:
            _display_control |= CTRL_BLINK_ON;
            writeCommand(CMD_DISPLAY_CONTROL | _display_control);
            break;

        case BLINK_OFF:
            _display_control &= ~CTRL_BLINK_ON;
            writeCommand(CMD_DISPLAY_CONTROL | _display_control);
            break;

        case SCROLL_LEFT:
            writeCommand(CMD_CURSOR_SHIFT | DISPLAY_MOVE | MOVE_LEFT);
            break;

        case SCROLL_RIGHT:
            writeCommand(CMD_CURSOR_SHIFT | DISPLAY_MOVE | MOVE_RIGHT);
            break;

        case LEFT_TO_RIGHT:
            _display_mode |= ENTRY_MODE_LEFT;
            writeCommand(CMD_ENTRY_MODE_SET | _display_mode);
            break;

        case RIGHT_TO_LEFT:
            _display_mode &= ~ENTRY_MODE_LEFT;
            writeCommand(CMD_ENTRY_MODE_SET | _display_mode);
            break;

        case SCROLL_ON:
            _display_mode |= ENTRY_MODE_SHIFT_INCREMENT;
            writeCommand(CMD_ENTRY_MODE_SET | _display_mode);
            break;

        case SCROLL_OFF:
            _display_mode &= ~ENTRY_MODE_SHIFT_INCREMENT;
            writeCommand(CMD_ENTRY_MODE_SET | _display_mode);
            break;
    }
}

void LCD::create(uint8_t location, uint8_t charmap[]) {
    if (location > 7) {
        return;
    }

    location &= 0x7;
    writeCommand(CMD_SET_CGRAM_ADDR | (location << 3));

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

    } else {
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

uint8_t LCD::getAddress(uint8_t column, uint8_t row) {
    switch (_type) {
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

        default:
            return 0x80 + (row * 0x40) + column;
    }
}

uint8_t LCD::columns() {
    switch (_type) {
        case LCD20x4:
        case LCD20x2:
        case LCD40x2:
            return 20;

        case LCD8x2:
            return 8;

        default:
            return 16;
    }
}

uint8_t LCD::rows() {
    switch (_type) {
        case LCD20x4:
            return 4;

        default:
            return 2;
    }
}

void LCD::writeCommand(uint8_t command) {
    _rs.write(0);
    writeByte(command);
}

void LCD::writeData(uint8_t data) {
    _rs.write(1);
    writeByte(data);
}

void LCD::writeByte(uint8_t value) {
    _data.write(value >> 4); // send upper part first
    pulseEnable();
    _data.write(value & 0b1111);
    pulseEnable();
}

void LCD::pulseEnable() {
    _en.write(0);
    wait_us(1);
    _en.write(1);
    wait_us(1);
    _en.write(0);
    wait_us(40);
}