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

#ifndef METEOSWISS_API_H
#define METEOSWISS_API_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Represents the current weather data.
 */
typedef struct {
    long long time;
    int icon;
    int iconV2;
    float temperature;
} CurrentWeather;

/**
 * @brief Represents a single forecast entry.
 */
typedef struct {
    char dayDate[11]; // YYYY-MM-DD format
    int iconDay;
    int iconDayV2;
    float temperatureMax;
    float temperatureMin;
    float precipitation;
    float precipitationMin;
    float precipitationMax;
} ForecastEntry;

/**
 * @brief Represents the weather graph data.
 */
typedef struct {
    long long start;
    long long startLowResolution;
    // Arrays to store graph data
    float *precipitation10m;
    size_t precipitation10m_count;
    // Add other arrays as needed
} WeatherGraph;

/**
 * @brief Represents the full weather data response.
 */
typedef struct {
    CurrentWeather currentWeather;
    ForecastEntry *forecast;
    size_t forecast_count;
    WeatherGraph graph;
    // Add other fields if needed
} MeteoSwissData;

/**
 * @brief Fetches and parses weather data for a given postal code.
 *
 * @param postal_code The postal code to query (e.g., 1201 for Geneva).
 * @param data Pointer to a MeteoSwissData structure to store the result.
 * @return 0 on success, non-zero on failure.
 */
int meteoswiss_query(int postal_code, MeteoSwissData *data, unsigned int timeout_ms);

/**
 * @brief Frees allocated memory in MeteoSwissData.
 *
 * @param data Pointer to the MeteoSwissData structure to clean up.
 */
void meteoswiss_data_free(MeteoSwissData *data);

#ifdef __cplusplus
}
#endif

#endif // METEOSWISS_API_H
