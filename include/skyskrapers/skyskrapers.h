/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkyScrapers
 * @details
 *
 * @date создан 25.09.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef _SKYSKRAPERS_H
#define _SKYSKRAPERS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _city city_t;

city_t *
city_new(int size);

void
city_free(city_t *city);

void
city_load(city_t *city, int *clues);

bool
city_solve(city_t *city);

int **
city_export(city_t *city);

void
city_print(city_t *city);

#ifdef __cplusplus
}
#endif

#endif /* _SKYSKRAPERS_H */
