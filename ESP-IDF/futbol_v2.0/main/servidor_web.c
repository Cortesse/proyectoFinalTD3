#include "servidor_web.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "motores.h"

static const char *TAG = "WEB_SERVER";

// --- Página principal ---
static esp_err_t handle_root_get(httpd_req_t *req) {
    const char *page =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>"
    "<title>Control Motores</title>"
    "<style>"
    "body{font-family:Arial;text-align:center;margin:0;padding:0;background:#fafafa;"
    "touch-action:none;-webkit-touch-callout:none;-webkit-user-select:none;"
    "-moz-user-select:none;-ms-user-select:none;user-select:none;}"
    ".container{display:flex;flex-direction:column;justify-content:center;align-items:center;height:100vh;}"
    ".button-row{display:flex;justify-content:center;align-items:center;gap:20px;flex-wrap:wrap;}"
    "button{font-size:22px;padding:25px 40px;margin:5px;border:none;border-radius:20px;"
    "background:#1976d2;color:white;box-shadow:0 4px 6px rgba(0,0,0,0.3);cursor:pointer;"
    "-webkit-user-select:none;user-select:none;-webkit-touch-callout:none;}"
    "button:active{background:#0d47a1;transform:scale(0.95);}"
    "#rotateMsg{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:#000;"
    "color:#fff;font-size:28px;justify-content:center;align-items:center;text-align:center;}"
    "@media (orientation:portrait){.container{display:none;}#rotateMsg{display:flex;}}"
    "</style>"
    "</head><body>"
    "<div id='rotateMsg'>🔄 Girá tu celular.</div>"
    "<div class='container'>"
    "<div class='button-row'>"
    "<button data-dir='ava_on'>↑</button>"
    "<button data-dir='ret_on'>↓</button>"
    "<button data-dir='izq_on'>←</button>"
    "<button data-dir='der_on'>→</button>"
    "</div>"
    "</div>"
    "<script>"
    "let pressed=false;"
    "let activeDir='';"
    "function sendCommand(cmd){fetch('/'+cmd,{method:'POST'}).catch(()=>{});}"
    "function press(dir){"
        "if(!pressed){pressed=true;activeDir=dir;sendCommand(dir);}"
    "}"
    "function release(){"
        "if(pressed){pressed=false;activeDir='';sendCommand('fre');}"
    "}"
    // --- Compatible Android, iPhone y PC ---
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
static esp_err_t handle_ava_on_post(httpd_req_t *req) {
    motores_ava();
    motor_estado = MOTOR_AVA;
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t handle_ret_on_post(httpd_req_t *req) {
    motores_ret();
    motor_estado = MOTOR_RET;
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t handle_izq_on_post(httpd_req_t *req) {
    motores_izq();
    motor_estado = MOTOR_IZQ;
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t handle_der_on_post(httpd_req_t *req) {
    motores_der();
    motor_estado = MOTOR_DER;
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t handle_fre_post(httpd_req_t *req) {
    motores_fre();
    motor_estado = MOTOR_FRE;
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// --- Servidor web ---
void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_root     = { .uri = "/",        .method = HTTP_GET,  .handler = handle_root_get };
        httpd_uri_t uri_ava_on    = { .uri = "/ava_on",   .method = HTTP_POST, .handler = handle_ava_on_post };
        httpd_uri_t uri_ret_on   = { .uri = "/ret_on",  .method = HTTP_POST, .handler = handle_ret_on_post };
        httpd_uri_t uri_izq_on  = { .uri = "/izq_on", .method = HTTP_POST, .handler = handle_izq_on_post };
        httpd_uri_t uri_der_on = { .uri = "/der_on",.method = HTTP_POST, .handler = handle_der_on_post };
        httpd_uri_t uri_fre     = { .uri = "/fre",    .method = HTTP_POST, .handler = handle_fre_post };

        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_ava_on);
        httpd_register_uri_handler(server, &uri_ret_on);
        httpd_register_uri_handler(server, &uri_izq_on);
        httpd_register_uri_handler(server, &uri_der_on);
        httpd_register_uri_handler(server, &uri_fre);

        ESP_LOGI(TAG, "Servidor web iniciado correctamente");
    }
}
