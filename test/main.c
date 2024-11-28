#include "meteoswiss.h"
#include <stdio.h>
#include <string.h>

// Function to validate data fields
int validate_data(const MeteoSwissData *data, int expect_failure)
{
    int valid = 1;

    // Validate current weather
    if (data->currentWeather.temperature < -50 || data->currentWeather.temperature > 50)
    {
        if (!expect_failure)
            printf("Error: Invalid current temperature: %.1f°C\n", data->currentWeather.temperature);
        valid = 0;
    }

    // Validate forecast
    if (data->forecast_count > 0)
    {
        for (size_t i = 0; i < data->forecast_count; i++)
        {
            const ForecastEntry *entry = &data->forecast[i];
            if (strlen(entry->dayDate) == 0)
            {
                if (!expect_failure)
                    printf("Error: Forecast entry %zu is missing a date\n", i);
                valid = 0;
            }
            if (entry->temperatureMax < entry->temperatureMin)
            {
                if (!expect_failure)
                    printf("Error: Forecast entry %zu has max temp %.1f°C less than min temp %.1f°C\n",
                           i, entry->temperatureMax, entry->temperatureMin);
                valid = 0;
            }
        }
    }
    else
    {
        if (!expect_failure)
            printf("Error: No forecast data found\n");
        valid = 0;
    }

    // Validate graph
    if (data->graph.precipitation10m && data->graph.precipitation10m_count == 0)
    {
        if (!expect_failure)
            printf("Error: Graph precipitation data exists but has zero elements\n");
        valid = 0;
    }

    return valid;
}

// Run a test for a single postal code
int run_test(int postal_code, int expect_failure, unsigned int timeout)
{
    int valid = 1;
    printf("Testing postal code: %04d\n", postal_code);
    printf("Expecting a %s\n", (expect_failure ? "failure" : "success"));

    MeteoSwissData data;
    memset(&data, 0, sizeof(MeteoSwissData));

    int result = meteoswiss_query(postal_code, &data, timeout); // No timeout
    if (result != 0 && !expect_failure)
    {
        printf("Unexpected query failure for postal code %d.\n", postal_code);
        valid = 0;
    }
    else if (result == 0 && expect_failure)
    {
        printf("Unexpected query success for postal code %d.\n", postal_code);
        valid = 0;
    }

    // Validate fields
    result = validate_data(&data, expect_failure);
    if (!result && !expect_failure)
    {
        printf("Unexpected validation failure for postal code %d.\n", postal_code);
        valid = 0;
    }
    else if (result && expect_failure)
    {
        printf("Unexpected validation success for postal code %d.\n", postal_code);
        valid = 0;
    }

    meteoswiss_data_free(&data);
    return valid;
}

int main()
{
    // Define test cases
    struct
    {
        int postal_code;
        int expect_failure;
        int timeout;
    } test_cases[] = {
        {1201, 0, 0}, // Geneva - Valid
        {8001, 0, 0}, // Zurich - Valid
        {1003, 0, 0}, // Lausanne - Valid
        {0000, 1, 0}, // Invalid location - Expected to fail
        {1201, 1, 1}, // Geneva - Minimal timeout, expected to fail
        {1700, 1, 1}, // Geneva - Minimal timeout, expected to fail
    };

    int total_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    int passed_tests = 0;

    printf("Running MeteoSwiss API tests...\n");
    for (int i = 0; i < total_tests; i++)
    {
        printf("\n################# Running test %d #################\n", i);
        if (run_test(test_cases[i].postal_code, test_cases[i].expect_failure, test_cases[i].timeout))
        {
            printf(">>PASSED<<\n");
            passed_tests++;
        }
        else
        {
            printf(">>FAILED<<\n");
        }
    }

    // Summary
    printf("\nTest Summary:\n");
    printf("Total tests: %d\n", total_tests);
    printf("Tests passed: %d\n", passed_tests);
    printf("Tests failed: %d\n", total_tests - passed_tests);

    if(passed_tests == total_tests)
    {
        printf("ALL TEST PASSED\n");
        return 0;
    }
    else
    {
        printf("ERROR: SOME TEST FAILED\n", total_tests - passed_tests);
        return 1;
    }
}
