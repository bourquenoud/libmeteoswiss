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

#include <meteoswiss/meteoswiss.h>
#include <stdio.h>

int main() {
    int postal_code = 1201; // Geneva
    MeteoSwissData data;

    if (meteoswiss_query(postal_code, &data, 0) == 0) {
        printf("Current Temperature: %.1f°C\n", data.currentWeather.temperature);
        meteoswiss_data_free(&data);
        return 0;
    } else {
        printf("Failed to retrieve weather data.\n");
        return 1;
    }
}