#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "wifi_ap.h"
#include "servidor_web.h"
#include "motores.h"

#define COLA_MOTORES_LENGTH 5

void app_main(void) {
    // Inicializar NVS (Non-Volatile Storage) para configuración WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Inicializar stack de red y sistema de eventos
    esp_netif_init();
    esp_event_loop_create_default();
    
    // Crear cola para comunicación entre tareas (servidor web y motores)
    cola_motores = xQueueCreate(COLA_MOTORES_LENGTH, sizeof(motor_command_t));
    if (cola_motores == NULL) {
        ESP_LOGE("MAIN", "Error al crear la cola de motores");
        return;
    }

    // Iniciar punto de acceso WiFi
    start_wifi_ap();

    // Tarea del servidor web en core 0 - maneja las peticiones HTTP
    xTaskCreatePinnedToCore(
        tarea_start_webserver,
        "TareaWeb",
        4096,
        NULL,
        1,
        NULL,
        0
    );

    // Tarea de motores en core 1 - controla los motores mediante la cola
    xTaskCreatePinnedToCore(
        tarea_motores,
        "TareaMotores",
        4096,
        NULL,
        2,
        NULL,
        1
    );
}
