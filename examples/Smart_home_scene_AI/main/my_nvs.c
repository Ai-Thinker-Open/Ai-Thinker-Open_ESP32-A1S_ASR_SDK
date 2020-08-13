#include "system_lib.h"

#include "my_nvs.h"

#define STORAGE_NAMESPACE "storage"
//#define DEBUG

//static void * obj= pdFALSE;
nvs_handle my_nvs_handle = pdFALSE;

esp_err_t init_nvs()
{
    if (my_nvs_handle == pdFALSE)
    {
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            // NVS partition was truncated and needs to be erased
            // Retry nvs_flash_init
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);

        // Open
        err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_nvs_handle);
        if (err != ESP_OK)
            return err;

        printf("***nvs初始化完成***\r\n");
    }

    return ESP_OK;
}

/*

类型：blob

 */

//获取长度
size_t get_nvs_blob_length(const char *key)
{
    esp_err_t err;
    size_t length = 0;

    init_nvs();

    err = nvs_get_blob(my_nvs_handle, key, NULL, &length);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        return 0;

    //   #ifdef DEBUG
    printf("%s_length=%d\n", key, length);
    //   #endif

    return length;
}

//写
esp_err_t nvs_write_blob(const char *key, void *str, size_t *required_size)
{
    esp_err_t err;

    init_nvs();

    err = nvs_set_blob(my_nvs_handle, key, str, required_size);
    if (err != ESP_OK)
        return err;

    // Commit
    err = nvs_commit(my_nvs_handle);
    if (err != ESP_OK)
        return err;

    return ESP_OK;
}

//读
esp_err_t nvs_read_blob(const char *key, void *dat)
{
    esp_err_t err;
    size_t length = 0;

    init_nvs();

    //get_length
    err = nvs_get_blob(my_nvs_handle, key, NULL, &length);
    if (err != ESP_OK)
        return err;

    //read
    err = nvs_get_blob(my_nvs_handle, key, dat, &length);
    if (err != ESP_OK)
        return err;

    // Commit
    err = nvs_commit(my_nvs_handle);
    if (err != ESP_OK)
        return err;

    return ESP_OK;
}

/*

类型：String

 */

//获取长度
size_t get_nvs_string_length(const char *key)
{
    esp_err_t err;
    size_t length = 0;

    init_nvs();

    err = nvs_get_str(my_nvs_handle, key, NULL, &length);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        return 0;

    //   #ifdef DEBUG
    printf("%s_length=%d\n", key, length);
    //   #endif

    return length;
}

//写
esp_err_t nvs_write_string(const char *key, char *value)
{
    esp_err_t err;

    init_nvs();

    err = nvs_set_str(my_nvs_handle, key, value);
    if (err != ESP_OK)
        return err;

    // Commit
    err = nvs_commit(my_nvs_handle);
    if (err != ESP_OK)
        return err;

    return ESP_OK;
}

//读
esp_err_t nvs_read_string(const char *key, char *dat)
{
    esp_err_t err;
    size_t length = 0;

    init_nvs();

    //get_length
    err =nvs_get_str(my_nvs_handle, key, NULL, &length);
    if (err != ESP_OK)
        return err;

    //get_dat
    err = nvs_get_str(my_nvs_handle, key, dat, &length);
    if (err != ESP_OK)
        return err;
    
    // Commit
    err = nvs_commit(my_nvs_handle);
    if (err != ESP_OK)
        return err;

    return ESP_OK;
}


    /*测试代码段 */
    // for (size_t i = 0; i < 11; i++)
    // {
    //        char dat0[16];
    //        sprintf(dat0,"%d\n",i);
    //        if(nvs_write_string("String_demo",dat0)==ESP_OK){printf("String_demo:::ESP_OK\n");}

    //        char * dat=malloc(get_nvs_string_length("String_demo"));
           
    //        nvs_read_string("String_demo",dat);
    //        printf("%s\n",dat);
    //        free(dat);  
    // }