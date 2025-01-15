#include <AmperkaServo.h>
#include <EEPROM.h>
#define HEADER 0x59
#define dalay_count 10 // по документации 2 но лучше ставить 3
unsigned int Dpoe;
int posVert = 0;  //объявлляем переменные
int deltVert = 10;
int minVert = 40;
int maxVert = 80;
int posHor = 0;
int deltHor = 20;
int minHor = 20;
int maxHor = 80;
bool repeat_indicator = false;
bool flag_stop = false;
// bool flagServo1 = false;
AmperkaServo servo;
AmperkaServo servo1;
// int strong;
// int check;
// int uart[9];
// char stopData[5] = { 0x5A, 0x05, 0x0A, 0x01, 0x6A };         //оставновка данных/общения
char trigData[4] = { 0x5A, 0x04, 0x04, 0x62 };               // запрос данных
char Framerate[6] = { 0x5A, 0x06, 0x03, 0x00, 0x00, 0x63 };  //частота общшеия
// bool flag = false;
// int test = 0;
// int clock1 = 1;
// uint32_t timer;
void setup() {
  Serial.begin(115200);
  delay(150);
  start_serrings();
  Serial.println(get_settings());
  Serial2.begin(115200);  // Задали скорость общения
  delay(150);             // задержка для того чтобы команда уж точно передалась и обработалась
  Serial2.write(Framerate, 6);
  delay(150);

  // servo.attach(5, 544, 2400, 0, 180);
  servo.attach(5);
  servo.writeAngle(servo.getMinAngle());
  // servo1.attach(18, 544, 2400, 0, 180);
  servo1.attach(18);
  servo1.writeAngle(servo.getMinAngle());
  delay(500);
}
void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Pars(data);
  }
  if (Serial2.available())  // Делаем так чтобы мы видели лишь длину, путём фильтрации всего что идёт до 59, после чего ставим флаг и по новой ищем 59 и только после этого выводим данные
  {
    char read = Serial2.read();
    if (read == HEADER) {
      read = Serial2.read();
      if (read == HEADER) {
        char data_buf[7];
        Serial2.readBytes(data_buf, 7);
        int dist = (data_buf[1] * 256 + data_buf[0]) / 10;
        Serial.print(posVert);
        Serial.print('\t');
        Serial.print(posHor);
        Serial.print('\t');
        Serial.println(dist);
        if (!flag_stop)
          ServoHor();
        else
          flag_stop = false;
      }
    }
  }
}

void ServoVert() {
  if (posVert + deltVert <= maxVert) {
    posVert += deltVert;
    servo.writeAngle(posVert);
    // flagServo1 = true;
    delay(500);
  }
}
void ServoHor() {
  if (posHor + deltHor > maxHor) {
    posHor = minHor;
    servo1.writeAngle(posHor);
    delay(200);
    if (posVert + deltVert <= maxVert && maxVert != 0) {
      posVert += deltVert;
      servo.writeAngle(posVert);
      // flagServo1 = true;
      // Serial.print("FLAG\t");
      // Serial.print(posVert);
      // Serial.print("\t");
      // Serial.print(maxVert);
      // Serial.print("\t");
      // Serial.println(deltVert);
      delay(deltVert * dalay_count);
      Serial2.write(trigData, 4);
    } else if (posVert + deltVert > maxVert) {
      posVert = minVert;
      servo.writeAngle(posVert);
      delay(deltVert * dalay_count);
    }
    // if (repeat_indicator)
    //   Serial2.write(trigData, 4);
  } else {
    posHor += deltHor;
    servo1.writeAngle(posHor);
    delay(deltHor * dalay_count);  //2
    Serial2.write(trigData, 4);
  }
}
void Pars(String data) {
  if (data.startsWith("$")) {
    String name = data.substring(1, data.indexOf(','));
    data.remove(0, data.indexOf(',') + 1);
    if (name == "LDESP") {
      String str_buf = data.substring(0, data.indexOf(','));
      // Serial.println(str_buf);
      if (str_buf == "STOP") {
        flag_stop = true;
        servo.writeAngle(minVert);
        servo1.writeAngle(minHor);
      } else if (str_buf == "START")
        ServoStartPoz();
      else if (str_buf == "SAVE")
        save_settings();
      else if (str_buf == "SETTINGS")
        Serial.println(get_settings());
      else if (str_buf == "RESTART_ESP")
        ESP.restart();
      else {
        minVert = str_buf.toInt();
        data.remove(0, data.indexOf(',') + 1);
        maxVert = data.substring(0, data.indexOf(',')).toInt();
        data.remove(0, data.indexOf(',') + 1);
        deltVert = data.substring(0, data.indexOf(',')).toInt();
        data.remove(0, data.indexOf(',') + 1);
        minHor = data.substring(0, data.indexOf(',')).toInt();
        data.remove(0, data.indexOf(',') + 1);
        maxHor = data.substring(0, data.indexOf(',')).toInt();
        data.remove(0, data.indexOf(',') + 1);
        deltHor = data.substring(0, data.indexOf(',')).toInt();
        data.remove(0, data.indexOf(',') + 1);
        repeat_indicator = data.substring(0, data.indexOf(',')).toInt();
        ServoStartPoz();
      }
    }
  }
}
String get_settings() {
  return "$LDESP," + String(minVert) + "," + String(maxVert) + "," + String(deltVert) + ","
         + String(minHor) + "," + String(maxHor) + "," + String(deltHor) + "," + String(repeat_indicator) + ",";
}
void ServoStartPoz() {
  servo.writeAngle(minVert);
  servo1.writeAngle(minHor);
  delay(200);
  Serial2.write(trigData, 4);
}
void save_settings() {
  if (minVert >= 0 && minVert <= 180)
    EEPROM.writeInt(0, minVert);
  if (maxVert >= 0 && maxVert <= 180)
    EEPROM.writeInt(4, maxVert);
  if (deltVert >= 1 && deltVert <= 180)
    EEPROM.writeInt(8, deltVert);
  if (minHor >= 0 && minHor <= 180)
    EEPROM.writeInt(12, minHor);
  if (maxHor >= 0 && maxHor <= 180)
    EEPROM.writeInt(16, maxHor);
  if (deltHor >= 1 && deltHor <= 180)
    EEPROM.writeInt(20, deltHor);
  if (repeat_indicator >= 0 && repeat_indicator <= 180)
    EEPROM.writeBool(25, repeat_indicator);
  EEPROM.commit();
}
void start_serrings() {
  EEPROM.begin(200);
  delay(500);
  if (EEPROM.readInt(0) >= 0 && EEPROM.readInt(0) <= 180)
    minVert = EEPROM.readInt(0);
  if (EEPROM.readInt(4) >= 0 && EEPROM.readInt(4) <= 180)
    maxVert = EEPROM.readInt(4);
  if (EEPROM.readInt(8) >= 1 && EEPROM.readInt(8) <= 180)
    deltVert = EEPROM.readInt(8);
  if (EEPROM.readInt(12) >= 0 && EEPROM.readInt(12) <= 180)
    minHor = EEPROM.readInt(12);
  if (EEPROM.readInt(16) >= 0 && EEPROM.readInt(16) <= 180)
    maxHor = EEPROM.readInt(16);
  if (EEPROM.readInt(20) >= 1 && EEPROM.readInt(20) <= 180)
    deltHor = EEPROM.readInt(20);
  repeat_indicator = EEPROM.readBool(25);
  // ServoStartPoz();
}