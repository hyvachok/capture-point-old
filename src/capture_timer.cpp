#define buttonPin1 7 // *кнопка на корпусе
#define buttonPin2 6 // *кнопка админа
#define piezoPin 9   // *пьезодинамик

#include <Arduino.h>
#include <Wire.h>              //либа для работы с l2c
#include <LiquidCrystal_I2C.h> //либа для работы с ЖК l2c экранами

LiquidCrystal_I2C lcd(0x27, 16, 2); // инит экрана и его размера
int buttonState1 = 0;               //переменная для хранения состояния кнопки
int buttonState2 = 0;

char letters[15] = "0123456789ABCD";      //набор допустимых символов для кода
char code[6];                             //переменная для хранения кода
String clearDisplay = "                "; //для очистки экрана

int counter = 0;    //счетчик нажатий кнопки
int timerState = 3; // выбранный таймстамп (раньше было количество минут)
long timer1 = 480000;
long timer2 = 300000;
long timer3 = 600000;
String codeMessage = "Code ";
long timerTime = timer2; //стандартное значение таймера
boolean isTimer = false; //состояние таймера
boolean isWork = false;  //* true - режим таймера false - режим настройки

// метод для генерации рандомного кода
void generateCode()
{
  for (int i = 0; i < 5; i++)
  {
    code[i] = letters[random(0, 13)];
  }
}

// метод показывающий на экране код
void showCode()
{
  counter++; //при показе кода прибавляется счетчик
  codeMessage = "Code ";
  lcd.setCursor(0, 0);
  lcd.print(clearDisplay);
  lcd.setCursor(0, 1);
  lcd.print(codeMessage);
  lcd.print(code);
  tone(piezoPin, 1000);
  delay(30000);
  noTone(piezoPin);
  lcd.setCursor(0, 1);
  lcd.print(clearDisplay); // очистка экрана
  lcd.noBacklight();       //выключение подстветки
}
// метод показывающий на экране счетчик нажатий кнопки
void showCounter()
{
  lcd.setCursor(0, 0);
  lcd.print("Code was shown");
  lcd.setCursor(0, 1);
  lcd.print(counter);
  lcd.print(" times");
  delay(5000);
}

//метод показывающий на экране время
void TimePrint()
{
  int time = timerTime / 1000;
  if (time / 60 % 60 < 10)
  {
    lcd.print("0");
  }
  lcd.print((time / 60) % 60);
  lcd.print(":");
  if (time % 60 < 10)
  {
    lcd.print("0");
  }
  lcd.print(time % 60);
}

void setup()
{
  randomSeed(analogRead(0));         // генератор шума на нулевом(0) аналоговом пине
  pinMode(buttonPin1, INPUT_PULLUP); // объявление кнопки с подтяжкой к земле
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(piezoPin, OUTPUT);
  lcd.init();      // инит экрана
  lcd.backlight(); // включение подстветки экрана
  generateCode();
}

void loop()
{
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  // проверяем нажата ли кнопка
  // если нажата, то buttonState1 будет LOW:
  if (buttonState1 == LOW && isWork == true)
  {
    lcd.backlight(); // включение подстветки экрана
    isTimer = true;
    lcd.setCursor(0, 0);
    lcd.print(clearDisplay); // очистка экрана
    lcd.setCursor(0, 1);
    lcd.print(clearDisplay); // очистка экрана
  }
  //если нажама внешняя кнопка при режиме настройки
  if (buttonState1 == LOW && isWork == false)
  {
    isWork = true;
    lcd.noBacklight();
    tone(piezoPin, 500);
    delay(2000); //защита от ложного первого срабатывания
    noTone(piezoPin);
  }
  //если нажата кнопка 2во время рабочего режима
  if (buttonState2 == LOW && isWork == true)
  {
    lcd.backlight();
    showCounter();
    isWork = false;
    isTimer = false;
    generateCode();
    codeMessage = "New code ";
  }
  //если нажата кнопка в режиме настройки меняется количество минут в таймере
  //TODO надо проверить работоспособность после изменения найзваний таймстампов
  if (buttonState2 == LOW && isWork == false)
  {
    if (timerState == 1)
    {
      timerState = 2;
    }
    else if (timerState == 3)
    {
      timerState = 1;
    }
    else if (timerState == 2)
    {
      timerState = 3;
    }
    else if (timerState != 2 && timerState != 3 && timerState != 1)
    {
      timerState = 3;
    }
    if (timerState == 2)
    {
      timerTime = timer1;
    }
    else if (timerState == 3)
    {
      timerTime = timer2;
    }
    else if (timerState == 1)
    {
      timerTime = timer3;
    }
    lcd.setCursor(0, 1);
    lcd.print(clearDisplay); // очистка экрана
  }
  // режим настройки
  if (isWork == false)
  {
    counter = 0;
    lcd.setCursor(0, 0);
    lcd.print(codeMessage);
    lcd.print(code);
    lcd.setCursor(0, 1);
    lcd.print("set minutes: ");
    lcd.print(((timerTime / 1000) / 60) % 60);
  }
  //когда таймер 0 восстановление состояния
  if (timerTime <= 0)
  {
    if (timerState == 2)
    {
      timerTime = timer1;
    }
    if (timerState == 3)
    {
      timerTime = timer2;
    }
    if (timerState == 1)
    {
      timerTime = timer3;
    }
    isTimer = false;
    showCode();
  }

  // если таймер активен вычитать по 100 мс
  if (isTimer == true)
  {
    timerTime -= 100;
  }
  // показ времени
  if (isWork == true)
  {
    lcd.setCursor(5, 0);
    TimePrint();
    delay(100);
  }
}
