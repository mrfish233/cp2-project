#pragma once

#if defined(__APPLE__) && defined(__MACH__)
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

static inline void *reallocarray(void *ptr, size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) {
        return realloc(ptr, 0); // realloc with 0 size is implementation-defined
    }

    // Check for overflow in the multiplication
    if (nmemb > SIZE_MAX / size) {
        errno = ENOMEM;
        return NULL;
    }

    return realloc(ptr, nmemb * size);
}

#endif
