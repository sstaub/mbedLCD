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

#ifndef LCD_H
#define LCD_H

#include "mbed.h"
#include "Stream.h"

/**
 * @brief A TextLCD interface for driving 4-bit HD44780-based LCDs
 *Currently supports 16x2, 20x2 and 20x4 panels
 *
 * @code
 * #include "mbed.h"
 * #include "TextLCD.h"
 *
 * LCD lcd(p10, p12, p15, p16, p29, p30); // rs, e, d4-d7
 *
 * int main() {
 *   lcd.printf("Hello World!\n");
 * }
 * @endcode
 */
class LCD : public Stream {
  public:
    enum lcd_type_t {
        LCD16x2,  // 16x2 LCD panel (default)
        LCD20x2,  // 20x2 LCD panel
        LCD20x4,  // 20x4 LCD panel
        LCD40x2   // 40x2 LCD panel
    };

    enum lcd_mode_t {
        DISPLAY_ON, // Turn the display on
        DISPLAY_OFF, // Turn the display off
        CURSOR_ON, // Turns the underline cursor on
        CURSOR_OFF, // Turns the underline cursor off
        BLINK_ON, // Turn the blinking cursor on
        BLINK_OFF, // Turn the blinking cursor off
        SCROLL_LEFT, // These command scroll the display without changing the RAM
        SCROLL_RIGHT, // These commands scroll the display without changing the RAM
        LEFT_TO_RIGHT, // This is for text that flows Left to right
        RIGHT_TO_LEFT, // This is for text that flows right to lLeft
        SCROLL_ON, // This will 'right justify' text from the cursor
        SCROLL_OFF, // This will 'left justify' text from the cursor
    };

    /**
     * @brief Create a TextLCD interface
     *
     * @param rs    Instruction/data control line
     * @param e     Enable line (clock)
     * @param d4-d7 Data lines for using as a 4-bit interface
     * @param type  Sets the panel size/addressing mode (default = LCD16x2)
     * @param rw    R/W pin
     */
    LCD(PinName rs, PinName en, PinName d4, PinName d5, PinName d6, PinName d7, lcd_type_t type = LCD16x2, PinName rw = NC);

    /**
     * @brief Destructor
     *
     */
    ~LCD();

    /**
     * @brief Initialize the display
     *
     */
    void init();

    /**
     * @brief Clear the screen and locate to 0,0
     *
     */
    void cls();

    /**
     * @brief Locate to a screen column and row
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     */
    void locate(uint8_t column, uint8_t row);

    /**
     * @brief Set cursor position to zero
     *
     */
    void home();

    /**
     * @brief Set display modes
     *
     * @param mode
     */
    void display(lcd_mode_t mode);

    /**
     * @brief Create a user defined char object
     * Allows us to fill the first 8 CGRAM locations
     * with custom characters
     *
     * @param location
     * @param charmap
     */
    void create(uint8_t location, uint8_t charmap[]);

    /**
     * @brief Writes a single char to a given position, usefull for UDC
     *
     * @param column
     * @param row
     * @param c character
     */
    void character(uint8_t column, uint8_t row, uint8_t c);

  protected:
    enum lcd_control_t {
        CTRL_DISPLAY_ON            = 0x04,
        CTRL_DISPLAY_OFF           = 0x00,
        CTRL_CURSOR_ON             = 0x02,
        CTRL_CURSOR_OFF            = 0x00,
        CTRL_BLINK_ON              = 0x01,
        CTRL_BLINK_OFF             = 0x00,
    };

    // Stream implementation functions
    virtual int _putc(int value);
    virtual int _getc();

    uint8_t rows();
    uint8_t columns();
    void writeByte(uint8_t value);
    void writeCommand(uint8_t command);
    void writeData(uint8_t data);

    DigitalOut   _rs;
    DigitalOut   _en;
    DigitalInOut *_rw;
    BusOut _data;
    lcd_type_t _type;

    uint8_t _display_control = CTRL_DISPLAY_OFF | CTRL_CURSOR_OFF | CTRL_BLINK_OFF;
    uint8_t _display_mode = ENTRY_MODE_LEFT | ENTRY_MODE_SHIFT_DECREMENT;

    uint8_t _column;
    uint8_t _row;

    enum lcd_command_t {
        CMD_CLEAR_DISPLAY   = 0x01,
        CMD_RETURN_HOME     = 0x02,
        CMD_ENTRY_MODE_SET  = 0x04,
        CMD_DISPLAY_CONTROL = 0x08,
        CMD_CURSOR_SHIFT    = 0x10,
        CMD_FUNCTION_SET    = 0x20,
        CMD_SET_CGRAM_ADDR  = 0x40,
        CMD_SET_DDRAM_ADDR  = 0x80,
    };
  private:
    enum lcd_function_t {
        FN_8BIT_MODE = 0x10,
        FN_4BIT_MODE = 0x00,
        FN_2LINE     = 0x08,
        FN_1LINE     = 0x00,
        FN_5x10DOTS  = 0x04,
        FN_5x8DOTS   = 0x00,
    };

    enum lcd_entry_mode_t {
        ENTRY_MODE_RIGHT           = 0x00,
        ENTRY_MODE_LEFT            = 0x02,
        ENTRY_MODE_SHIFT_INCREMENT = 0x01,
        ENTRY_MODE_SHIFT_DECREMENT = 0x00,
    };

    enum lcd_shift_t {
        DISPLAY_MOVE = 0x08,
        CURSOR_MOVE  = 0x00,
        MOVE_RIGHT   = 0x04,
        MOVE_LEFT    = 0x00,
    };

    void pulseEnable();
    uint8_t getAddress(uint8_t column, uint8_t row);
};

#endif
