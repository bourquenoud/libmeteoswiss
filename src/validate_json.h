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
