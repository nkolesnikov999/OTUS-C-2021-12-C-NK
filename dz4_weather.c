#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <json-c/json.h>

#define CHUNK_SIZE 2048

// Структура для получения Curl данных
typedef struct {
    unsigned char *buffer;
    size_t len;
    size_t buflen;
} get_request;

// Функция передачи данных из Curl
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    get_request *req = (get_request *) userdata;

    //printf("receive chunk of %zu bytes\n", realsize);

    // Увеличение размера буфера для полученных данных
    while (req->buflen < req->len + realsize + 1)
    {
        req->buffer = realloc(req->buffer, req->buflen + CHUNK_SIZE);
        req->buflen += CHUNK_SIZE;
    }
    memcpy(&req->buffer[req->len], ptr, realsize);
    req->len += realsize;
    req->buffer[req->len] = 0;

    return realsize;
}

// Функция обработки Curl запроса
size_t curl_request(char *str_url, get_request *req) {

    // Если будет результат, вернем размер
    size_t res_size = 0;
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if(curl) {
        // Предустановка
        curl_easy_setopt(curl, CURLOPT_URL, str_url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)req);

        res = curl_easy_perform(curl);

        // Проверка на ошибку
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            //printf("Result = %u\n",res);
            //printf("Total received bytes: %zu\n", req->len);
            res_size = req->len;
            //printf("Buflen bytes: %zu\n", req->buflen);
        }

        // Очистка
        curl_easy_cleanup(curl);
    }

    return res_size;
}



int main(int argc, char* argv[]) {

    // Получаем название города
    char *str_query;
    if (argc > 1) {
        str_query = argv[1];
        //printf("argv1 - %s\n", str_query);
    } else {
        //При отсутствии выходим с сообщением
        printf("Please, enter a city name  - %s Moscow\n", argv[0]);
        return 0;
    }
    // Формирует URL для поиска id города
    char str_url[256] = "https://www.metaweather.com/api/location/search/?query=";
    if (strlen(str_query) < 100) {
        strcat(str_url, str_query);
    }
    //printf("URL - %s\n", str_url);

    // Создаем буфер для запроса id
    get_request main_req = {.buffer = NULL, .len = 0, .buflen = 0};
    main_req.buffer = malloc(CHUNK_SIZE);
    main_req.buflen = CHUNK_SIZE;

    // Запрос id города в случае 0 ответа выходим
    if ( curl_request(str_url, &main_req) == 0 ) {
        printf("No answer for city id request.\n");
        return 0;
    }
    // printf("Received data: %s\n", main_req.buffer);

    // Структуры для парсинга  city id
    struct json_object *parsed_json;
    struct json_object *city;
    struct json_object *title;
    struct json_object *woeid;
    size_t n_cities;

    // Парсинг запроса для city id
    parsed_json = json_tokener_parse((const char *)main_req.buffer);

    n_cities = json_object_array_length(parsed_json);

    int city_id = 0;

    // Если в ответе 1 город, находим его id
    if (n_cities == 1) {
        city = json_object_array_get_idx(parsed_json, 0);
        json_object_object_get_ex(city, "title", &title);
        printf("Title: %s\n", json_object_get_string(title));
        json_object_object_get_ex(city, "woeid", &woeid);
        city_id = json_object_get_int(woeid);
        //printf("Woeid: %d\n", city_id);
        free(main_req.buffer);
    } else {
        // Иначе выходим с сообщением
        printf("Found %lu cities with name %s. Check the name of city. \n", n_cities, str_query);
        free(main_req.buffer);
        return 0;
    }

    // Формирует URL для вывода погоды
    char url_weather[256] = "";
    sprintf(url_weather, "https://www.metaweather.com/api/location/%d", city_id);
    //printf("URL - %s\n", url_weather);

    // Создаем буфер для запроса погоды
    get_request weather_req = {.buffer = NULL, .len = 0, .buflen = 0};
    weather_req.buffer = malloc(CHUNK_SIZE);
    weather_req.buflen = CHUNK_SIZE;

    // Запрос погоды, в случае 0 ответа выходим
    if ( curl_request(url_weather, &weather_req) == 0 ) {
        printf("No answer for weather request.\n");
        return 0;
    }
    // printf("Received data: %s\n", weather_req.buffer);

    // Структуры для парсинга  погоды
    struct json_object *weather_json;
    struct json_object *consolidated_weather;
    struct json_object *day_weather;
    struct json_object *weather_state_name;
    struct json_object *wind_direction_compass;
    struct json_object *wind_speed;
    struct json_object *min_temp;
    struct json_object *max_temp;
    struct json_object *the_temp;

    // Парсинг погоды и вывод
    weather_json = json_tokener_parse((const char *)weather_req.buffer);
    json_object_object_get_ex(weather_json, "consolidated_weather", &consolidated_weather);
    day_weather = json_object_array_get_idx(consolidated_weather, 0);
    json_object_object_get_ex(day_weather, "weather_state_name", &weather_state_name);
    printf("Weather State: %s\n", json_object_get_string(weather_state_name));
    json_object_object_get_ex(day_weather, "wind_direction_compass", &wind_direction_compass);
    printf("Wind Direction: %s\n", json_object_get_string(wind_direction_compass));
    json_object_object_get_ex(day_weather, "wind_speed", &wind_speed);
    printf("Wind Speed: %f\n", json_object_get_double(wind_speed));
    json_object_object_get_ex(day_weather, "min_temp", &min_temp);
    printf("Min Temp: %f\n", json_object_get_double(min_temp));
    json_object_object_get_ex(day_weather, "max_temp", &max_temp);
    printf("Max Temp: %f\n", json_object_get_double(max_temp));
    json_object_object_get_ex(day_weather, "the_temp", &the_temp);
    printf("The Temp: %f\n", json_object_get_double(the_temp));

    // Очистка памяти для буфера запроса
    free(weather_req.buffer);

    return 0;
}
