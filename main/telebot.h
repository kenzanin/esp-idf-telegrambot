#ifndef MAIN_TELEBOT_H_
#define MAIN_TELEBOT_H_

#include "string.h"
#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_tls.h"
#include "cJSON.h"

#include "esp_http_client.h"

void
loop (void *pvParameters);

#endif /* MAIN_TELEBOT_H_ */
