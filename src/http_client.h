// http_client.h

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Perform an HTTPS GET request.
 *
 * @param url The URL to request.
 * @param response Buffer to store the response.
 * @param max_response_size The maximum size of the response buffer.
 * @return 0 on success, non-zero on failure.
 */
int https_get(const char *url, char *response, size_t max_response_size, unsigned int timeout);

#ifdef __cplusplus
}
#endif

#endif // HTTP_CLIENT_H
