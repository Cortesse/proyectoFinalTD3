#include "motores.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char *TAG = "MOTORES";

// Variable global para estado actual de los motores
motor_estado_t motor_estado = MOTOR_FRE;

// Cola global para recibir comandos desde el servidor web
QueueHandle_t cola_motores = NULL;

// Función de inicialización de hardware de motores
void motores_init(void) {
    // Configurar pines de dirección como salidas
    gpio_reset_pin(IN1); gpio_reset_pin(IN2);
    gpio_reset_pin(IN3); gpio_reset_pin(IN4);
    gpio_set_direction(IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN3, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN4, GPIO_MODE_OUTPUT);

    // Configurar timer PWM para control de velocidad
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = PWM_TIMER,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configurar canal PWM para motor A
    ledc_channel_config_t ledc_channel_a = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL_A,
        .timer_sel = PWM_TIMER,
        .gpio_num = ENA,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel_a);

    // Configurar canal PWM para motor B
    ledc_channel_config_t ledc_channel_b = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL_B,
        .timer_sel = PWM_TIMER,
        .gpio_num = ENB,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel_b);

    // Inicializar motores en estado libre (detenidos)
    motores_fre();
}

// Función para detener ambos motores
void motores_fre(void) {
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
    motor_estado = MOTOR_FRE;
}

// Función para avanzar (ambos motores hacia adelante)
void motores_ava(void) {
    gpio_set_level(IN1, 1); gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 1); gpio_set_level(IN4, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
    motor_estado = MOTOR_AVA;
}

// Función para retroceder (ambos motores hacia atrás)
void motores_ret(void) {
    gpio_set_level(IN1, 0); gpio_set_level(IN2, 1);
    gpio_set_level(IN3, 0); gpio_set_level(IN4, 1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
    motor_estado = MOTOR_RET;
}

// Función para girar a la izquierda
void motores_izq(void) {
    gpio_set_level(IN1, 0); gpio_set_level(IN2, 1);
    gpio_set_level(IN3, 1); gpio_set_level(IN4, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
    motor_estado = MOTOR_IZQ;
}

// Función para girar a la derecha
void motores_der(void) {
    gpio_set_level(IN1, 1); gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0); gpio_set_level(IN4, 1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
    motor_estado = MOTOR_DER;
}

// ------------------------------------------------------------
// Tarea de motores: inicializa hardware y procesa la cola
// ------------------------------------------------------------
void tarea_motores(void *pvParameters) {
    motores_init();
    ESP_LOGI(TAG, "Tarea motores iniciada en core %d", xPortGetCoreID());

    motor_command_t cmd = MOTOR_CMD_FRE;

    // Bucle infinito de la tarea - espera comandos de la cola
    for (;;) {
        if (xQueueReceive(cola_motores, &cmd, portMAX_DELAY) == pdTRUE) {
            switch (cmd) {
                case MOTOR_CMD_AVA: 
                    motores_ava(); 
                    ESP_LOGI(TAG, "Motor: AVANZAR");
                    break;
                case MOTOR_CMD_RET: 
                    motores_ret(); 
                    ESP_LOGI(TAG, "Motor: RETROCEDER");
                    break;
                case MOTOR_CMD_IZQ: 
                    motores_izq(); 
                    ESP_LOGI(TAG, "Motor: IZQUIERDA");
                    break;
                case MOTOR_CMD_DER: 
                    motores_der(); 
                    ESP_LOGI(TAG, "Motor: DERECHA");
                    break;
                default: 
                    motores_fre(); 
                    ESP_LOGI(TAG, "Motor: DETENER");
                    break;
            }
        }
    }
}