#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Screen
{
private:
    LiquidCrystal_I2C *_lcd;
    bool stateBackLight;
    int currentScreen = 0; //0-waiting data, 1-cpu/ram; 2-temperature;3-network;4-diskio

    void ViewData(String firstString, String secondString){
        _lcd->clear();
        _lcd->setCursor(0,0);            // Установка курсора в начало первой строки
        _lcd->print(firstString);       // Набор текста на первой строке
        _lcd->setCursor(0, 1);             // Установка курсора в начало второй строки
        _lcd->print(secondString);       // Набор текста на второй строке
    }

    void WaitScreen(){
        currentScreen = 0;
        ViewData("     waiting    ","      data      ");
    }
public:
    Screen(){
        _lcd = new LiquidCrystal_I2C(0x27, 16, 2);
        _lcd->setBacklight(true);
        stateBackLight = true;
    };

    void ChangeBackLightOnScreen()
    {   
        _lcd->setBacklight(stateBackLight);
        stateBackLight = !stateBackLight;
    }

    void NextScreen(){
        currentScreen++;
    }

    void UpdateScreen(){




    }

};

