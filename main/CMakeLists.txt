# Edit following two lines to set component requirements (see docs)
idf_component_register(SRCS "main.c"
                            "gpio.c"
                            "web_server.c"
                            "wifi.c"
                            "restart.c"
                    PRIV_REQUIRES esp_netif esp_driver_gpio esp_wifi esp_http_server nvs_flash bt
                    INCLUDE_DIRS ".")
