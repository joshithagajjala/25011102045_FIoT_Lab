#include <reg51.h>
#include <intrins.h>

#define LCD_ROWS 2
#define LCD_COLS 16
#define MAX_DIGITS 5
#define MOVE_CHAR '*'

unsigned char cur_row = 0;
unsigned char cur_col = 0;

sbit RS = P3^2;
sbit EN = P3^3;

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 160; j++);
    }
}

int keypad_scan(void)
{
    static const int keypad[4][4] =
    {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    static const unsigned char rowPin[4] = {7, 6, 5, 4};
    static const unsigned char colPin[4] = {3, 2, 1, 0};

    int row, col;

    for(row = 0; row < 4; row++)
    {
        P2 |= 0xF0;
        P2 &= ~(1 << rowPin[row]);

        for(col = 0; col < 4; col++)
        {
            if((P2 & (1 << colPin[col])) == 0)
            {
                return keypad[row][col];
            }
        }
    }

    return -1;
}

void lcd_port_init(void) {
    P3 = 0x00;   
    RS = 0;
    EN = 0;      
}

//Send a nibble to the LCD
void lcd_nibble(unsigned char nibble, bit rs_val) {
    RS = rs_val;

    P3 = (P3 & 0x0F) | ((nibble & 0x0F) << 4);  

    EN = 1;
    _nop_();
    _nop_();
    EN = 0;
}

//Send a command to the LCD
void lcd_cmd(unsigned char val) {
    unsigned char upper, lower;

    upper = (val >> 4) & 0x0F;
    lower = val & 0x0F;

    lcd_nibble(upper, 0);   
    lcd_nibble(lower, 0);   
}

//Send data to the LCD
void lcd_data(unsigned char val) {
    unsigned char upper, lower;

    upper = (val >> 4) & 0x0F;
    lower = val & 0x0F;

    lcd_nibble(upper, 1);   
    lcd_nibble(lower, 1);   
}

//Initialize the LCD
void lcd_init(void) {
    lcd_port_init();
    delay_ms(20);            

    lcd_nibble(0x03, 0); 
    delay_ms(5);           
    lcd_nibble(0x03, 0); 
    delay_ms(1);           
    lcd_nibble(0x03, 0); 
    delay_ms(1);           
    lcd_nibble(0x02, 0); //Set 4-bit mode
    delay_ms(1);  
    
    lcd_cmd(0x28); //Function set: 4-bit, 2 lines, 5x8 dots
    lcd_cmd(0x08); //Display OFF, Cursor OFF, Blink OFF
    lcd_cmd(0x01); //Clear display
    delay_ms(2);

    lcd_cmd(0x06); //Entry mode set: Increment cursor, no shift
}

void lcd_str(char arr[]) {
    int i = 0;
    while (arr[i] != '\0') {
        lcd_data(arr[i]);
        i++;
    }
}

//Move the cursor to a specific row and column
void lcd_goto(unsigned char row, unsigned char col)
{
    unsigned char address;

    if (row >= LCD_ROWS || col >= LCD_COLS)
        return;

    address = row*0x40 + col;

    lcd_cmd(0x80 | address);
}

//Display a string at a specific row and column
void lcd_puts(unsigned char row, unsigned char col, char *str) {
    lcd_goto(row, col);
    lcd_str(str);
}

//Send a number to the LCD
void lcd_print_number(unsigned int value)
{
    unsigned char digits[MAX_DIGITS];
    unsigned char i = 0;

    if (value == 0) {
        lcd_data('0');
        return;
    }

    while (value > 0) {
        digits[i] = value % 10;
        value = value / 10;
        i++;
    }

    while (i > 0) {
        i--;
        lcd_data(digits[i] + '0');
    }
}

void move_char(char direction) {
    lcd_goto(cur_row, cur_col);
    lcd_data(' ');

    switch (direction) {
        case '6':  // right
            if (cur_col < LCD_COLS - 1) cur_col++;
            break;
        case '4':  // left
            if (cur_col > 0) cur_col--;
            break;
        case '8':  // down
            if (cur_row < LCD_ROWS - 1) cur_row++;
            break;
        case '2':  // up
            if (cur_row > 0) cur_row--;
            break;
    }

    lcd_goto(cur_row, cur_col);
    lcd_data(MOVE_CHAR);
}

void main(void) {
    int key;
    lcd_init();
    
    lcd_cmd(0x0C); // Display ON, Cursor OFF, Blink OFF
    
    // Demonstrate Task 7A & Task 7B: Strings, both rows, and dynamic number display
    lcd_puts(0, 0, "Hello, World!");
    lcd_puts(1, 0, "Count: ");
    lcd_print_number(100);
    delay_ms(2000); // Display initial message and number for 2 seconds
    
    lcd_cmd(0x01);  // Clear display
    delay_ms(2);
    
    // Task 7C: Initialize moving character at (0, 0)
    cur_row = 0;
    cur_col = 0;
    lcd_goto(cur_row, cur_col);
    lcd_data(MOVE_CHAR);
    
    while(1) {
        key = keypad_scan();
        if (key == '2' || key == '4' || key == '6' || key == '8') {
            move_char((char)key);
            delay_ms(200);   // simple debounce - avoid one press registering many moves
        }
    }
}
