#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define CE_PIN 9
#define CSN_PIN 10

#define JOY_X A0
#define JOY_Y A1
#define JOY_BUTTON 3  // кнопка джойстика, HIGH = нажатие

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "NODE1";  // Адрес устройства (трубопровод)

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C адрес и размеры LCD экрана (16x2)

struct JoyData {
  int8_t x;
    int8_t y;
      const char* mode;
      };

      int X_CENTER = 512;
      int Y_CENTER = 512;
      #define DEADZONE -1

      bool scanning = false; // флаг режима сканирования

      // Функция калибровки джойстика
      void calibrateJoystick() {
        long sumX = 0, sumY = 0;
          const int samples = 50;
            for (int i = 0; i < samples; i++) {
                sumX += analogRead(JOY_X);
                    sumY += analogRead(JOY_Y);
                        delay(10);
                          }
                            X_CENTER = sumX / samples;
                              Y_CENTER = sumY / samples;
                                Serial.print("Center X: ");
                                  Serial.print(X_CENTER);
                                    Serial.print(" Y: ");
                                      Serial.println(Y_CENTER);
                                      }

                                      // Маппинг данных джойстика с учетом мертвой зоны
                                      int8_t mapJoyCalibrated(int val, int center) {
                                        int deg;
                                          if (val < center)
                                              deg = map(val, 0, center, -40, 0);
                                                else
                                                    deg = map(val, center, 1023, 0, 40);

                                                      if (abs(deg) <= DEADZONE) deg = 0;

                                                        return constrain(deg, -40, 40);
                                                        }

                                                        // Отправка данных и вывод на LCD
                                                        void sendAndDisplay(int8_t x, int8_t y, const char* mode) {
                                                          JoyData data = {x, y, mode};
                                                            radio.stopListening();
                                                              radio.write(&data, sizeof(data));
                                                                radio.startListening();

                                                                  // Очищаем LCD и выводим данные
                                                                    lcd.clear();
                                                                      
                                                                        // Отображаем ID устройства и первую букву режима работы на первой строке
                                                                          lcd.setCursor(0, 0);
                                                                            lcd.print("ID: ");
                                                                              lcd.print((const char*)address);  // Преобразуем массив байтов в строку и выводим
                                                                                lcd.print(" ");
                                                                                  lcd.print(mode[0]);  // Первая буква режима работы
                                                                                    
                                                                                      // Отображаем координаты на второй строке
                                                                                        lcd.setCursor(0, 1);
                                                                                          lcd.print("X: ");
                                                                                            lcd.print(x);
                                                                                              lcd.print(" Y: ");
                                                                                                lcd.print(y);

                                                                                                  Serial.print("Sent X: ");
                                                                                                    Serial.print(x);
                                                                                                      Serial.print(" | Sent Y: ");
                                                                                                        Serial.println(y);
                                                                                                        }

                                                                                                        // Функции для различных типов сканирования:
                                                                                                        void horizontalScan() {
                                                                                                          for (int y = -40; y <= 41; y += 10) {
                                                                                                              sendAndDisplay(0, y, "Horizontal Scan");
                                                                                                                  delay(2900);
                                                                                                                    }
                                                                                                                    }

                                                                                                                    void verticalScan() {
                                                                                                                      for (int x = -40; x <= 41; x += 10) {
                                                                                                                          sendAndDisplay(x, 0, "Vertical Scan");
                                                                                                                              delay(2900);
                                                                                                                                }
                                                                                                                                }

                                                                                                                                void diagonalScan1() {
                                                                                                                                  for (int a = -40; a <= 41; a += 10) {
                                                                                                                                      sendAndDisplay(a, a, "Diagonal Scan 1");
                                                                                                                                          delay(2900);
                                                                                                                                            }
                                                                                                                                            }

                                                                                                                                            void diagonalScan2() {
                                                                                                                                              for (int b = -40; b <= 41; b += 10) {
                                                                                                                                                  sendAndDisplay(b, -b, "Diagonal Scan 2");
                                                                                                                                                      delay(2900);
                                                                                                                                                        }
                                                                                                                                                        }

                                                                                                                                                        void resetPosition() {
                                                                                                                                                          sendAndDisplay(0, 0, "Reset Position");
                                                                                                                                                            delay(3000);
                                                                                                                                                            }

                                                                                                                                                            void scanRoutine() {
                                                                                                                                                              scanning = true;
                                                                                                                                                                horizontalScan();
                                                                                                                                                                  verticalScan();
                                                                                                                                                                    diagonalScan1();
                                                                                                                                                                      diagonalScan2();
                                                                                                                                                                        resetPosition();
                                                                                                                                                                          scanning = false;
                                                                                                                                                                          }

                                                                                                                                                                          void setup() {
                                                                                                                                                                            Serial.begin(9600);
                                                                                                                                                                              lcd.init();
                                                                                                                                                                                lcd.backlight();
                                                                                                                                                                                  lcd.clear();
                                                                                                                                                                                    lcd.setCursor(0, 0);
                                                                                                                                                                                      lcd.print("Transmitter Ready");

                                                                                                                                                                                        pinMode(JOY_BUTTON, INPUT); // кнопка джойстика для активации сканирования

                                                                                                                                                                                          if (!radio.begin()) {
                                                                                                                                                                                              Serial.println("NRF24 init failed!");
                                                                                                                                                                                                  while (1);
                                                                                                                                                                                                    }
                                                                                                                                                                                                      radio.setPALevel(RF24_PA_HIGH);
                                                                                                                                                                                                        radio.openWritingPipe(address);
                                                                                                                                                                                                          radio.stopListening();

                                                                                                                                                                                                            randomSeed(analogRead(0));

                                                                                                                                                                                                              calibrateJoystick(); // Калибровка джойстика
                                                                                                                                                                                                              }

                                                                                                                                                                                                              void loop() {
                                                                                                                                                                                                                // Проверка нажатия кнопки для запуска сканирования
                                                                                                                                                                                                                  if (digitalRead(JOY_BUTTON) == HIGH && !scanning) {
                                                                                                                                                                                                                      scanRoutine();
                                                                                                                                                                                                                        }

                                                                                                                                                                                                                          // Ручное управление джойстиком
                                                                                                                                                                                                                            if (!scanning) {
                                                                                                                                                                                                                                int rawX = analogRead(JOY_X);
                                                                                                                                                                                                                                    int rawY = analogRead(JOY_Y);

                                                                                                                                                                                                                                        int8_t x = mapJoyCalibrated(rawX, X_CENTER);
                                                                                                                                                                                                                                            int8_t y = mapJoyCalibrated(rawY, Y_CENTER);

                                                                                                                                                                                                                                                sendAndDisplay(x, y, "Manual mode");
                                                                                                                                                                                                                                                    delay(200); // обновление вручную
                                                                                                                                                                                                                                                      }
                                                                                                                                                                                                                                                      }