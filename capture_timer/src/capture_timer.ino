#define buttonPin1 7 // *кнопка на корпусе

#include <Wire.h> //либа для работы с l2c
#include <LiquidCrystal_I2C.h> //либа для работы с ЖК l2c экранами

LiquidCrystal_I2C lcd(0x27,16,2);  // инит экрана и его размера
int buttonState = 0; //переменная для хранения состояния кнопки

char letters[14] = "0123456789ABCD"; //набор допустимых символов для кода
char code[6]; //переменная для хранения кода
String clearDisplay = "                "; //для очистки экрана

long timerTime = 10000; //основной таймер
boolean isTimer = false; //состояние таймера

// метод для генерации рандомного кода
void generateCode() {
  for(int i = 0; i < 5; i++){
    code[i] = letters[random(0, 13)];
  }
}

// метод показывающий на экране код
void showCode() {
  lcd.setCursor(0, 1);
  lcd.print(code);
  delay(5000);
  lcd.setCursor(0, 1);
  lcd.print(clearDisplay);
}

//метод показывающий на экране время
void TimePrint() {
  int time=timerTime/1000;
  if (time/60%60<10) { lcd.print ("0"); }
  lcd.print ((time/60)%60);
  lcd.print (":");
  if (time%60<10) { lcd.print ("0"); }
  lcd.print (time%60);
}

void setup() {
  randomSeed(analogRead(0)); // генератор шума на нулевом(0) аналоговом пине
  pinMode(buttonPin1, INPUT_PULLUP); // объявление кнопки с подтяжкой к земле
  lcd.init(); // инит экрана
  lcd.backlight(); // включение подстветки экрана
  generateCode();

}

void loop() {
  buttonState = digitalRead(buttonPin1);
  // проверяем нажата ли кнопка
  // если нажата, то buttonState будет LOW:
  if (buttonState == LOW) {   
    isTimer = true;
  }

  if (isTimer == true) {
    timerTime-= 100;
  }

  if (timerTime <= 0){
    timerTime = 10000;
    isTimer = false;
    showCode();
  }
  
  lcd.setCursor(0, 0);
  TimePrint();
  delay(100);
}
