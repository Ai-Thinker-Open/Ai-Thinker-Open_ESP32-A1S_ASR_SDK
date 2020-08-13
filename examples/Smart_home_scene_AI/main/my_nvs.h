#ifndef __MY_NVS__
#define __MY_NVS__

esp_err_t init_nvs();

size_t get_nvs_blob_length(const char *key);
esp_err_t nvs_write_blob(const char *key, void *str, size_t *required_size);
esp_err_t nvs_read_blob(const char *key, void *dat);

size_t get_nvs_string_length(const char *key);
esp_err_t nvs_write_string(const char *key, char * value);
esp_err_t nvs_read_string(const char *key,char *dat);

#endif