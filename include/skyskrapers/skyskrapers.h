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

extern city_t *
city_new(int size);

extern void
city_free(city_t *city);

extern void
city_load_clues(city_t *city, const int *clues);

extern bool
city_solve(city_t *city);

extern int **
city_get_heights(const city_t *city);

extern void
city_set_heights(city_t *city, const int **heights);

extern void
city_print(const city_t *city);

#ifdef __cplusplus
}
#endif

#endif /* _SKYSKRAPERS_H */
