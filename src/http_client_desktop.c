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

#if HTTP_WRAPPER_DESKTOP == 1

#include "http_client.h"
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

static unsigned long int MAX_RESPONSE_SIZE;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t total_size = size * nmemb;
    char *response = (char *)userp;

    // Calculate the remaining buffer size
    size_t current_length = strlen(response);
    size_t remaining_space = MAX_RESPONSE_SIZE - current_length - 1; // -1 for null-terminator

    // Ensure we don't exceed the buffer size
    if (total_size > remaining_space)
    {
        total_size = remaining_space;
    }

    // Append the contents to the response buffer
    strncat(response, (char *)contents, total_size);

    // Return the number of bytes actually written
    return total_size;
}

int https_get(const char *url, char *response, size_t max_response_size, unsigned int timeout_ms)
{
    CURL *curl;
    CURLcode res;

    // Initialize response buffer
    if (response == NULL || max_response_size == 0)
    {
        return -1;
    }
    response[0] = '\0';                    // Ensure null-termination
    MAX_RESPONSE_SIZE = max_response_size; // Set buffer size globally if necessary

    curl = curl_easy_init();
    if (!curl)
    {
        return -1;
    }

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    if (timeout_ms)
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);

    // Skip SSL verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Perform the request
    res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        return -1;
    }

    return 0;
}

#endif // HTTP_WRAPPER_DESKTOP