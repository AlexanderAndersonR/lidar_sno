#include <AmperkaServo.h>
unsigned int Dpoe;
int posVert = 0;  //объявлляем переменные
int deltVert = 10;
int minVert = 40;
int maxVert = 80;
bool flagServo = false;
int posHor = 0;
int deltHor = 20;
int minHor = 20;
int maxHor = 80;
bool flagServo1 = false;
AmperkaServo servo;
AmperkaServo servo1;
int strong;
int check;
int uart[9];
char stopData[5] = { 0x5A, 0x05, 0x0A, 0x01, 0x6A };  //оставновка данных/общения

char trigData[4] = { 0x5A, 0x04, 0x04, 0x62 };  // запрос данных

char Framerate[6] = { 0x5A, 0x06, 0x03, 0x00, 0x00, 0x63 };  //частота общшеия
const int HEADER = 0x59;
bool flag = false;
int test = 0;
byte D;
byte dist[2];
int clock1 = 1;
uint32_t timer;

void setup() {            // здесь включаем то что нужно при запуске
  Serial2.begin(115200);  // Задали скорость общения
  delay(150);             // задержка для того чтобы команда уж точно передалась и обработалась
  //Serial1.write(Baudrate, 8);
  //Serial1.end();
  // Serial1.begin(9600);
  Serial.begin(115200);
  delay(150);
  Serial2.write(Framerate, 6);
  delay(150);
  // servo.attach(5, 544, 2400, 0, 180);
  servo.attach(5);
  servo.writeAngle(servo.getMinAngle());
  // servo1.attach(18, 544, 2400, 0, 180);
  servo1.attach(18);
  servo1.writeAngle(servo.getMinAngle());
  delay(500);
  // put your setup code here, to run once:
}

void loop() {
  for (int i = 0; i < 180; i += 10) {
    servo1.writeAngle(i);
    delay(500);
  }
  // if (Serial.available()) {  //ищим поданное сообщение путём выделения его, поначинаем читатьтьт после $
  //   if (Serial.find('$')) {
  //     String NMEA = Serial.readStringUntil('\n');
  //     //  Serial.println(Svaz(NMEA));() иф на свич поменять
  //     //(проверка на контрольь суммы)
  //     if (Svaz(NMEA) == "LDESP") {  //присваиваем какждому значкению между запятыми переменную
  //       minVert = Svaz(NMEA).toInt();
  //       maxVert = Svaz(NMEA).toInt();
  //       deltVert = Svaz(NMEA).toInt();
  //       minHor = Svaz(NMEA).toInt();
  //       maxHor = Svaz(NMEA).toInt();
  //       deltHor = Svaz(NMEA).toInt();
  //       flagServo1 = true;
  //     }
  //   }
  // }
  // if ((flagServo1) && (millis() - timer > 450)) {
  //   {
  //     Serial2.write(trigData, 4);
  //     timer = millis();
  //   }
  // }

  // // put your main code here, to run repeatedly:    нАПИСАТЬ КОМЕНТАРИЙ
  // if (Serial2.available())  // Делаем так чтобы мы видели лишь длину, путём фильтрации всего что идёт до 59, после чего ставим флаг и по новой ищем 59 и только после этого выводим данные
  // {
  //   flagServo1 = false;
  //   D = Serial2.read();
  //   if (test > 1) {
  //     if (clock1 >= 0) {
  //       // Serial.print(D,HEX);
  //       // Serial.print(' ');
  //       dist[clock1] = D;
  //       clock1 = clock1 - 1;
  //     } else {
  //       Serial.print(posVert);
  //       Serial.print('\t');
  //       Serial.print(posHor);
  //       Serial.print('\t');
  //       Serial.println((dist[0] * 256 + dist[1]) / 10);  // преобразуем данные данные в 16-ти ричной системе в 10-тичьную
  //       test = 0;
  //       clock1 = 1;
  //       ServoHor();
  //       // Serial2.write(trigData, 4);
  //     }
  //   }
  //   if (D == HEADER) {
  //     test++;
  //   }
  // }
}

void ServoVert() {

  if (posVert + deltVert <= maxVert) {  // тут задаём движение в секторе для верттикального сервопривода
    posVert += deltVert;
    servo.writeAngle(posVert);
    flagServo1 = true;
    delay(500);
  }
}
void ServoHor() {

  if (posHor + deltHor > maxHor) {  // тут задаём движение в секторе для горизонтального сервопривода
    posHor = minHor;
    servo1.writeAngle(posHor);
    ServoVert();

  } else {  //условие для того чтобы горизонтальный сервопривод начал движение после того какк вертикальный пройдёт весь свой сектор
    posHor += deltHor;
    servo1.writeAngle(posHor);
    delay(500);
    flagServo1 = true;
  }
}
String Svaz(String& LSP) {  // в данной части мы выделяем из общго масива даных нужные значения, вырезая их до хзапятой.
  String A = LSP.substring(0, LSP.indexOf(','));
  LSP.remove(0, LSP.indexOf(',') + 1);
  return (A);
}
