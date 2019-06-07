#define buttonPin1 7 // *кнопка 1 на корпусе
#define buttonPin2 8 // *кнопка 2 на корпусе
#define btn1Led 9    // *диод кнопки 1

#include <Wire.h>              //либа для работы с l2c
#include <LiquidCrystal_I2C.h> //либа для работы с ЖК l2c экранами
#include <avr/sleep.h>
#include <avr/wdt.h>

static int id = 1;          //! ID данной точки, при прошивке обязательно указать необходимую
int minutes = 1;            //! количество минут захвата
long showCodeDelay = 30000; //! сколько секунд показывать код
float multiply = 1.0;       //! множитель на случаи перебоев с таймером

static String codes[8] = { //! массив доступных кодов, нужный выбирается по ID
    "2MP9P6",
    "SF438R",
    "PXY2Z9",
    "1X56XH",
    "HG58IN",
    "5C22SK",
    "BH6F9Y",
    "5BDNY5"};

volatile bool f = 0; //! aaaaaaaaaaaaaa

LiquidCrystal_I2C lcd(0x27, 16, 2); // инит экрана и его размера
int buttonState1 = 0;               //переменная для хранения состояния кнопки
int buttonState2 = 0;
String clearDisplay = "                "; //для очистки экрана
float timer = minutes * 60.0 * 1000.0;    // дополнительные вычисления таймера
int time = 0;                             // переменная для показа текущего времени
String codeMessage = "Code ";
long timerTime = timer;    //стандартное значение таймера
boolean isTimer = false;   //состояние таймера
boolean tik_tok = false;   //переменная для моргания светодиодом
boolean isCapture = false; //состояние захвата

// метод показывающий на экране код
void showCode()
{
  codeMessage = "Code ";
  lcd.setCursor(0, 0);
  lcd.print(clearDisplay);
  lcd.setCursor(0, 1);
  lcd.print(codeMessage);
  lcd.print(codes[id - 1]); // показ кода
  delay(showCodeDelay);
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
  pinMode(buttonPin1, INPUT_PULLUP); // объявление кнопки с подтяжкой к земле
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(btn1Led, OUTPUT);
  lcd.init(); // инит экрана
  lcd.setCursor(0, 1);
  lcd.print(clearDisplay); // очистка экрана
  lcd.setCursor(0, 0);
  lcd.print(clearDisplay); // очистка экран
  lcd.noBacklight();       //выключение подстветки
}

void loop()
{
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  // проверяем нажата ли кнопка
  // если нажата, то buttonState1 будет LOW:
  if (buttonState1 == LOW || buttonState2 == LOW)
  {
    lcd.backlight(); // включение подстветки экрана
    isTimer = true;
    lcd.setCursor(0, 0);
    lcd.print(clearDisplay); // очистка экрана
    lcd.setCursor(0, 1);
    lcd.print(clearDisplay); // очистка экрана
    digitalWrite(btn1Led, HIGH);
    isCapture = true;
  }

  if (isCapture)
  {
    //когда таймер 0 восстановление состояния
    if (timerTime <= 0)
    {
      timerTime = timer * 0.5; // уменьшаем время последующих захватов
      isTimer = false;
      digitalWrite(btn1Led, HIGH);
      showCode();
      digitalWrite(btn1Led, LOW);
      isCapture = false;
    }

    // если таймер активен вычитать по 1000 мс
    if (isTimer == true)
    {
      lcd.setCursor(5, 0);
      TimePrint();
      timerTime -= 1000 * multiply;
      digitalWrite(btn1Led, tik_tok ? HIGH : LOW);
      tik_tok = !tik_tok;
      delay(1000);
    }
  }
  else //если захват не идет то устанавливаем вотчдог
  {
    wdt_enable(WDTO_8S);                 //Задаем интервал сторожевого таймера (8с)
    WDTCSR |= (1 << WDIE);               //Устанавливаем бит WDIE регистра WDTCSR для разрешения прерываний от сторожевого таймера
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Устанавливаем интересующий нас режим
    sleep_mode();                        // Переводим МК в спящий режим
  }
}

ISR(WDT_vect)
{
  wdt_disable();
  f = !f;
}
