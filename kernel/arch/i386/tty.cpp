#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <tty.h>
#include <io.h>
#include "VirtualMemoryManager.h"

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void terminal_init(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;	
	terminal_clear();
	update_cursor(0,0);
	disable_cursor();
}

void terminal_clear()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) 
{
	size_t index = y * VGA_WIDTH + x;
	if(index > (VGA_WIDTH * VGA_HEIGHT))
	{
		index = 0;
		terminal_clear();
		terminal_row = 0;
		terminal_column = 0;
	}
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
	unsigned char uc = c;
	switch(c)
	{
		case '\r':
		case '\n':
			terminal_row++;
			terminal_column = 0;
			break;
		default: 	
			terminal_putentryat(uc, terminal_color, terminal_column, terminal_row); 
			if (++terminal_column == VGA_WIDTH) 
			{
				terminal_column = 0;
				if (++terminal_row == VGA_HEIGHT)
					terminal_row = 0;
			}
			break;
	}
	update_cursor(terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	int len = strlen(data);
	terminal_write(data, len);
	update_cursor(len, terminal_row);
}
