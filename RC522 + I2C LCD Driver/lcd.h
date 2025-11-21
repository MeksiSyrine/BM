#ifndef __lcd__h
#define __lcd__h
void lcd_init (void);
void lcd_send_string (char *str);
void setCursor(int a, int b);
void lcd_clear (void);
#endif