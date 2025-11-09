#ifndef MOTORES_H
#define MOTORES_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// --- Pines Motor A ---
#define IN1 GPIO_NUM_33
#define IN2 GPIO_NUM_25
#define ENA GPIO_NUM_32

// --- Pines Motor B ---
#define IN3 GPIO_NUM_26
#define IN4 GPIO_NUM_27
#define ENB GPIO_NUM_14

// --- PWM ---
#define PWM_CHANNEL_A LEDC_CHANNEL_0
#define PWM_CHANNEL_B LEDC_CHANNEL_1
#define PWM_TIMER LEDC_TIMER_0
#define PWM_DUTY 150  // Ciclo de trabajo del PWM (0-255)
#define PWM_FREQ 1000 // Frecuencia del PWM en Hz

// Comandos que se enviarán por la cola (desde servidor web a tarea de motores)
typedef enum {
    MOTOR_CMD_FRE = 0,  // Detener
    MOTOR_CMD_AVA,      // Avanzar
    MOTOR_CMD_RET,      // Retroceder
    MOTOR_CMD_IZQ,      // Izquierda
    MOTOR_CMD_DER       // Derecha
} motor_command_t;

// Estado interno (solo lo usa la tarea de motores)
typedef enum {
    MOTOR_FRE,
    MOTOR_AVA,
    MOTOR_RET,
    MOTOR_IZQ,
    MOTOR_DER
} motor_estado_t;

extern motor_estado_t motor_estado;

// Cola para comunicar comandos hacia la tarea de motores
extern QueueHandle_t cola_motores;

// Prototipos de funciones de control de motores
void motores_init(void);
void motores_fre(void);
void motores_ava(void);
void motores_ret(void);
void motores_izq(void);
void motores_der(void);

// Tarea para la lógica de motores
void tarea_motores(void *pvParameters);

#endif
