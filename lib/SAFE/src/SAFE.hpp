#ifndef SAFE_h
#define SAFE_h

// Pinos de ativação dos relés - Cada pino representa uma porta
#define PORTA_1 4
#define PORTA_2 16
#define PORTA_3 17
#define PORTA_4 18
#define PORTA_5 19
#define PORTA_6 21

// Pinos dos sensores de cada porta
#define SENSOR_PORTA_1 33
#define SENSOR_PORTA_2 32
#define SENSOR_PORTA_3 35
#define SENSOR_PORTA_4 34
#define SENSOR_PORTA_5 39
#define SENSOR_PORTA_6 36

// Pinos dos leds de indicação das portas
#define LED_PORTA_1 13
#define LED_PORTA_2 14
#define LED_PORTA_3 27
#define LED_PORTA_4 26
#define LED_PORTA_5 25
#define LED_PORTA_6 22

// Estados de cada porta, false -> (0) representa aberta enquanto true -> (1) representa fechada
bool isDoorOneOpened = false;
bool isDoorTwoOpened = false;
bool isDoorThreeOpened = false;
bool isDoorFourOpened = false;
bool isDoorFiveOpened = false;
bool isDoorSixOpened = false;

#endif