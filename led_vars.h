
//#define DEBUG

const unsigned int LIGHT_OFF = 4095; // состояние выключено (OFF)
const unsigned int LIGHT_ON = 0; // состояние включено (ON)
const unsigned int LIGHT_MAX = 4095; // максимальное значение яркости (для контроллера ШИМ)
//const unsigned int LIGHT_MAX = 16; // максимальное значение ярковсти (для контроллера ШИМ)
const unsigned int ST = 4;
const unsigned int STMAX = 16;
const unsigned int CHANNEL_FIRST = 0;
const unsigned int CHANNEL_LAST = 16;

const unsigned int SMESHENIE = 1000;
const unsigned int BRIGHT = 10; // Делитель яркости свечения 0...4095
const unsigned int STAIR_FIRST = 0; // Канал с первой ступенькой
const unsigned int STAIR_LAST = 16; // Канал с последней ступенькой
const unsigned int BARIER_PIN_1 = 4; // Пин для датчика 1
const unsigned int BARIER_PIN_2 = 5; // Пин для датчика 2

const unsigned int LIGHT_VALUE_STANDBY = int(LIGHT_MAX-(LIGHT_MAX/BRIGHT/10)); // уровень подсветки в режиме "standby" 

int LDR=0; // Пин датчика овещенности (A0)
const unsigned int light_sensitivity = 1000; // Значение датчика освещенности (чем ниже знаение, тем светлее), с которого надо включать подсветку лестницы (min)
const unsigned int light_sensitivity_hysteresis = 300; // Гистерезис (Значение датчика освещенности) 300
const unsigned long LIGHTON_TIMEOUT = 30 * 1000; // Длительность состояния подсветка лестницы

// Детектор прохождения людей (срабатывание датчиков типа барьер)
const unsigned int TIME_DETECT_CONFIRM = 50; // Время (в мс), если в течении которого не было изменения статуса датчика (с активного на пассивный), то срабатывание считается подтвержденным

// ALARM MODE
const unsigned long LIGHTON_TIMEOUT_ALARM = 7*1000; // время перед выключением подсветки, после которого начинает выдаваться сигнал, что свет скоро будет выключен
const unsigned int DELTA_LIGHT_VALUE_ALERT_MODE = 200; // разница яркости свечения во время loop эффекта (предупреждающего)
const unsigned int ALERT_MODE_EMPTY_STEPS = 170; // пропуск количества шагов

enum flag_state { // Текущее состояние управляемого источника света
    OFF = 0,
    ON = 1
};
