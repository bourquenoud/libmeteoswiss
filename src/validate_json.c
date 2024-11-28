// validate_json.c

#include "validate_json.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Helper function to get a value from a JSON object by key.
 *
 * @param object The JSON object.
 * @param key The key to search for.
 * @return The JSON value associated with the key, or NULL if not found.
 */
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

/**
 * @brief Helper function to validate an array of objects with specific required keys.
 *
 * @param array The JSON array.
 * @param keys The array of required keys.
 * @param num_keys The number of required keys.
 * @return VALIDATE_JSON_SUCCESS if all elements have all required keys.
 *         VALIDATE_JSON_ERROR_MISSING_KEY if any key is missing.
 */
static ValidateJsonStatus validate_array_of_objects(struct json_array_s *array, const char **keys, size_t num_keys)
{
    struct json_array_element_s *element = array->start;
    while (element)
    {
        struct json_object_s *object = json_value_as_object(element->value);
        if (object == NULL)
        {
            return VALIDATE_JSON_ERROR_MISSING_KEY;
        }
        for (size_t i = 0; i < num_keys; ++i)
        {
            struct json_value_s *value = get_object_value(object, keys[i]);
            if (value == NULL)
            {
                return VALIDATE_JSON_ERROR_MISSING_KEY;
            }
        }
        element = element->next;
    }
    return VALIDATE_JSON_SUCCESS;
}

/**
 * @brief Validates the JSON response against the required keys.
 *
 * @param root The root JSON value.
 * @return VALIDATE_JSON_SUCCESS if all required keys are present,
 *         VALIDATE_JSON_ERROR_MISSING_KEY otherwise.
 */
ValidateJsonStatus validate_json(struct json_value_s *root)
{
    if (root == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }

    struct json_object_s *root_obj = json_value_as_object(root);
    if (root_obj == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }

    // Check currentWeather
    struct json_value_s *currentWeather_val = get_object_value(root_obj, "currentWeather");
    if (currentWeather_val == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    struct json_object_s *currentWeather_obj = json_value_as_object(currentWeather_val);
    if (currentWeather_obj == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    // Required keys in currentWeather
    const char *currentWeather_keys[] = {"time", "icon", "iconV2", "temperature"};
    for (size_t i = 0; i < sizeof(currentWeather_keys) / sizeof(currentWeather_keys[0]); ++i)
    {
        if (get_object_value(currentWeather_obj, currentWeather_keys[i]) == NULL)
        {
            return VALIDATE_JSON_ERROR_MISSING_KEY;
        }
    }

    // Check forecast[]
    struct json_value_s *forecast_val = get_object_value(root_obj, "forecast");
    if (forecast_val == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    struct json_array_s *forecast_array = json_value_as_array(forecast_val);
    if (forecast_array == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    // Required keys in each forecast element
    const char *forecast_keys[] = {
        "dayDate",
        "iconDay",
        "iconDayV2",
        "temperatureMax",
        "temperatureMin",
        "precipitation",
        "precipitationMin",
        "precipitationMax"
    };
    if (validate_array_of_objects(forecast_array, forecast_keys, sizeof(forecast_keys)/sizeof(forecast_keys[0])) != VALIDATE_JSON_SUCCESS)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }

    // Check warnings[]
    struct json_value_s *warnings_val = get_object_value(root_obj, "warnings");
    if (warnings_val == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    struct json_array_s *warnings_array = json_value_as_array(warnings_val);
    if (warnings_array == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    // No further validation specified for warnings[]

    // Check warningsOverview[]
    struct json_value_s *warningsOverview_val = get_object_value(root_obj, "warningsOverview");
    if (warningsOverview_val == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    struct json_array_s *warningsOverview_array = json_value_as_array(warningsOverview_val);
    if (warningsOverview_array == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    // No further validation specified for warningsOverview[]

    // Check graph
    struct json_value_s *graph_val = get_object_value(root_obj, "graph");
    if (graph_val == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    struct json_object_s *graph_obj = json_value_as_object(graph_val);
    if (graph_obj == NULL)
    {
        return VALIDATE_JSON_ERROR_MISSING_KEY;
    }
    // Required keys in graph
    const char *graph_keys[] = {"start", "startLowResolution"};
    for (size_t i = 0; i < sizeof(graph_keys) / sizeof(graph_keys[0]); ++i)
    {
        if (get_object_value(graph_obj, graph_keys[i]) == NULL)
        {
            return VALIDATE_JSON_ERROR_MISSING_KEY;
        }
    }

    // Arrays under graph
    const char *graph_array_keys[] = {
        "precipitation10m",
        "precipitationMin10m",
        "precipitationMax10m",
        "weatherIcon3h",
        "weatherIcon3hV2",
        "windDirection3h",
        "windSpeed3h",
        "sunrise",
        "sunset",
        "temperatureMin1h",
        "temperatureMax1h",
        "temperatureMean1h",
        "precipitation1h",
        "precipitationMin1h",
        "precipitationMax1h",
        "windSpeed1h",
        "windSpeed1hq10",
        "windSpeed1hq90",
        "gustSpeed1h",
        "gustSpeed1hq10",
        "gustSpeed1hq90",
        "sunshine1h",
        "precipitationProbability3h"
    };

    for (size_t i = 0; i < sizeof(graph_array_keys)/sizeof(graph_array_keys[0]); ++i)
    {
        struct json_value_s *array_val = get_object_value(graph_obj, graph_array_keys[i]);
        if (array_val == NULL)
        {
            return VALIDATE_JSON_ERROR_MISSING_KEY;
        }
        struct json_array_s *array = json_value_as_array(array_val);
        if (array == NULL)
        {
            return VALIDATE_JSON_ERROR_MISSING_KEY;
        }
        // No further validation on the contents of the arrays
    }

    return VALIDATE_JSON_SUCCESS;
}
