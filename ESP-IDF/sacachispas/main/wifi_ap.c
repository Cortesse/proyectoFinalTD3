#include <string.h>
#include "wifi_ap.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

static const char *TAG = "WIFI_AP";

void start_wifi_ap(void) {
    // Crear interfaz de red para Access Point
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Configurar parámetros del Access Point
    wifi_config_t ap_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASS,
            .max_connection = WIFI_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .channel = 1,
        },
    };

    // Si no hay contraseña, usar autenticación abierta
    if (strlen(WIFI_PASS) == 0) ap_config.ap.authmode = WIFI_AUTH_OPEN;

    // Iniciar WiFi en modo Access Point
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Punto de acceso iniciado");
    ESP_LOGI(TAG, "SSID: %s PASS: %s", WIFI_SSID, WIFI_PASS);
    ESP_LOGI(TAG, "Abrí tu navegador en http://192.168.4.1/");
}