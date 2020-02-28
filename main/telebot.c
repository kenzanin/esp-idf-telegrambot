#include "telebot.h"
#include "setup.h"
#include "jsmn.h"

#define MAX_HTTP_RECV_BUFFER 512

extern const char api_telegram_start[] asm("_binary_api_telegram_pem_start");

static
int
getUpdates (int _update_id, int *_chat_id, int *_message_id, char **_username,
	    char **_text)
{
  const char *TAG = "GETUPDATES";
  char *_url = malloc (128);
  bzero (_url, strlen (_url));
  sprintf (
      _url,
      "https://api.telegram.org/bot832695492:AAGfBuLDaqISfxDl7drb6XNduWu54Ks2Xjg/getUpdates?offset=%d&limit=1",
      _update_id);
  esp_http_client_config_t config =
    { .url = "https://api.telegram.org", .transport_type =
	HTTP_TRANSPORT_OVER_SSL, .event_handler = _http_event_handler,
	.cert_pem = api_telegram_start, };

  esp_http_client_handle_t client = esp_http_client_init (&config);
  esp_http_client_set_url (client, _url);
  free (_url);
  esp_err_t err;
  if ((err = esp_http_client_open (client, 0)) != ESP_OK)
    {
      ESP_LOGE(TAG, "Failed to open HTTP connection: %s",
	       esp_err_to_name (err));
      return 0;
    }
  int content_length = esp_http_client_fetch_headers (client);
  int total_read_len = 0, read_len;
  char *buffer = malloc (content_length + 1);
  bzero (buffer, content_length);

  if (total_read_len < content_length)
    {
      read_len = esp_http_client_read (client, buffer, content_length);
      if (read_len <= 0)
	{
	  ESP_LOGE(TAG, "Error read data");
	  return 0;
	}
      buffer[read_len] = 0;
      ESP_LOGD(TAG, "read_len = %d", read_len);
    }
  ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %d",
	   esp_http_client_get_status_code (client),
	   esp_http_client_get_content_length (client));
  //esp_http_client_close (client);

  cJSON *parser = cJSON_Parse (buffer);
  free (buffer);
  cJSON *result = cJSON_GetObjectItem (parser, "result");
  cJSON *resultArray = cJSON_GetArrayItem (result, 0);
  if (resultArray == NULL)
    {
      *_text = 0;
      *_username = 0;
      cJSON_Delete (parser);
      esp_http_client_cleanup (client);
      return 0;
    }
  cJSON *updateID = cJSON_GetObjectItem (resultArray, "update_id");
  int update = updateID->valueint;
  cJSON *message = cJSON_GetObjectItem (resultArray, "message");
  cJSON *text = cJSON_GetObjectItem (message, "text");
  cJSON *message_id = cJSON_GetObjectItem (message, "message_id");
  *_message_id = message_id->valueint;
  cJSON *chat = cJSON_GetObjectItem (message, "chat");
  cJSON *chat_id = cJSON_GetObjectItem (chat, "id");
  *_chat_id = chat_id->valueint;
  cJSON *username = cJSON_GetObjectItem (chat, "username");

  *_text = malloc (strlen (text->valuestring) + 1);
  *_username = malloc (strlen (username->valuestring) + 1);
  sprintf (*_text, "%s", text->valuestring);
  sprintf (*_username, "%s", username->valuestring);

  cJSON_Delete (parser);
  esp_http_client_cleanup (client);
  ESP_LOGI(TAG, "esp_get_free_heap_size: %d", esp_get_free_heap_size ());
  return update;
}

static
void
sendMessage (int _chat_id, int _message_id, char *_text)
{
  const char *TAG = "SENDMESSAGE";
  char *_url = malloc (1024 * 2);
  sprintf (
      _url,
      "https://api.telegram.org/bot832695492:AAGfBuLDaqISfxDl7drb6XNduWu54Ks2Xjg/sendMessage?chat_id=%d&parse_mode=MarkdownV2&text=%s&reply_to_message_id=%d",
      _chat_id, _text, _message_id);
  esp_http_client_config_t config =
    { .url = "https://api.telegram.org", .transport_type =
	HTTP_TRANSPORT_OVER_SSL, .event_handler = _http_event_handler,
	.cert_pem = api_telegram_start, };

  esp_http_client_handle_t client = esp_http_client_init (&config);
  ESP_LOGI(TAG, "yang dikirim: %s", _url);
  esp_http_client_set_url (client, _url);
  free (_url);
  esp_err_t err;
  err = esp_http_client_perform (client);
  if (err == ESP_OK)
    {
      ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
	       esp_http_client_get_status_code (client),
	       esp_http_client_get_content_length (client));
    }
  else
    {
      ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name (err));
    }
  ESP_LOGI(TAG, "esp_get_free_heap_size: %d", esp_get_free_heap_size ());
  esp_http_client_cleanup (client);
}

void
loop (void *pvParameters)
{
  const char *TAG = "LOOP";
  char *username = 0;
  char *text = 0;
  int update_id = 0;
  int chat_id = 0;
  int message_id = 0;
  while (1)
    {
      update_id = getUpdates (update_id, &chat_id, &message_id, &username,
			      &text);
      if (update_id > 0)
	{
	  ESP_LOGI(TAG, "update_id: %d", update_id);
	  ESP_LOGI(TAG, "message_id: %d", message_id);
	  ESP_LOGI(TAG, "chat id: %d", chat_id);
	  ESP_LOGI(TAG, "username: %s", username);
	  ESP_LOGI(TAG, "text: %s", text);
	  sendMessage (chat_id, message_id, text);
	  update_id++;
	}
      free (username);
      free (text);
      ESP_LOGI(TAG, "esp_get_free_heap_size: %d", esp_get_free_heap_size ());
      vTaskDelay (1000 / portTICK_PERIOD_MS);
    }
}
