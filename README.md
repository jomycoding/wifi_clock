# wifi_clock
ESP8266 wifi oled clock with korean font

Wemos D1R1
OLED GPio Setting

U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4, /* cs=*/ U8X8_PIN_NONE, /* dc=*/ 2, /* reset=*/ 16 ); //성공 6pin
//U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 2, /* reset=*/ 16); //성공 7pin
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4, /* reset=*/ U8X8_PIN_NONE); //성공
