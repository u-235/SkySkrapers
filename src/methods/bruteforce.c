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

#include "skyskrapers/skyskrapers.h"
#include "skyskrapers/city.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

bool
method_bruteforce(city_t *city)
{
    tower_t *tower;
    int x = 0, y = 0, max = 0;

    /* Вычисление оптимальной точки для перебора.
     * Сначала в каждой колонке этажи недостроенных зданий суммируются и эта сумма записывается
     * в поле weight зданий колонки.*/
    for (int iy = 0; iy < city->size; iy++) {
        int sum = 0;

        for (int ix = 0; ix < city->size; ix++) {
            tower = city_get_tower(city, 0, ix, iy);

            if (tower->height == 0) {
                sum += tower->options;
            }
        }

        for (int ix = 0; ix < city->size; ix++) {
            city_get_tower(city, 0, ix, iy)->weight = sum;
        }
    }

    max = 0;

    /* Затем находится такие же суммы для строк и эти суммы плюсуются с полем weight. Попутно
     * запоминается недостроенное здание с самым большим весом. */
    for (int ix = 0; ix < city->size; ix++) {
        int sum = 0;

        for (int iy = 0; iy < city->size; iy++) {
            tower = city_get_tower(city, 0, ix, iy);

            if (tower->height == 0) {
                sum += tower->options;
            }
        }

        for (int iy = 0; iy < city->size; iy++) {
            tower = city_get_tower(city, 0, ix, iy);
            int w = tower->weight + sum;

            if (tower->height == 0 && w > max) {
                x = ix;
                y = iy;
                max = w;
            }
        }
    }

    /* Для возврата состояния города при неудачной попытке перебора делаем копию.
     * А для перебора только допустимых высот заводим bit_enable. Это немного ускорит поиск. */
    tower = city_get_tower(city, 0, x, y);
    city_t *backup = city_copy(0, city);
    int bit_enable = 1 << (city->size - 1);

    for (int i = city->size; i > 0 ; i--) {
        if ((bit_enable & tower->options) != 0) {
            tower_set_height(tower, i);

            if (city_solve(city)) {
                city_free(backup);
                return true;
            }
        }

        bit_enable >>= 1;
        city_copy(city, backup);
    }

    city_free(backup);
    return false;
}
