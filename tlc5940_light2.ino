#include "led_vars.h"
#include "Tlc5940.h"
#include "tlc_fades.h"
#include "led_timers.h"
#include "led_control.h"
#include "led_sensors.h"

//TLC_CHANNEL_TYPE channel;

unsigned int LDRValue = 0;

// Создание и инициализация таймеров
timer timeout_counter(0); // таймаут выключения подсветки
timer timeB1_counter(0); // отсчет времени от события B1

lightControl light; // Объект управления светом

// Инициализация датчиков
  Sensors sensorBarierUP(BARIER_PIN_1); // создание и инициализация верхнего датчика
  Sensors sensorBarierDown(BARIER_PIN_2); // создание и инициализация нижнего датчика
  
void setup()
{
  
    Serial.begin(115200);
  #ifdef DEBUG
    Serial.println("Start: setup()");
  #endif

  light.init();
}

void loop()
{
  LDRValue = analogRead(LDR); // Получим текущее значение освещенности
  #ifdef DEBUG
    Serial.print("LDR=");
    Serial.println(LDRValue);
    delay(100); // Ничего не делаем 0,1 секунды
  #endif
  // Определим надо ли включать подсветку сейчас (с гистерезисом)
  if ((!light.getModeStandBy()) && (LDRValue > light_sensitivity)) { // Верхнее значение гистерезиса
      light.setModeStandByOn(); // Темно. Надо включать дежурную подсветку
  } else if ((light.getModeStandBy()) && (LDRValue < (light_sensitivity - light_sensitivity_hysteresis))) { // Нижнее значение гистерезиса
      light.setModeStandByOff(); // Светло. Надо выключить подсветку
  }


// Рабочий режим. Нужна подсветка лестницы
  if (light.getModeStandBy()) {
      // Будем опрашивать датчики и реагировать на них

      // Датчик 1 (верхний)
      if(sensorBarierUP.getSensorBarier()){ // Сработал (изменил свое состояние) верхний крайний дайтик (BARIER_PIN_1)
              if (!timeout_counter.getTrigger()) { // Если ранее не был запущен таймер (триггер не поднят)
                    timeout_counter.setTimer(LIGHTON_TIMEOUT); // Устанавливаем таймер на LIGHTON_TIMEOUT (30 сек)
                    light.lightOnDown(); // Включим анимацию (подсветку). Бегущее включение вниз
              } else if (light.getLedState() == ON) // Если было срабатываение датчика во время свечения, то перевыставить таймер
              {
                    timeout_counter.setTimer(LIGHTON_TIMEOUT); // Переустанавливаем таймер на LIGHTON_TIMEOUT (30 сек)
                    light.lighton();
              }
      } else if (light.getLedState() == ON) {  // Не было срабатывания датчика, но свет ранее был включен
              if (timeout_counter.checkTimeout()) { // Проверим не пора ли выключать свет (не сработал ли таймаут)
                  light.lightSoftOff(5);
              } else { 
                  if (timeout_counter.getTriggerAlert()) { light.lightModeAlert1Step(0); } // Если до срабатывания таймаута осталось менее 10 секунд, то надо показать, что свет скоро выключиться
              }
      } else {  // Не было срабатывания датчика и свет ранее не включался

      }

      // Датчик 2 (нижний)
      if(sensorBarierDown.getSensorBarier()){ // Сработал (изменил свое состояние) верхний крайний дайтик (BARIER_PIN_2)
              if (!timeout_counter.getTrigger()) { // Если ранее не был запущен таймер (триггер не поднят)
                    timeout_counter.setTimer(LIGHTON_TIMEOUT); // Устанавливаем таймер на LIGHTON_TIMEOUT (30 сек)
                    light.lightOnUp(); // Включим анимацию (подсветку). Бегущее включение вниз
              } else if (light.getLedState() == ON) // Если было срабатываение датчика во время свечения, то перевыставить таймер
              {
                    timeout_counter.setTimer(LIGHTON_TIMEOUT); // Переустанавливаем таймер на LIGHTON_TIMEOUT (30 сек)
                    light.lighton();
              }
      } else if (light.getLedState() == ON) {  // Не было срабатывания датчика, но свет ранее был включен
              if (timeout_counter.checkTimeout()) { // Проверим не пора ли выключать свет (не сработал ли таймаут)
                  light.lightSoftOff(5);
              } else {
                  if (timeout_counter.getTriggerAlert()) { light.lightModeAlert1Step(0); } // Если до срабатывания таймаута осталось менее 10 секунд, то надо показать, что свет скоро выключиться
              }
      } else {  // Не было срабатывания датчика и свет ранее не включался

      }

  }


  // Проверить датчик освещенности
  // Если очень темно, включить подсветку яркостью 5% первых ступенек
  // Если не очень темно, включить подсветку 30% первых ступенек
  
  // Проверить датчик входа на лесенку
  // Если сработал датчик нижний, включаем анимацию снизу вверх
     // Если потом сработал верхний датчик, выключаем подсветку (уход с лесенки)
  // Если сработал датчик верхний, включаем анимацию сверху вниз
    // Если потом сработал нижний датчик, выключаем подсветку (уход с лесенки)

// Мысли
// После выключения подсветки оставить небольшое свечение %3 еще на 10 сек (чтобы можно было дойти)
/*
  lightoff();
  lightOnUp(); // Бегущее включение вверх
  lightSoftOff(int(LIGHT_MAX-LIGHT_MAX/BRIGHT), 5);
  lightOnDown(); // Бегущее включение вниз
  lightSoftOff(int(LIGHT_MAX-LIGHT_MAX/BRIGHT), 5);
  lightModeStandBy(int(LIGHT_MAX-LIGHT_MAX/BRIGHT), 5);
  */

}

