//#include "led_vars.h"
//#include "Tlc5940/Tlc5940.h"
//#include "Tlc5940/tlc_fades.h"

TLC_CHANNEL_TYPE channel;
Tlc5940 LEDClass;

class lightControl {
  private: 
      flag_state led_state_flag; // Текущее состояние управляемого источника света
      bool flag_mode_standby; // флаг активности режима standby (если true-режим включен)
      unsigned int LightValue_alertMode;
      unsigned int LightValue_alertMode_min;
      unsigned int LightValue_alertMode_max;
      unsigned int alertMode_counter;
      bool directionAlertMode; // етекущее направление движения анимации (уменьшение или увеличение)

      void setLedStateOn() { led_state_flag = ON; }
      void setLedStateOff() { led_state_flag = OFF; }

  public:
      // Конструктор
      lightControl()
      {
          //LEDClass.init(LIGHT_OFF);
          //lightoff();
        setModeStandBy(false);
      }

      // Инициализация
      void init()
      {
          #ifdef DEBUG
            Serial.println("light.init()");
          #endif
          LEDClass.init(LIGHT_OFF);
          lightoff();
          LightValue_alertMode = 0;
          directionAlertMode = false;
          alertMode_counter = 0;
      }

      /**
      * Сброс флага установки начального состояния AlertMode
      */
      void resetAlertMode()
      {
          LightValue_alertMode = 0;
      }

      /**
      * Установка дежурного режима (и включение тригера состояния дежурного режима)
      * Если сейчас подсветка не включена то:
      * 1.Гасим подсветку
      * 2.Устанавливаем флаг
      ***************************************************** */ 
      void setModeStandByOn()
      {
        if (!getModeStandBy()) // Если режим standby еще не включен, то будем включать
        {
          lightModeStandByOn();
          setModeStandBy(true);
        }
      }
      
      /**
      * Выключение режима standby
      * Если сейчас подсветка включена то:
      * 1.Гасим подсветку
      * 2.Устанавливаем флаг
      ***************************************************** */ 
      void setModeStandByOff()
      {
        if (getModeStandBy()) // Если режим standby еще не включен, то будем включать
        {
          lightoff();
          setModeStandBy(false);
        }
      }

      /**
      * Изменение флага активности режима standby
      ***************************************************** */ 
      void setModeStandBy(bool value = false)
      {
          flag_mode_standby = value;
      }

      /**
      *
      ***************************************************** */ 
      bool getModeStandBy()
      {
        return flag_mode_standby;
      }

      /**
      *
      ***************************************************** */
      flag_state getLedState()
      { 
          #ifdef DEBUG
              //Serial.print("getLedState=");
              //Serial.println(led_state_flag);
          #endif
          return led_state_flag;
      }

      /**
      * Выключение освещения. Всего сразу
      ***************************************************** */ 
      void lightoff()
      {
        #ifdef DEBUG
          Serial.println("light.lightoff()");
        #endif

        LEDClass.setAll(LIGHT_OFF);
        LEDClass.update();
        setLedStateOff();
      }

      /**
      * Включение освещения. Всего сразу (с заданным уровнем яркости)
      ***************************************************** */ 
      void lighton()
      {
        unsigned int ValueMax = LIGHT_OFF - LIGHT_MAX/BRIGHT; // максимальная яркость свечения
        LEDClass.setAll(ValueMax);
        LEDClass.update();
        resetAlertMode();
        setLedStateOn();
      }

      /**
      * Включение дежурной подсветки лестницы.
      * Включаются первые ступеньки (тускло)
      ***************************************************** */ 
      void lightModeStandBy(int LightValueBegin, int Delay)
      {
        #ifdef DEBUG
          Serial.println("light.lightModeStandBy");
        #endif
        for (long ValueCurrent = LIGHT_OFF;  ValueCurrent > LightValueBegin; ValueCurrent -=1)
        {
            //Serial.print(" ValueCurrent="); Serial.println(ValueCurrent);
            LEDClass.set(STAIR_FIRST, ValueCurrent);
            LEDClass.set(STAIR_LAST, ValueCurrent);
            LEDClass.update();
            delay(Delay);
         }
         setLedStateOff();
      }

      /**
      * Включение дежурной подсветки лестницы (стандартный режим 1)
      *
      ***************************************************** */
      void lightModeStandByOn()
      {
        lightModeStandBy(LIGHT_VALUE_STANDBY, 1); // временно поставил делитель светимости (в 5 раз меньше)
      }


      /**
      * Плавное выключение освещения. Всего сразу
      * Выключение производится всех линеек, кроме крайних (режим стендбай)
      ***************************************************** */ 
      void lightSoftOff(int Delay)
      {
        #ifdef DEBUG
          Serial.println("light.lightSoftOff");
        #endif
        unsigned int LightValueBegin = LEDClass.get(STAIR_FIRST); // начинать отсчет от текущей светимости
        for (long ValueCurrent = LightValueBegin;  ValueCurrent <= LIGHT_OFF; ValueCurrent +=1)
        {
            LEDClass.setAll(ValueCurrent);
            if (ValueCurrent > LIGHT_VALUE_STANDBY) { // Если это крайние ленты
              LEDClass.set(STAIR_FIRST, LIGHT_VALUE_STANDBY);
              LEDClass.set(STAIR_LAST, LIGHT_VALUE_STANDBY);
            }
            LEDClass.update();
            delay(Delay);

         }
         setLedStateOff();
      }

      /**
      ***************************************************** */ 
      void lightOnWave(int Direction = 0)
      {
        int Light;
        unsigned int ValueMax = LIGHT_MAX/BRIGHT; // максимальная яркость свечения
        unsigned int steps = 100; // количество шагов в анимации на 1 (до начала зажигания следующей)
        unsigned int channels = STAIR_LAST-STAIR_FIRST;
        float k = 5; // Наклон в анимации
        unsigned int delayTime = 5; // Задержка времени между шагами анимации
        int t = 0;
        bool b = false;
        int channelSet;
        while (!b)
        {
          b = true;
          for (int c=0; c<=channels; c++)
          {
            float y = k * t - c * steps;
            Light =  round(y);
            if (Light < 0) { Light = 0; }
            if (Light >= ValueMax) { Light = ValueMax; }
            else { b = false; }
            // Направление движения
            if (Direction == 1) { channelSet = STAIR_LAST - c; }
            else { channelSet = STAIR_FIRST + c; }

            int ValueCurrent = LIGHT_OFF - Light;
            if (ValueCurrent > LIGHT_VALUE_STANDBY) { // Если уровень яркости уменьшился до уровня стендбай или ниже
              if (channelSet == STAIR_FIRST) { ValueCurrent = LIGHT_VALUE_STANDBY; } // Если это крайние ленты (первая)
              if (channelSet == STAIR_LAST) { ValueCurrent = LIGHT_VALUE_STANDBY; } // Если это крайние ленты (последняя)
            }
            LEDClass.set(channelSet, ValueCurrent);
          }
          delay(delayTime);
          LEDClass.update();
          t++;
        }
        setLedStateOn();
        #ifdef DEBUG
          Serial.println("Fin animate");
        #endif
      }

      /**
      ***************************************************** */ 
      void lightOnDown()
      {
        #ifdef DEBUG
          Serial.println("light.lightOnDown");
        #endif
        lightOnWave(1);
      }

      /**
      ***************************************************** */ 
      void lightOnUp()
      {
        #ifdef DEBUG
          Serial.println("light.lightOnUp");
        #endif
        lightOnWave(0);
      }

      /**
      * Включение предупреждающей подсветки лестницы.
      * Процедура не зацикливаемая (к ней надо обращаться периодически, каждое новое обращение изменяет состояние на 1 шаг)
      * Текущее состояние будет храниться в процедуре
      ***************************************************** */ 
      void lightModeAlert1Step(int Delay)
      {
          if (LightValue_alertMode == 0) // Инициализация (приходим сюда 1 раз)
          {
              directionAlertMode = false;
              LightValue_alertMode_min = LEDClass.get(STAIR_FIRST) + DELTA_LIGHT_VALUE_ALERT_MODE; // конечное значение
              if (LightValue_alertMode_min>LIGHT_MAX) {LightValue_alertMode_min=LIGHT_MAX;}
              LightValue_alertMode_max = LEDClass.get(STAIR_FIRST); // Начальное значение (с которого надо светиться)
              LightValue_alertMode = LightValue_alertMode_max; 
          }

          // Прошуск шагов (срабатывание только через заданное ALERT_MODE_EMPTY_STEPS количество шагов)
          alertMode_counter++;
          if (alertMode_counter >= ALERT_MODE_EMPTY_STEPS)
          {
              alertMode_counter = 0;
              if (LightValue_alertMode >= LightValue_alertMode_min) { directionAlertMode = true;
              } else if (LightValue_alertMode <= LightValue_alertMode_max) { directionAlertMode = false; }

              if (directionAlertMode) {
                  LightValue_alertMode=LightValue_alertMode-2;
              } else {
                  LightValue_alertMode=LightValue_alertMode+2;
              }
              #ifdef DEBUG
                //Serial.println(LightValue_alertMode);
              #endif
              if (LightValue_alertMode>LIGHT_MAX) {LightValue_alertMode=LIGHT_MAX;}
              LEDClass.setAll(LightValue_alertMode);
              LEDClass.update();
          }
      }
};
