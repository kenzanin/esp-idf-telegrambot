#ifndef MAIN_SETUP_H_
#define MAIN_SETUP_H_

extern const char api_telegram_start[] asm("_binary_api_telegram_pem_start");
extern const char api_telegram_end[] asm("_binary_api_telegram_pem_end");

esp_err_t
_http_event_handler (esp_http_client_event_t *evt);

#endif /* MAIN_SETUP_H_ */
