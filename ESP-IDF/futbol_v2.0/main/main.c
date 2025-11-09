#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "wifi_ap.h"
#include "servidor_web.h"
#include "motores.h"

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();

    motores_init();
    start_wifi_ap();
    start_webserver();
}
