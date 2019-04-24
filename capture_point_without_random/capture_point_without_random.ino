#define buttonPin1 7 // *кнопка 1 на корпусе
#define buttonPin2 8 // *кнопка 2 на корпусе
#define btn1Led 9    // *диод кнопки 1

#include <Wire.h>              //либа для работы с l2c
#include <LiquidCrystal_I2C.h> //либа для работы с ЖК l2c экранами

static int id = 1;          //! ID данной точки, при прошивке обязательно указать необходимую
int minutes = 1;            //! количество минут захвата
long showCodeDelay = 30000; //! сколько секунд показывать код
float multiply = 1.0;       //! множитель на случаи перебоев с таймером

static String codes[8] = { //! массив доступных кодов, нужный выбирается по ID
    "2MP9P6",
    "SF438R",
    "PXY2Z9",
    "0XO6XH",
    "HG58IN",
    "5C22SK",
    "BH6F9Y",
    "5BDNY5"};

LiquidCrystal_I2C lcd(0x27, 16, 2); // инит экрана и его размера
int buttonState1 = 0;               //переменная для хранения состояния кнопки
int buttonState2 = 0;
char letters[15] = "0123456789ABCD";      //набор допустимых символов для кода
String clearDisplay = "                "; //для очистки экрана
// * переменные для евент лупа без delay
unsigned long previousMillis = 0;     //время во время предыдущего тика
unsigned long currentMillis = 0;      //текущее время
const long interval = 500 * multiply; //интервал с которым должно происходить событие
boolean tik_tok = false;
// *
float timerF = minutes * 60.0 * 1000.0; // дополнительные вычисления таймера
long timer = timerF * multiply;         // значение таймера захвата
int time = 0;
String codeMessage = "Code ";
long timerTime = timer;  //стандартное значение таймера
boolean isTimer = false; //состояние таймера
boolean isWork = true;   //* true - режим таймера false - режим настройки

// метод показывающий на экране код
void showCode()
{
  codeMessage = "Code ";
  lcd.setCursor(0, 0);
  lcd.print(clearDisplay);
  lcd.setCursor(0, 1);
  lcd.print(codeMessage);
  lcd.print(codes[id - 1]); // показ кода
  delay(showCodeDelay * multiply);
  lcd.setCursor(0, 1);
  lcd.print(clearDisplay); // очистка экрана
  lcd.setCursor(0, 0);
  lcd.print(clearDisplay); // очистка экрана
  lcd.noBacklight();       //выключение подстветки
}
//метод показывающий на экране время
void TimePrint()
{
  time = timerTime / 1000;
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
  pinMode(btn1Led, OUTPUT);
  lcd.init(); // инит экрана
  digitalWrite(btn1Led, HIGH);
}

void loop()
{
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  // проверяем нажата ли кнопка
  // если нажата, то buttonState1 будет LOW:
  if (buttonState1 == LOW || buttonState2 == LOW && isWork == true)
  {
    lcd.backlight(); // включение подстветки экрана
    isTimer = true;
    isWork = false;
    lcd.setCursor(0, 0);
    lcd.print(clearDisplay); // очистка экрана
    lcd.setCursor(0, 1);
    lcd.print(clearDisplay); // очистка экрана
    digitalWrite(btn1Led, HIGH);
  }
  //когда таймер 0 восстановление состояния
  if (timerTime <= 0)
  {
    timerTime = timer * 0.5; // уменьшаем время последующих захватов
    isTimer = false;
    isWork = true;
    showCode();
    digitalWrite(btn1Led, LOW);
  }

  // если таймер активен вычитать по 1000 мс
  if (isTimer == true)
  {
    lcd.setCursor(5, 0);
    TimePrint();
    timerTime -= 1000 * multiply;
    delay(1000 * multiply);
  }
  // евент луп для моргания диодом
  currentMillis = millis();                                         //записываем текущее время
  if (currentMillis - previousMillis >= interval && isWork == true) //если разница между текущим временем и временем предыдущего события больше интервала
  {
    previousMillis = currentMillis; //перезаписываем время срабатывания события
    if (tik_tok)
    {
      digitalWrite(btn1Led, HIGH);
      tik_tok = !tik_tok;
    }
    else
    {
      digitalWrite(btn1Led, LOW);
      tik_tok = !tik_tok;
    }
  }
}
