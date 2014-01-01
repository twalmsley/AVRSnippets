/*
 * LCD_2560.h
 *
 * Created: 07/07/2013 09:33:49
 *  Author: tony
 */

extern "C" {
	uint8_t readByte();
	void writeNibble(const uint8_t value);
	void waitUntilNotBusy();
	void writeDataByte(const uint8_t value);
	void writeIntructionByte(const uint8_t value);
	void init_lcd();
	void setPosition(const uint8_t line, const uint8_t column);
	void display(char* value);
	void displayAt(const uint8_t line, const uint8_t column, char* value);
	void to_hex(unsigned char c, char *hex);
};