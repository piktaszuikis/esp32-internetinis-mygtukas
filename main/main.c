#include <esp_event.h>
#include <nvs_flash.h>

#include "web_server.h"
#include "wifi.h"

void app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	wifi_connect();
	web_server_start();
}
