/*
 * GNU LESSER GENERAL PUBLIC LICENSE
 * Version 3, 29 June 2007
 * Copyright (C) 2024 Mathieu Bourquenoud
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "meteoswiss.h"
#include "http_client.h"
#include "validate_json.h"
#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define METEOSWISS_URL "https://app-prod-ws.meteoswiss-app.ch/v1/plzDetail?plz="
#define PLZ_FORMAT_STRING "%04d00"
#define PLZ_LENGTH 6
#define RESPONSE_BUFFER_SIZE 16384

// Prototypes
static struct json_value_s *get_object_value(struct json_object_s *object, const char *key);
static int json_value_to_int(struct json_value_s *value, int *out_int);
static int json_value_to_long_long(struct json_value_s *value, long long *out_long_long);
static int json_value_to_float(struct json_value_s *value, float *out_float);
static int json_value_to_string(struct json_value_s *value, char *buffer, size_t buffer_size);
static int parse_current_weather(struct json_object_s *json_obj, CurrentWeather *current_weather);
static int parse_forecast(struct json_array_s *json_array, ForecastEntry **forecast, size_t *count);
static int parse_graph(struct json_object_s *json_obj, WeatherGraph *graph);
static int parse_float_array(struct json_array_s *json_array, float **out_array, size_t *out_count);

// API functions
int meteoswiss_query(int postal_code, MeteoSwissData *data, unsigned int timeout)
{
    if (data == NULL)
    {
        return -1;
    }

    static char url[sizeof(METEOSWISS_URL) + PLZ_LENGTH + 1] = METEOSWISS_URL;
    snprintf(url + sizeof(METEOSWISS_URL) - 1, PLZ_LENGTH + 1, PLZ_FORMAT_STRING, postal_code);

    char response[RESPONSE_BUFFER_SIZE];
    if (https_get(url, response, sizeof(response), timeout) != 0)
    {
        return -1;
    }

    struct json_value_s *root = json_parse(response, strlen(response));
    if (root == NULL)
    {
        return -1;
    }

    struct json_object_s *root_obj = json_value_as_object(root);
    if (root_obj == NULL)
    {
        free(root);
        return -1;
    }

    if(validate_json(root) != VALIDATE_JSON_SUCCESS)
    {
        free(root);
        return -1;
    }

    memset(data, 0, sizeof(MeteoSwissData));

    // Parse currentWeather
    struct json_value_s *current_weather_val = get_object_value(root_obj, "currentWeather");
    if (current_weather_val)
    {
        struct json_object_s *current_weather_obj = json_value_as_object(current_weather_val);
        if (current_weather_obj)
        {
            if (parse_current_weather(current_weather_obj, &data->currentWeather) != 0)
            {
                free(root);
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }

    // Parse forecast
    struct json_value_s *forecast_val = get_object_value(root_obj, "forecast");
    if (forecast_val)
    {
        struct json_array_s *forecast_array = json_value_as_array(forecast_val);
        if (forecast_array)
        {
            if (parse_forecast(forecast_array, &data->forecast, &data->forecast_count) != 0)
            {
                free(root);
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }

    // Parse graph
    struct json_value_s *graph_val = get_object_value(root_obj, "graph");
    if (graph_val)
    {
        struct json_object_s *graph_obj = json_value_as_object(graph_val);
        if (graph_obj)
        {
            if (parse_graph(graph_obj, &data->graph) != 0)
            {
                free(root);
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }

    // Clean up
    free(root);
    return 0;
}

void meteoswiss_data_free(MeteoSwissData *data)
{
    if (data)
    {
        // Free forecast entries
        if (data->forecast)
        {
            free(data->forecast);
            data->forecast = NULL;
            data->forecast_count = 0;
        }
        // Free graph data arrays
        if (data->graph.precipitation10m)
        {
            free(data->graph.precipitation10m);
            data->graph.precipitation10m = NULL;
            data->graph.precipitation10m_count = 0;
        }
        // Free other arrays as needed
    }
}

// Helper function to get a value from a JSON object by key
static struct json_value_s *get_object_value(struct json_object_s *object, const char *key)
{
    struct json_object_element_s *elem = object->start;
    size_t key_len = strlen(key);
    while (elem)
    {
        if (elem->name->string_size == key_len &&
            strncmp(elem->name->string, key, key_len) == 0)
        {
            return elem->value;
        }
        elem = elem->next;
    }
    return NULL;
}

// Helper functions to convert JSON values to C types
static int json_value_to_int(struct json_value_s *value, int *out_int)
{
    struct json_number_s *num = json_value_as_number(value);
    if (num)
    {
        *out_int = atoi(num->number);
        return 0;
    }
    return -1;
}

static int json_value_to_long_long(struct json_value_s *value, long long *out_long_long)
{
    struct json_number_s *num = json_value_as_number(value);
    if (num)
    {
        *out_long_long = atoll(num->number);
        return 0;
    }
    return -1;
}

static int json_value_to_float(struct json_value_s *value, float *out_float)
{
    struct json_number_s *num = json_value_as_number(value);
    if (num)
    {
        *out_float = atof(num->number);
        return 0;
    }
    return -1;
}

static int json_value_to_string(struct json_value_s *value, char *buffer, size_t buffer_size)
{
    struct json_string_s *str = json_value_as_string(value);
    if (str)
    {
        size_t copy_size = (str->string_size < buffer_size - 1) ? str->string_size : buffer_size - 1;
        memcpy(buffer, str->string, copy_size);
        buffer[copy_size] = '\0';
        return 0;
    }
    return -1;
}

static int parse_current_weather(struct json_object_s *json_obj, CurrentWeather *current_weather)
{
    struct json_value_s *value;

    if ((value = get_object_value(json_obj, "time")) != NULL)
    {
        json_value_to_long_long(value, &current_weather->time);
    }
    if ((value = get_object_value(json_obj, "icon")) != NULL)
    {
        json_value_to_int(value, &current_weather->icon);
    }
    if ((value = get_object_value(json_obj, "iconV2")) != NULL)
    {
        json_value_to_int(value, &current_weather->iconV2);
    }
    if ((value = get_object_value(json_obj, "temperature")) != NULL)
    {
        json_value_to_float(value, &current_weather->temperature);
    }
    return 0;
}

static int parse_forecast(struct json_array_s *json_array, ForecastEntry **forecast, size_t *count)
{
    size_t idx = 0;
    *count = json_array->length;
    *forecast = calloc(*count, sizeof(ForecastEntry));
    if (*forecast == NULL)
    {
        return -1;
    }

    struct json_array_element_s *element = json_array->start;
    while (element)
    {
        struct json_object_s *forecast_obj = json_value_as_object(element->value);
        if (forecast_obj)
        {
            ForecastEntry *entry = &(*forecast)[idx];
            struct json_value_s *value;

            if ((value = get_object_value(forecast_obj, "dayDate")) != NULL)
            {
                json_value_to_string(value, entry->dayDate, sizeof(entry->dayDate));
            }
            if ((value = get_object_value(forecast_obj, "iconDay")) != NULL)
            {
                json_value_to_int(value, &entry->iconDay);
            }
            if ((value = get_object_value(forecast_obj, "iconDayV2")) != NULL)
            {
                json_value_to_int(value, &entry->iconDayV2);
            }
            if ((value = get_object_value(forecast_obj, "temperatureMax")) != NULL)
            {
                json_value_to_float(value, &entry->temperatureMax);
            }
            if ((value = get_object_value(forecast_obj, "temperatureMin")) != NULL)
            {
                json_value_to_float(value, &entry->temperatureMin);
            }
            if ((value = get_object_value(forecast_obj, "precipitation")) != NULL)
            {
                json_value_to_float(value, &entry->precipitation);
            }
            if ((value = get_object_value(forecast_obj, "precipitationMin")) != NULL)
            {
                json_value_to_float(value, &entry->precipitationMin);
            }
            if ((value = get_object_value(forecast_obj, "precipitationMax")) != NULL)
            {
                json_value_to_float(value, &entry->precipitationMax);
            }
        }
        element = element->next;
        idx++;
    }
    return 0;
}

static int parse_float_array(struct json_array_s *json_array, float **out_array, size_t *out_count)
{
    size_t count = json_array->length;
    float *array = calloc(count, sizeof(float));
    if (array == NULL)
    {
        return -1;
    }

    size_t idx = 0;
    struct json_array_element_s *element = json_array->start;
    while (element)
    {
        struct json_number_s *num = json_value_as_number(element->value);
        if (num)
        {
            array[idx] = atof(num->number);
        }
        element = element->next;
        idx++;
    }

    *out_array = array;
    *out_count = count;
    return 0;
}

static int parse_graph(struct json_object_s *json_obj, WeatherGraph *graph)
{
    struct json_value_s *value;

    if ((value = get_object_value(json_obj, "start")) != NULL)
    {
        json_value_to_long_long(value, &graph->start);
    }
    if ((value = get_object_value(json_obj, "startLowResolution")) != NULL)
    {
        json_value_to_long_long(value, &graph->startLowResolution);
    }
    if ((value = get_object_value(json_obj, "precipitation10m")) != NULL)
    {
        struct json_array_s *array = json_value_as_array(value);
        if (array)
        {
            if (parse_float_array(array, &graph->precipitation10m, &graph->precipitation10m_count) != 0)
            {
                return -1;
            }
        }
    }
    // Parse other arrays as needed
    return 0;
}
