#include "servidor_web.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "motores.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static const char *TAG = "WEB_SERVER";

// --- Página principal ---
static esp_err_t handle_root_get(httpd_req_t *req) {
    // Página HTML con interfaz de control táctil
    const char *page =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>"
    "<title>Control Motores</title>"
    "<style>"
    "body{margin:0;padding:0;touch-action:none;-webkit-touch-callout:none;-webkit-user-select:none;"
    "-moz-user-select:none;-ms-user-select:none;user-select:none;overflow:hidden;background:#000;}"
    ".container{display:flex;flex-direction:row;width:100vw;height:100vh;}"
    "button{flex:1;font-size:10vw;border:none;cursor:pointer;color:white;"
    "display:flex;justify-content:center;align-items:center;height:100vh;transition:transform 0.1s ease;}"
    "button:active{transform:scale(0.95);}"
    "#btn_ava{background:#e53935;}"   /* rojo */
    "#btn_ret{background:#43a047;}"   /* verde */
    "#btn_izq{background:#1e88e5;}"   /* azul */
    "#btn_der{background:#fdd835;color:black;}" /* amarillo */
    "#rotateMsg{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:#000;"
    "color:#fff;font-size:28px;justify-content:center;align-items:center;text-align:center;}"
    "@media (orientation:portrait){.container{display:none;}#rotateMsg{display:flex;}}"
    "</style>"
    "</head><body>"
    "<div id='rotateMsg'>🔄 Girá tu celular.</div>"
    "<div class='container'>"
    "<button id='btn_ava' data-dir='ava_on'>↑</button>"
    "<button id='btn_ret' data-dir='ret_on'>↓</button>"
    "<button id='btn_izq' data-dir='izq_on'>←</button>"
    "<button id='btn_der' data-dir='der_on'>→</button>"
    "</div>"
    "<script>"
    "let pressed=false;let activeDir='';"
    "function sendCommand(cmd){fetch('/'+cmd,{method:'POST'}).catch(()=>{});}" // Enviar comando al servidor
    "function press(dir){if(!pressed){pressed=true;activeDir=dir;sendCommand(dir);}}" // Presionar botón
    "function release(){if(pressed){pressed=false;activeDir='';sendCommand('fre');}}" // Soltar botón
    "function setupControls(){"
        "const buttons=document.querySelectorAll('button');"
        "buttons.forEach(btn=>{"
            "btn.addEventListener('pointerdown',e=>{e.preventDefault();press(btn.dataset.dir);},{passive:false});"
            "btn.addEventListener('pointerup',e=>{e.preventDefault();release();},{passive:false});"
            "btn.addEventListener('pointercancel',release);"
            "btn.addEventListener('pointerout',release);"
        "});"
        "document.addEventListener('pointerup',release);"
        "document.addEventListener('pointercancel',release);"
    "}"
    "window.addEventListener('load',setupControls);"
    "</script>"
    "</body></html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// --- Handlers HTTP ---
// Función genérica para manejar comandos de motor
static esp_err_t handle_cmd(httpd_req_t *req, motor_command_t cmd) {
    if (cola_motores) {
        xQueueSend(cola_motores, &cmd, 0); // Enviar comando a la cola de motores
    }
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// Handlers específicos para cada dirección
static esp_err_t handle_ava(httpd_req_t *req) { return handle_cmd(req, MOTOR_CMD_AVA); }
static esp_err_t handle_ret(httpd_req_t *req) { return handle_cmd(req, MOTOR_CMD_RET); }
static esp_err_t handle_izq(httpd_req_t *req) { return handle_cmd(req, MOTOR_CMD_IZQ); }
static esp_err_t handle_der(httpd_req_t *req) { return handle_cmd(req, MOTOR_CMD_DER); }
static esp_err_t handle_fre(httpd_req_t *req) { return handle_cmd(req, MOTOR_CMD_FRE); }

// --- Servidor web ---
void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        // Registrar handlers de URI
        httpd_uri_t uris[] = {
            {"/", HTTP_GET, handle_root_get, NULL},           // Página principal
            {"/ava_on", HTTP_POST, handle_ava, NULL},         // Avanzar
            {"/ret_on", HTTP_POST, handle_ret, NULL},         // Retroceder
            {"/izq_on", HTTP_POST, handle_izq, NULL},         // Izquierda
            {"/der_on", HTTP_POST, handle_der, NULL},         // Derecha
            {"/fre", HTTP_POST, handle_fre, NULL}             // Detener
        };

        // Registrar todos los handlers
        for (int i = 0; i < sizeof(uris)/sizeof(uris[0]); i++) {
            httpd_register_uri_handler(server, &uris[i]);
        }

        ESP_LOGI(TAG, "Servidor web iniciado en core %d", xPortGetCoreID());
    }
}

// Función wrapper para la tarea del servidor web
void tarea_start_webserver(void *pvParameters) {
    start_webserver();
    vTaskDelete(NULL); // Eliminar tarea después de iniciar servidor
}