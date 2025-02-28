#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Screen
{
private:
    LiquidCrystal_I2C *lcd;

    /* data */
public:
    Screen(){
        lcd = new LiquidCrystal_I2C(0x27, 16, 2);
    };
};

