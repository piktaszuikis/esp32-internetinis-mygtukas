/*
 * GPIO valdymas - PC įjungimas, indikacinio LED'o įjungimas/išjungimas, PC LED'o nuskaitymas.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_system.h>
#include <esp_log.h>
#include "gpio.h"

static const char* TAG = "mygtukas-gpio";
static int is_configed = 0;

void config()
{
	if(is_configed)
		return;

	ESP_LOGI(TAG, "Konfigūruojami GPIO...");

	//Įjungimo GPIO
	if( !GPIO_IS_VALID_OUTPUT_GPIO(CONFIG_MYGTUKAS_GPIO_POWER) )
	{
		ESP_LOGE(TAG, "Bloga konfigūracija: netinkama CONFIG_MYGTUKAS_GPIO_POWER reikšmė '%d'. Nėra tokio išvesties GPIO!", CONFIG_MYGTUKAS_GPIO_POWER);
		abort();
	}

	//Stebėjimo GPIO
	if( !GPIO_IS_VALID_GPIO(CONFIG_MYGTUKAS_GPIO_POWER_STATE) )
	{
		ESP_LOGE(TAG, "Bloga konfigūracija: netinkama CONFIG_MYGTUKAS_GPIO_POWER_STATE reikšmė '%d'. Nėra tokio GPIO!", CONFIG_MYGTUKAS_GPIO_POWER_STATE);
		abort();
	}

	//Indikacinio led'o GPIO
	if( !GPIO_IS_VALID_GPIO(CONFIG_MYGTUKAS_GPIO_ONLINE) )
	{
		ESP_LOGE(TAG, "Bloga konfigūracija: netinkama CONFIG_MYGTUKAS_GPIO_ONLINE reikšmė '%d'. Nėra tokio GPIO!", CONFIG_MYGTUKAS_GPIO_ONLINE);
		abort();
	}

	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = (1ULL << CONFIG_MYGTUKAS_GPIO_POWER) | (1ULL << CONFIG_MYGTUKAS_GPIO_ONLINE);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	ESP_ERROR_CHECK(gpio_config(&io_conf) != ESP_OK); //Sukonfigūruoja iš kart įjungimo ir indikatoriaus GPIO

	io_conf.pin_bit_mask = (1ULL << CONFIG_MYGTUKAS_GPIO_POWER_STATE);
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	ESP_ERROR_CHECK(gpio_config(&io_conf)); //Sukonfigūruoja kompiuterio būklės stebėjimo GPIO

	is_configed = 1; //Antrą kartą nebekonfigūruokim
}

//"Paspausti" kompiuterio įjungimo mygtuką
void gpio_trigger_pc_power_btn()
{
	config();
	
	ESP_ERROR_CHECK(gpio_set_level(CONFIG_MYGTUKAS_GPIO_POWER, 1));
	vTaskDelay(700 / portTICK_RATE_MS); //Galima dėti ir trumpesnį, bet noriu, kad matytųsi kaip led'as šviečia
	
	ESP_ERROR_CHECK(gpio_set_level(CONFIG_MYGTUKAS_GPIO_POWER, 0));
	vTaskDelay(1000 / portTICK_RATE_MS); //Duodam šansą užsidegti kompiuterio "POWER LED"'ui.
}

//Indikacinio LED'o valdymas
void gpio_set_online_led(int is_online)
{
	config();
	ESP_ERROR_CHECK(gpio_set_level(CONFIG_MYGTUKAS_GPIO_ONLINE, is_online));
}

//Ar dega kompo "POWER LED"'as.
int gpio_is_pc_powered()
{
	config();
	return !gpio_get_level(CONFIG_MYGTUKAS_GPIO_POWER_STATE);
}

void reset_gpio_state()
{
	gpio_set_level(CONFIG_MYGTUKAS_GPIO_ONLINE, 0);
	gpio_set_level(CONFIG_MYGTUKAS_GPIO_POWER, 0);
}

void init_gpio()
{
	config();
	reset_gpio_state();
	esp_register_shutdown_handler(&reset_gpio_state);
}
