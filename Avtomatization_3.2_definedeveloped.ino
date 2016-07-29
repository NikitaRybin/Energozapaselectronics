#include <Multiservo.h>
#include <Wire.h>

//СЕРВОПРИВОДЫ
#define CONV_SERVO 10
#define HITCH_SERVO 11
#define LIFT_SERVO 12
#define CATCH_SERVO 13
#define MOVE_SERVO 14
#define FRAME_SERVO 15
#define SERVO_DELAY 1000

// КНОПКИ
#define ConvButton  0 // кнопка концевика на конвеере
#define LiftUpButton  1 // концевик лифтовой части
#define LiftDownButton  2 // концевик лифтовой части
#define ControlButton1  3 // кнопка для запуска последующего действия
#define ControlButton2  4 // кнопка для запуска последующего действия
#define ControlButton3  3 // кнопка для запуска последующего действия
#define ControlButton4  4 // кнопка для запуска последующего действия
#define SignalButton1  5 // кнопка для оповещения о выъезде механизма в раму
#define SignalButton2  6 // кнопка для оповещения о возврате направляющих
#define InitialButton  7 // кнопка возврата в исходное положение
#define HitchServoButton1  8 // кнопка крутящего механизма
#define HitchServoButton2  9// кнопка крутящего механизма

Multiservo ConvServo; //сервопривод конвеера постоянный
Multiservo HitchServo; // сервопривод первичного захвата постоянный
Multiservo LiftServo; // лифтовый сервопривод постоянный
Multiservo CatchServo; // сервопривод вторичного захвата постоянный
Multiservo MoveServo; // сервопривод выдвигающий направляющие постоянный
Multiservo FrameServo; // сервопривод удерживающий капсулу постоянный

int temp = 0; // подсчет количества действий
int i = 0; // подсчет количества загруженных мешков

void setup (void) {
  // подключаем сервомашинки
  ConvServo.attach (CONV_SERVO);
  HitchServo.attach (HITCH_SERVO);
  LiftServo.attach(LIFT_SERVO);
  CatchServo.attach(CATCH_SERVO);
  MoveServo.attach(MOVE_SERVO);
  FrameServo.attach(FRAME_SERVO);
}
// ВЫСТАВЛЕНИЕ НАЧАЛЬНОГО ПОЛОЖЕНИЯ
void InitialPosition(void) {
  InitialHitchPosition();
  delay(1000);
  InitialCatchPosition();
}
void InitialHitchPosition(void) {
  //возврат первичной системы подхвата в начальное положение
  do // расправить ручки первичного подхвата
  {
    HitchServo.write(0);
  } while (digitalRead(HitchServoButton1) != HIGH);
  HitchServo.write(90);
  delay(1000);
  do {
    LiftServo.write (180); // поднять механизм подхвата вверх
  } while (LiftUpButton != HIGH);
  LiftServo.write(90);
}
void InitialCatchPosition(void) {
  do // направляющие выставить в начальное положение
  {
    MoveServo.write (0);
  } while (digitalRead(SignalButton2 != HIGH));
  MoveServo.write (90);
  delay(1000);
  CatchServo.write(90); // начальное положение машинки вторичного подхвата
}

// ФУНКЦИОНИРОВАНИЕ СИСТЕМЫ
// ФУНКЦИОНИРОВАНИЯ РАБОТЫ КОНВЕЕРА
void ConveyorMoving(void) {
  do
  {
    ConvServo.write (180); // запуск конвеера с максимальной скоростью
  }
  while (digitalRead(ConvButton) != HIGH); // работает до тех пор, пока кнопка не нажата
  ConvServo.write(90);// остановить конвеер !проверить при каком сигнале останавливается!
}
void ReverseConveyorMoving(void) {
  ConvServo.write(0);
  delay(3000);
  ConvServo.write(90);
}
//МЕХАНИЗМ ОПУСКАНИЯ ПЕРВИЧНОГО ПОДХВАТА И ИСХОДНОЙ ПОЗИЦИИ ПОДХВАТА
void LiftPickup() {
  do
  {
    LiftServo.write(0); // опустить первичный подхват вниз
  } while (LiftDownButton != HIGH);
  LiftServo.write (90);
}
void FirstHitch(void) {
  do
  {
    HitchServo.write(180);
  } while (digitalRead(HitchServoButton2) != HIGH);
  HitchServo.write(90);
}


// ПЕРЕНОС МЕШКА МЕЖДУ МЕХАНИЗМАМИ ПОДХВАТОВ
void Transfer (void) {
  LiftServo.write (180); // поднять мешок вверх в течении двух секунд
  delay(2000);
  LiftServo.write(90);
  delay (500);
  HitchServo.write (100); // раздвинул ушки мешка
  delay (500);
  LiftServo.write (180);// еще приподнялся в течении двух секунд
  delay (2000);
  LiftServo.write (90); // остановился
  delay(500);
  HitchServo.write (80);// сдвинул ушки мешка
  delay(500);
  LiftServo.write (0); //опустился в течении двух секунд
  delay(2000);
  LiftServo.write (90);//остановился
  InitialHitchPosition();
}
void ReverseTransfer (void) {
  LiftServo.write (0); // опустить первичный подхват
  delay(1000);
  LiftServo.write(90);
  delay(500);
  FirstHitch(); // подхватить мешок
  delay(1000);
  CatchServo.write (0); // перекинуть мешок на первичный подхват
  delay(1000);
  LiftPickup();// опустить первичный подхват вместе с мешком
  delay(1000);
  InitialHitchPosition(); // вернуть первичный подхват в начальное положение
  delay(1000);
  CatchServo.write(90);
}
void Moving(void) {
  // въезд в раму
  CatchServo.write (180); //собрать лапки механизма вторичного подхвата
  do // загоняем систему в раму
  {
    MoveServo.write (180);
  } while (digitalRead(SignalButton1 != HIGH)); // сработал концевик
  delay (1000); // завели мешок с машинкой подхвата в раму
}
void ReverseMoving(void) {
  CatchServo.write (0);
  do
  {
    MoveServo.write (180);
  } while (digitalRead(SignalButton1 != HIGH)); // сработал концевик
  delay (1000); // завели мешок с машинкой подхвата в раму
}

void Final(void) {
  // переносим ушки мешка на раму
  CatchServo.write (0);
  delay(1000);
  do
  {
    MoveServo.write (0);
  } while (digitalRead(SignalButton2 != HIGH));
  MoveServo.write (90); // установлена начальная позицию для машинки
}
void ReverseFinal (void) {
  CatchServo.write (180);
  delay(1000);
  InitialCatchPosition();
}

// АЛГОРИТМ РАБОТЫ СИСТЕМЫ

void loop (void) {

  // ВЫСТАВЛЕНИЕ НАЧАЛЬНОЙ ПОЗИЦИИ СИСТЕМЫ
  InitialPosition();

  // АЛГОРИТМ РАБОТЫ ПОШАГОВЫЙ

  if (digitalRead(ControlButton1) == HIGH && temp == 0)
  {
    ConveyorMoving();
    temp++;// temp=1
  }
  if (digitalRead(ControlButton1) == HIGH && temp == 1)
  {
    LiftPickup();
    temp++;//temp=2
  }
  if (digitalRead(ControlButton1) == HIGH && temp == 2)
  {
    FirstHitch(); // подхват мешка первичным подхватом
    temp++;//temp=3
  }
  if (digitalRead(ControlButton1) == HIGH && temp == 3)
  {
    Transfer(); // перенос мешка на направляющие
    temp++;//temp=4
  }
  if (digitalRead(ControlButton1) == HIGH && temp == 4)
  {
    Moving(); // въезд в раму
    temp++;//temp=5
  }
  if (digitalRead(ControlButton1) == HIGH && temp == 5)
  {
    Final();
    temp = 0;
    i++;
  } 

  // АЛГОРИТМ РАБОТЫ ОТ И ДО
  if (digitalRead(ControlButton2) == HIGH)
  {
    if (temp == 0) {
      goto label0;
    }
    if (temp == 1) {
      goto label1;
    }
    if (temp == 2) {
      goto label2;
    }
    if (temp == 3) {
      goto label3;
    }
    if (temp == 4) {
      goto label4;
    }
    if (temp == 5) {
      goto label5;
    }
label0:
    ConveyorMoving();
    delay(1000);
label1:
    LiftPickup();
    delay(1000);
label2:
    FirstHitch();
    delay(1000);
label3:
    Transfer();
    delay(1000);
label4:
    Moving();
    delay(1000);
label5:
    Final();
    delay(1000);
    // обнуляем счетчик по окончанию процесса
    //сообщаем о количестве загруженных мешков
    temp = 0;
    i++;
  }
  //ЗАПУСК ДВИЖЕНИЯ СЕРВОПРИВОДА В РАМЕ
  if (i == 1 || i == 2)
  {
    FrameServo.write (180);
    delay (3000);
    FrameServo.write (90);
  }

  // АЛГОРИТМ ОБРАТНЫХ ДЕЙСТВИЙ
  if (digitalRead(ControlButton3) == HIGH )
  {
    // обратный цикл пошагово
  }
  if (digitalRead(ControlButton4) == HIGH)
  {
    // обратный цикл от и до
  }
}





