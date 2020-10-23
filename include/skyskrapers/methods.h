/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkySkrapers
 * @details
 *
 * @date создан 18.10.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef _METHODS_H
#define _METHODS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool
method_obvious(city_t *city);

extern bool
method_exclude(city_t *city);

/**
 * Ограничивает высоту недостроенных зданий в ряду с подсказкой "2". Высота этих зданий не может
 * быть выше чем максимальная возможная высота первого здания минус один этаж.
 *
 * @param city Проверяемый город
 *
 * @return true если были изменения в @p city.
 */
extern bool
method_first_of_two(city_t *city);

extern bool
method_slope(city_t *city);

extern bool
method_bruteforce(city_t *city);

#ifdef __cplusplus
}
#endif

#endif /* _METHODS_H */
