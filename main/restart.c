#include "restart.h"
#include "gpio.h"
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_system.h>

static const char* TAG = "mygtukas-autorestart";

void timer_callback()
{
	ESP_LOGI(TAG, "Reguliarus persikrovimas! Nuo paskutinio įjungimo praėjo %lld sekundžių.", esp_timer_get_time() / (int64_t)1e6);
	esp_restart();
}

void init_autoreboot()
{
	const esp_timer_create_args_t config = {
		.callback = &timer_callback,
		.name = "reboot_timer"
	};
	esp_timer_handle_t timer;

	ESP_ERROR_CHECK(esp_timer_create(&config, &timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 60 * 60e6 /*60 * 60e6 */)); //kas 1 valandą
}
