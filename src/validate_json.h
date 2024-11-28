// validate_json.h

#ifndef VALIDATE_JSON_H
#define VALIDATE_JSON_H

#include "json.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enum for validation status codes.
 */
typedef enum {
    VALIDATE_JSON_SUCCESS = 0,
    VALIDATE_JSON_ERROR_MISSING_KEY = -1
} ValidateJsonStatus;

/**
 * @brief Validates the content of the JSON response.
 *
 * Checks for the presence of all required keys in the JSON response.
 *
 * @param root The root JSON value.
 * @return VALIDATE_JSON_SUCCESS if all required keys are present.
 *         VALIDATE_JSON_ERROR_MISSING_KEY if any key is missing.
 */
ValidateJsonStatus validate_json(struct json_value_s *root);

#ifdef __cplusplus
}
#endif

#endif // VALIDATE_JSON_H
