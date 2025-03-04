#include <Arduino.h>
#include "button.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Подключение библиотеки

LiquidCrystal_I2C lcd(0x27, 16, 2); // Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой

Button btnNextScr(3);
Button btnOffScr(5);

int CurrentScr = 0; // 0 - cpu/ram; 1 - temperature; 2 - network speed; 3 - disk speed

float percentCPU = 0.0;
float percentMEM = 0.0;
float tempCPU = 0.0;
float netU = 0.0;
float netD = 0.0;
float diskR = 0.0;
float diskW = 0.0;

String templMBc = "Mb/c";

bool StateScreen = true;

uint32_t toneTimer = 0;

void ViewStringOnScreen(int numberString, String stringBegin, String stringEnd)
{
  lcd.setCursor(0, numberString);         
  lcd.print(stringBegin); 
  int cursorPos = 16 - stringEnd.length();
  lcd.setCursor(cursorPos, numberString);
  lcd.print(stringEnd);
}

void ViewDataOnScreen(String firstStringBegin, String firstStringEnd, String secondStringBegin, String secondStringEnd)
{
  lcd.clear();
  ViewStringOnScreen(0, firstStringBegin,firstStringEnd);
  ViewStringOnScreen(1, secondStringBegin,secondStringEnd);
}

void ChangeBackLightOnScreen()
{
  lcd.setBacklight(StateScreen);
  StateScreen = !StateScreen;
}

void UpdateScreen()
{
  String firstStringBegin,firstStringEnd = "";
  String secondStringBegin, secondStringEnd = "";

  if (CurrentScr == 0)
  {
    firstStringBegin = "CPU";
    firstStringEnd = String(percentCPU) + "%";
    secondStringBegin = "MEM";
    secondStringEnd = String(percentMEM) + "%";
  }
  else if (CurrentScr == 1)
  {
    firstStringBegin = "      TEMP";
    firstStringEnd = " ";
    secondStringBegin = "  ";
    secondStringEnd = String(tempCPU) + " C.    ";
  }
  else if (CurrentScr == 2)
  {
    firstStringBegin = "NetU";
    firstStringEnd = String(netU) + templMBc;
    secondStringBegin = "NetD";
    secondStringEnd = String(netD) + templMBc;
  }
  else if (CurrentScr == 3)
  {
    firstStringBegin = "DiskR";
    firstStringEnd = String(diskR) + templMBc;
    secondStringBegin = "DiskW";
    secondStringEnd = String(diskW) + templMBc;
  }
  ViewDataOnScreen(firstStringBegin,firstStringEnd, secondStringBegin,secondStringEnd);
}
void WaitScreen()
{
  CurrentScr = 0;
}
void NextScreen()
{
  CurrentScr++;
  if (CurrentScr > 3)
  {
    CurrentScr = 0;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(100);

  lcd.init();                // Инициализация дисплея
  ChangeBackLightOnScreen(); // Подключение подсветки
}

void loop()
{
  btnNextScr.process_button();
  btnOffScr.process_button();

  if (btnNextScr.is_clicked())
  {
    NextScreen();
    UpdateScreen();
  }

  if (btnOffScr.is_clicked())
  {
    ChangeBackLightOnScreen();
  }

  if (Serial.available())
  {
    String val = Serial.readString();
    // CPU%  + MEM% + TEMP + NetU + NetD +DiskR+DiskW
    // 003.10_041.70_039.00_008.21_001.25_045.23_055.17
    // 103.12241.71039.23248.21501.25645.23755.17
    percentCPU = val.substring(0, 6).toFloat();
    percentMEM = val.substring(6, 12).toFloat();
    tempCPU = val.substring(12,18).toFloat();
    netU = val.substring(18,24).toFloat();
    netD = val.substring(24,30).toFloat();
    diskR = val.substring(30,36).toFloat();
    diskW = val.substring(36).toFloat();

    UpdateScreen();
  }

  if(tempCPU > 64.5 && millis() - toneTimer > 2000)
  {
    tone(8, 523, 600);
    toneTimer = millis();
    
  }

}
