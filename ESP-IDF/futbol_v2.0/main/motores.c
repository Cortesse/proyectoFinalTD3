#include "motores.h"

motor_estado_t motor_estado = MOTOR_FRE;

void motores_init(void) {
    gpio_reset_pin(IN1); gpio_reset_pin(IN2);
    gpio_reset_pin(IN3); gpio_reset_pin(IN4);
    gpio_set_direction(IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN3, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN4, GPIO_MODE_OUTPUT);

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = PWM_TIMER,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_a = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL_A,
        .timer_sel = PWM_TIMER,
        .gpio_num = ENA,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel_a);

    ledc_channel_config_t ledc_channel_b = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL_B,
        .timer_sel = PWM_TIMER,
        .gpio_num = ENB,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel_b);

    motores_fre();
}

void motores_fre(void) {
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
}

void motores_ava(void) {
    gpio_set_level(IN1, 1); gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 1); gpio_set_level(IN4, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
}

void motores_ret(void) {
    gpio_set_level(IN1, 0); gpio_set_level(IN2, 1);
    gpio_set_level(IN3, 0); gpio_set_level(IN4, 1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
}

void motores_izq(void) {
    gpio_set_level(IN1, 0); gpio_set_level(IN2, 1);
    gpio_set_level(IN3, 1); gpio_set_level(IN4, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
}

void motores_der(void) {
    gpio_set_level(IN1, 1); gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0); gpio_set_level(IN4, 1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_A);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B, PWM_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_B);
}
