# MeteoSwiss C Library

A simple C library to interact with the MeteoSwiss API, providing weather data for Swiss postal codes. This library fetches and parses meteorological data, allowing developers to easily integrate weather information into their C applications.

## Features

- Fetch current weather data for a given Swiss postal code.
- Retrieve forecast information.
- Access detailed weather graphs and data arrays.
- Supports both static and dynamic linking.
- Minimal dependencies.

## Requirements

- **C Compiler**: GCC or compatible compiler supporting C99 standard.
- **libcurl**: The library depends on [libcurl](https://curl.se/libcurl/) for HTTP requests.
- **pkg-config**: For easy integration and linking.
- **Operating System**: Currently tested only on Linux.

## Installation

### Clone the Repository

```bash
git clone https://github.com/bourquenoud/libmeteoswiss.git
cd libmeteoswiss
```

### Build the Library

Build the library:

```bash
make
```

### Install the Library

To install the library, headers, and pkg-config file to the default locations (`/usr/local`):

```bash
sudo make install
```

To install to a custom directory:

```bash
sudo make install PREFIX=/custom/install/path
```

## Usage

### Compiling Your Application

Include the library header in your C source code:

```c
#include "meteoswiss.h"
```

Compile and link your application using `pkg-config`:

#### Linking with the Shared Library

```bash
gcc your_app.c $(pkg-config --cflags --libs meteoswisslib) -o your_app
```

#### Linking with the Static Library

```bash
gcc your_app.c $(pkg-config --cflags --libs --static meteoswisslib) -o your_app
```

### Example

```c
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
```

## Build and Run Tests

To build and run the test suite:

```bash
make
```

## License

This library is licensed under the [GNU Lesser General Public License v3.0](LICENSE).

### Legal/License Notices

- **json.h**: This library includes `json.h`, a header-only JSON parsing library by [sheredom](https://github.com/sheredom). It is licensed under the [Unlicense](https://unlicense.org/).

## Acknowledgments

- **json.h**: JSON parsing library by [sheredom](https://github.com/sheredom).

## Disclaimer

This library is currently tested only on Linux systems. Compatibility with other operating systems has not been verified.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request on GitHub.

## Contact

For any questions or suggestions, please open an issue.
