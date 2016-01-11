class Sensors{
  private: 
      unsigned int PIN; // на каком пине подключен датчик
      unsigned int sensor_old_state; // предыдущее состояние датчика

  public:
    /** Конструктор
    * Pin - пин на который подключен датчик типа барьер
    */
      Sensors(unsigned int Pin)
      {
          PIN = Pin;
          pinMode(PIN, INPUT); // Режим работы порта (чтение)
      }

      bool getSensorBarier()
      {
        bool result = false;
        bool flagChange = false;
        if (digitalRead(PIN) == 0 ) // Датчик сработал
        {
            //flagChange = true;
            // устранение дребезга
            unsigned long startTimeLoop = millis();
            do 
            {
                #ifdef DEBUG
                  Serial.print("startTimeLoop=");
                  Serial.print(startTimeLoop);
                  Serial.print(", millis()=");
                  Serial.println(millis());
                #endif
              // Если после срабатывания датчика мы опять получаем с датчика сигнал 0 (нет события)
              // в течении 300 миллисекунд, то значит это ложное срабатывание и не надо его считать за нормальное срабатывание датчика
              if (digitalRead(PIN)==1) { // это было ложное срабатывание
                flagChange = false;
                #ifdef DEBUG
                  Serial.println("getSensorBarier(). Change sensor false (digitalRead(PIN)==1)");
                #endif
                break; // закончили отсчитывать нормальное время (считаем что срабатывание ложное)
              }
              flagChange = true;
            } while ((millis()-startTimeLoop) < TIME_DETECT_CONFIRM); // TIME_DETECT_CONFIRM мс. если в течении этого времени небыло "дребезга" то будем считать что это нормальное срабатывание
        }

        if (flagChange == true) // Получили реальное срабатывание
        {
            result = true;
        }
        return result;
      }
};
