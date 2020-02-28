#include "telebot.h"

esp_err_t
_http_event_handler (esp_http_client_event_t *evt)
{
  const char *TAG = "HTTPEVENHANDLER";
  switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
      ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
	       evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response (evt->client))
	{
	  // Write out data
	  // printf("%.*s", evt->data_len, (char*)evt->data);
	  //ESP_LOGI(TAG, "%d %.*s",evt->data_len, evt->data_len, (char* )evt->data);
	}
      break;
    case HTTP_EVENT_ON_FINISH:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
      int mbedtls_err = 0;
      esp_err_t err = esp_tls_get_and_clear_last_error (
	  (esp_tls_error_handle_t) evt->data, &mbedtls_err, NULL);
      if (err != 0)
	{
	  ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
	  ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
	}
      break;
    }
  return ESP_OK;
}

void
app_main (void)
{
  const char *TAG = "APP_MAIN";
  esp_err_t ret = nvs_flash_init ();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
      ESP_ERROR_CHECK(nvs_flash_erase ());
      ret = nvs_flash_init ();
    }
  ESP_ERROR_CHECK(ret);
  ESP_ERROR_CHECK(esp_netif_init ());
  ESP_ERROR_CHECK(esp_event_loop_create_default ());

  /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
   * Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
  ESP_ERROR_CHECK(example_connect ());
  ESP_LOGI(TAG, "Connected to AP, begin http example");
  ESP_LOGI(TAG, "esp_get_free_heap_size: %d", esp_get_free_heap_size ());
  xTaskCreate (&loop, "loop", 1024 * 5, NULL, 5, NULL);
  //loop();
}
