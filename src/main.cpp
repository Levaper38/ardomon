#include <Arduino.h>
#include "button.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Подключение библиотеки

LiquidCrystal_I2C lcd(0x27, 16, 2); // Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой

Button btnNextScreen(3);
Button btnOffScreen(5);

int CurrentScreen = 0; //0 - cpu/ram; 1 - temperature

float percentCPU = 0.0;
float percentMEM = 0.0;
float tempCPU = 0.0;

String spaces_before_10 =  "     ";
String spaces_before_100 = "    ";
String spaces_equal_100 =  "   ";

bool StateScreen = true;

String align_line(String currentString, float currentValue)
{
  String resultString = currentString;
  if (currentValue < 10) {
    resultString += spaces_before_10 + String(currentValue);
  } else if (currentValue < 100) {
    resultString += spaces_before_100 + String(currentValue);
  } else {
    resultString += spaces_equal_100 + String(currentValue);
  }
  return resultString;
}

void ChangeBackLightOnScreen()
{
  Serial.println("!");
  lcd.setBacklight(StateScreen);
  StateScreen = !StateScreen;
}

void UpdateScreen()
{
  if (CurrentScreen == 0) {
    lcd.clear();

    String firstString = align_line("CPU", percentCPU) + "%";
    String secondString = align_line("MEM", percentMEM) + "%";

    lcd.setCursor(0, 0);             // Установка курсора в начало первой строки
    lcd.print(firstString);       // Набор текста на первой строке
    lcd.setCursor(0, 1);             // Установка курсора в начало второй строки
    lcd.print(secondString);       // Набор текста на второй строке
  } else
  {
    lcd.clear();
    String secondString = align_line(" ", tempCPU) + " C.";
    lcd.setCursor(0, 0);             // Установка курсора в начало первой строки
    lcd.print("      TEMP");       // Набор текста на первой строке
    lcd.setCursor(0, 1);             // Установка курсора в начало второй строки
    lcd.print(secondString);       // Набор текста на второй строке
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(100);

  lcd.init();                      // Инициализация дисплея
  ChangeBackLightOnScreen();       // Подключение подсветки
  UpdateScreen();
}

void loop()
{
  btnNextScreen.process_button();
  btnOffScreen.process_button();

  if(btnNextScreen.is_clicked()){
    if (CurrentScreen == 0) {
      CurrentScreen = 1;
    } else {
      CurrentScreen = 0;
    }
    UpdateScreen();
  }

  if(btnOffScreen.is_clicked()){
    ChangeBackLightOnScreen();
  }

  if (Serial.available()) {
    String val = Serial.readString();
    Serial.println(val);
    //003.10041.70039.00
    // char currentFlag = val[0];
    // float currentValue = val.substring(1).toFloat();

    percentCPU = val.substring(0,5).toFloat();
    percentMEM = val.substring(6,11).toFloat();
    tempCPU = val.substring(12).toFloat();

    UpdateScreen();
  }
}


