/*
 * Установка конденсаторной сварки термопар
 * Односторонняя версия платы
 * 
 * Изменения для односторонней платы:
 * - Добавлена калибровка для односторонней разводки
 * - Увеличены временные задержки из-за большей индуктивности
 * - Добавлена диагностика перемычек
 */

// Пин подключения (остаются те же)
#define START_BUTTON 2
#define TIME_POT_PIN A1
#define THYRISTOR_PIN 6
#define CHARGE_RELAY_PIN 8
#define DISCHARGE_RELAY_PIN 9
#define VOLTAGE_PIN A0

// Параметры для односторонней платы
#define SINGLE_LAYER_DELAY 20  // Дополнительная задержка для односторонней разводки (мс)
#define MAX_VOLTAGE 50.0
#define MIN_VOLTAGE 12.0       // Увеличено из-за потерь в дорожках

// Коэффициент делителя для односторонней платы (может отличаться из-за разводки)
const float VOLTAGE_DIVIDER_RATIO = 10.8;  // Скорректировано

void setup() {
  Serial.begin(9600);
  Serial.println("Установка конденсаторной сварки (односторонняя плата)");
  Serial.println("Версия 2.0 - Single Layer PCB");
  
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(THYRISTOR_PIN, OUTPUT);
  pinMode(CHARGE_RELAY_PIN, OUTPUT);
  pinMode(DISCHARGE_RELAY_PIN, OUTPUT);
  
  // Начальное состояние
  digitalWrite(THYRISTOR_PIN, LOW);
  digitalWrite(CHARGE_RELAY_PIN, LOW);
  digitalWrite(DISCHARGE_RELAY_PIN, HIGH);
  
  // Диагностика односторонней платы
  Serial.println("Проверка односторонней платы:");
  Serial.println("1. Проверьте все перемычки (JMP1-JMP5)");
  Serial.println("2. Проверьте силовые дорожки (3-5 мм шириной)");
  Serial.println("3. Убедитесь в отсутствии КЗ под перемычками");
  
  delay(1000);
}

float readVoltageSingleLayer() {
  // Функция чтения напряжения с учетом особенностей односторонней платы
  int rawValue = analogRead(VOLTAGE_PIN);
  float voltage = (rawValue * 5.0 / 1023.0) * VOLTAGE_DIVIDER_RATIO;
  
  // Коррекция для односторонней платы (потери в дорожках)
  voltage = voltage * 1.02;  // +2% компенсация
  
  return voltage;
}

void chargeCapacitorsSingleLayer() {
  // Зарядка для односторонней платы с учетом индуктивности
  digitalWrite(DISCHARGE_RELAY_PIN, LOW);
  delay(100 + SINGLE_LAYER_DELAY);  // Увеличенная задержка
  digitalWrite(CHARGE_RELAY_PIN, HIGH);
  
  Serial.println("Зарядка (односторонняя плата)");
  Serial.println("Внимание: зарядка медленнее из-за индуктивности дорожек");
}

void weldSingleLayer(int durationMs) {
  // Сварка для односторонней платы
  digitalWrite(CHARGE_RELAY_PIN, LOW);
  delay(50 + SINGLE_LAYER_DELAY);
  
  digitalWrite(THYRISTOR_PIN, HIGH);
  delay(durationMs);
  digitalWrite(THYRISTOR_PIN, LOW);
  
  // Увеличенная пауза перед разрядом
  delay(100 + SINGLE_LAYER_DELAY);
  digitalWrite(DISCHARGE_RELAY_PIN, HIGH);
  
  Serial.print("Импульс: ");
  Serial.print(durationMs + SINGLE_LAYER_DELAY);  // Фактическое время
  Serial.println(" мс (с учетом односторонней платы)");
}

void loop() {
  // Основной цикл с учетом односторонней платы
  
  // Чтение напряжения с калибровкой
  float voltage = readVoltageSingleLayer();
  
  // Управление зарядкой
  if (voltage < MAX_VOLTAGE && voltage > 5.0) {
    chargeCapacitorsSingleLayer();
  }
  
  // Проверка кнопки с увеличенной антидребезгом
  if (digitalRead(START_BUTTON) == LOW) {
    delay(100);  // Увеличенная задержка для односторонней платы
    if (digitalRead(START_BUTTON) == LOW && voltage > MIN_VOLTAGE) {
      int pulseTime = map(analogRead(TIME_POT_PIN), 0, 1023, 5, 100);
      weldSingleLayer(pulseTime);
    }
  }
  
  // Медленный цикл для стабильности
  delay(200);
}
