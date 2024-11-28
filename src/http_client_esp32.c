#if HTTP_WRAPPER_ESP32==1

#include "http_client.h"
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"

#define TAG "HTTP_CLIENT"

static char *resp_buffer = NULL;
static size_t resp_buffer_len = 0;

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        if (resp_buffer && evt->data_len < resp_buffer_len) {
            memcpy(resp_buffer, evt->data, evt->data_len);
            resp_buffer[evt->data_len] = '\0';
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

int https_get(const char *url, char *response, size_t max_response_size, unsigned int timeout_ms)
{
    if (response == NULL || max_response_size == 0) {
        return -1;
    }

    resp_buffer = response;
    resp_buffer_len = max_response_size;

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        // For simplicity, skip SSL certificate verification (not recommended for production)
        .cert_pem = NULL,
        .skip_cert_common_name_check = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return -1;
    }
    
    esp_err_t err = esp_http_client_set_timeout_ms(client, timeout_ms);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set HTTP timeout: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return -1;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return -1;
    }

    esp_http_client_cleanup(client);
    return 0;
}

#endif //HTTP_WRAPPER_ESP32==1