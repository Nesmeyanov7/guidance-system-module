#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define CE_PIN 9
#define CSN_PIN 10
#define LASER_PIN 7

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "NODE1";  // Адрес для связи

Servo yawServo;
Servo pitchServo;

#define YAW_PIN 5
#define PITCH_PIN 6

#define DEADZONE 3  // Мертвая зона джойстика (настраиваемая)

#define SERVO_MIN -40  // Минимальный угол для сервопривода
#define SERVO_MAX  40  // Максимальный угол для сервопривода

// Структура для передачи данных джойстика
struct JoyData {
  int8_t x;
    int8_t y;
    };

    int8_t applyDeadzone(int8_t val) {
      if (abs(val) <= DEADZONE) return 0;
        return val;
        }

        // Ограничение движения серво по заданным углам
        int constrainServo(int val) {
          return constrain(val, SERVO_MIN, SERVO_MAX);
          }

          void setup() {
            Serial.begin(9600);

              yawServo.attach(YAW_PIN);  // Подключение серво на пин Yaw
                pitchServo.attach(PITCH_PIN);  // Подключение серво на пин Pitch

                  yawServo.write(90);  // Центрируем серво на 0°
                    pitchServo.write(90);  // Центрируем серво на 0°
                      
                        pinMode(LASER_PIN, OUTPUT);  // Настроим пин для лазера
                          digitalWrite(LASER_PIN, LOW);  // Лазер выключен по умолчанию

                            if (!radio.begin()) {
                                Serial.println("NRF24 init failed!");
                                    while (1);
                                      }

                                        radio.setPALevel(RF24_PA_HIGH);  // Устанавливаем мощность
                                          radio.openReadingPipe(1, address);  // Открываем канал для приема
                                            radio.startListening();  // Начинаем прослушивание

                                              Serial.println("Receiver ready");
                                              }

                                              void loop() {
                                                if (radio.available()) {  // Проверка, есть ли данные от передатчика
                                                    JoyData data;
                                                        radio.read(&data, sizeof(data));  // Получаем данные с передатчика

                                                            // Применяем мертвую зону
                                                                data.x = applyDeadzone(data.x);
                                                                    data.y = applyDeadzone(data.y);

                                                                        // Ограничиваем движение сервоприводов
                                                                            int X = constrainServo(data.x);
                                                                                int Y = constrainServo(data.y);

                                                                                    // Управление серво моторами
                                                                                        yawServo.write(90 + X);  // Поворот по оси Yaw
                                                                                            pitchServo.write(90 + Y);  // Наклон по оси Pitch
                                                                                                
                                                                                                    // Отображаем данные на мониторе
                                                                                                        Serial.print("X: ");
                                                                                                            Serial.print(data.x);
                                                                                                                Serial.print(" | Servo: ");
                                                                                                                    Serial.print(90 + X);
                                                                                                                        Serial.print(" | Y: ");
                                                                                                                            Serial.print(data.y);
                                                                                                                                Serial.print(" | Servo: ");
                                                                                                                                    Serial.println(90 + Y);
                                                                                                                                      }

                                                                                                                                        // Здесь можно добавить логику для активации лазера в зависимости от состояния
                                                                                                                                          // Например, включаем лазер, если на вход поступает определенная команда
                                                                                                                                            // Лазер активируется, когда джойстик имеет определённое значение по оси X
                                                                                                                                              if (abs(pitchServo.read() - 90) > 10 || abs(yawServo.read() - 90) > 10) {
                                                                                                                                                  digitalWrite(LASER_PIN, HIGH);  // Включаем лазер, если серво отклонены больше чем на 10°
                                                                                                                                                    } else {
                                                                                                                                                        digitalWrite(LASER_PIN, HIGH);  // Отключаем лазер, если серво в центре
                                                                                                                                                          }

                                                                                                                                                            delay(20);  // Маленькая задержка для стабильной работы
                                                                                                                                                            }
