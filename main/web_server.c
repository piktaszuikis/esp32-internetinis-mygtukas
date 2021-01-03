/*
 * HTTP serveris, rodantis HTML puslapį ir reaguojantis į mygtuko paspaudimą.
 */

#include <esp_log.h>
#include <esp_event.h>
#include <esp_http_server.h>

#include "gpio.h"
#include "puslapis.h"
#include "web_server.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
static const char *TAG = "mygtukas-web";

static int PUSLAPIS_INDEX = 0;
static char *PUSLAPIS_STATUS = "";

void query_pc_status()
{
	if(gpio_is_pc_powered())
		PUSLAPIS_STATUS = "✓ įjungtas";
	else
		PUSLAPIS_STATUS = "✗ išjungtas";
}

/* HTTP GET handleris */
static esp_err_t mygtukas_get_handler(httpd_req_t *req)
{
	query_pc_status();
	
	httpd_resp_set_type(req, "text/html; charset=UTF-8");
	
	//Siuntimas vykdomas trimis etapais:
	//1. Išsiunčiama dalis iki "Būsena: ?"
	httpd_resp_send_chunk(req, PUSLAPIS, PUSLAPIS_INDEX);
	
	//2. Išsiunčiama būsena
	httpd_resp_send_chunk(req, PUSLAPIS_STATUS, HTTPD_RESP_USE_STRLEN);
	
	//3. Išsiunčiama likusi dalis.
	httpd_resp_send_chunk(req, PUSLAPIS + PUSLAPIS_INDEX + 1, HTTPD_RESP_USE_STRLEN);
	
	//Užbaigiamas siuntimas.
	httpd_resp_send_chunk(req, NULL, 0);
	
	ESP_LOGI(TAG, "Puslapis sėkmingai parodytas!");
	
	return ESP_OK;
}

static const httpd_uri_t mygtukas_get = {
	.uri       = "/",
	.method    = HTTP_GET,
	.handler   = mygtukas_get_handler,
	.user_ctx = NULL
};

/* HTTP POST handleris */
static esp_err_t mygtukas_post_handler(httpd_req_t *req)
{
	char buf[64];
	int ret = httpd_req_recv(req, buf, MIN(req->content_len, sizeof(buf)));

	if(ret <= 0) {
		ESP_LOGI(TAG, "Klaida skaitant užklausą! %d", ret);
		return ret;
	}

	ESP_LOGI(TAG, "POST: %.*s", ret, buf);

	if(strcmp(buf, "action=power") == 0) //Jeigu buvo paspaustas mygtukas
	{
		ESP_LOGI(TAG, "Įjungimas!");
		gpio_trigger_pc_power_btn();
	}

	//Redirect'inam į GET puslapį
	httpd_resp_set_status(req, "303 See Other");
	httpd_resp_set_hdr(req, "Location", ".");
	
	return httpd_resp_send(req, NULL, 0);
}

static const httpd_uri_t mygtukas_post = {
	.uri       = "/",
	.method    = HTTP_POST,
	.handler   = mygtukas_post_handler,
	.user_ctx  = NULL
};

static void stop(httpd_handle_t server)
{
	httpd_stop(server);
}

static httpd_handle_t start(void)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	ESP_LOGI(TAG, "Paleidžiamas serveris, portas: '%d'", config.server_port);
	
	if (httpd_start(&server, &config) == ESP_OK) {
		ESP_LOGI(TAG, "Registruojami URI handleriai...");
		httpd_register_uri_handler(server, &mygtukas_get);
		httpd_register_uri_handler(server, &mygtukas_post);
		
		gpio_set_online_led(1);
		ESP_LOGI(TAG, "Serveris paleistas sėkmingai.");
		
        return server;
    }

    ESP_LOGI(TAG, "Klaida paleidžiant serverį!");
    return NULL;
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	httpd_handle_t* server = (httpd_handle_t*) arg;
	if (*server) {
		ESP_LOGI(TAG, "Išjungiu webserverį");
		stop(*server);
		*server = NULL;
	}

	gpio_set_online_led(0);
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start();
    }
}

void web_server_start()
{
	PUSLAPIS_INDEX = strchr(PUSLAPIS, '?') - PUSLAPIS;
	
	static httpd_handle_t server = NULL;
	
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

	/* Pirmas serverio paleidimas turi būti iškviestas atskirai */
	server = start();	
}

