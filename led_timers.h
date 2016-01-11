class timer {
  private: 
      unsigned long   timer_counter; // Значение счетчик в милисекундах
      unsigned long   timer_counter_stop; // Значение uptime до которого необходимо вести отсчет
      bool            trigger;
      bool            triggerAlert; // Тригер события достижения времени оповещения


    /**
    * Изменение состояния тригера 
    */
    void setTrigger(bool value)
    { 
      trigger = value;
    }

    void setTriggerOn()
    {
        #ifdef DEBUG
          Serial.println("setTriggerOn()");
        #endif
      setTrigger(true);
    }

    void setTriggerOff()
    {
        #ifdef DEBUG
          Serial.println("setTriggerOff()");
        #endif
      setTrigger(false);
    }
          
    void setTriggerAlert(bool value)
    {
      /*
        #ifdef DEBUG
          Serial.print("setTriggerAlert(");
          Serial.print(value);
          Serial.println(")");
        #endif
      */
      triggerAlert = value;
    }

    void setTriggerAlertOn() 
    { 
        #ifdef DEBUG
          Serial.println("setTriggerAlertOn()");
        #endif
      setTriggerAlert(true); 
    }

    void setTriggerAlertOff() 
    { 
        #ifdef DEBUG
          Serial.println("setTriggerAlertOff()");
        #endif
      setTriggerAlert(false); 
    }  

  public:
    // Конструктор
    timer(unsigned int value = 0)
    {
      timer_counter = value;
      timer_counter_stop = timer_counter;
      trigger = false;
      setTriggerAlertOff();
    }

    void setTimer(unsigned int value)
    {
        #ifdef DEBUG
          Serial.print("setTimer(");
          Serial.print(value);
          Serial.println(")");
        #endif
      timer_counter = value;
      timer_counter_stop = millis() + value;
      setTriggerOn();
      setTriggerAlertOff();
    }
    
    /**
    * Получение времени таймера 
    * так-же уменьшение времени таймера на время прошедшее с предыдущего обращения (расчитывается на основании mills())
    * return 0 - если таймер достиг состояния таймаута (далее отсчет вестить не будет)
    * return unsigned int - если таймер еще не достиг состояния таймаута (ведется обратный отсчет)
    */
    unsigned long getTimer()
    {
        #ifdef DEBUG
          Serial.println("getTimer()");
        #endif
      long dT;
      // Изменим значение таймера (уменьшим)
      if (timer_counter != 0) { // Будем уменьшать только, если не достигнут таймаут
          dT = timer_counter_stop - millis();
          if (dT>0) {
            timer_counter = dT;
          } else {
            timer_counter = 0;
            setTriggerOff();
            setTriggerAlertOff();
          }
          // Если до конца свечения осталось менее чем LIGHTON_TIMEOUT_ALARM, то поднимем тригер (включить алярм-режим подсветки)
          //if (timer_counter < LIGHTON_TIMEOUT_ALARM && !getTriggerAlert())  {
          if (timer_counter < LIGHTON_TIMEOUT_ALARM)  {
            #ifdef DEBUG
              Serial.println(" 0000000                000000000 timer_counter < LIGHTON_TIMEOUT_ALARM && !getTriggerAlert()");
            #endif
            setTriggerAlertOn();
          }
        }
      // Вернем текущее значение таймера
      return timer_counter;
    }

    /**
    * Получение состояния таймаута
    * return true - время вышло (таймаут сработал)
    * return false - время еще не вышло (таймаут не сработал)
    */
    bool checkTimeout()
    {
        #ifdef DEBUG
          Serial.println("checkTimeout()");
        #endif
      bool res = false;
       if (getTimer()==0) {
          res = true; // Время вышло
       }
      return res; // Время еще не вышло
    }

    /**
    * Получение состояния триггера
    */
    bool getTrigger()
    { 
        #ifdef DEBUG
          Serial.println("getTrigger()");
        #endif
        return trigger;
    }

    /**
    * Получение состояния триггера достижения времени оповещения
    */
    bool getTriggerAlert()
    { 
        #ifdef DEBUG
          Serial.println("getTriggerAlert()");
        #endif
        return triggerAlert;
    }

};
