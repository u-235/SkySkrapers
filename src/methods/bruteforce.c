/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkyScrapers
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
    tower_t *tower = 0;
    bool run = true;

    for (int x = 0; run && x < city->size; x++) {
        for (int y = 0; run && y < city->size; y++) {
            tower = city_get_tower(city, 0, x, y);

            run = tower_is_complete(tower);
        }
    }

    if (run) {
        return false;
    }

    /* Для возврата состояния города при неудачной попытке перебора делаем копию.
     * А для перебора только допустимых высот заводим bit_enable. Это немного ускорит поиск. */
    city_t *backup = city_copy(0, city);
    int bit_enable = 1 << (city->size - 1);

    for (int i = city->size; i > 0 ; i--) {
        if (tower_has_floors(tower, bit_enable)) {
            tower_set_height(tower, i);

            if (city_solve(city)) {
                city_free(backup);
                return true;
            }

            city_copy(city, backup);
        }

        bit_enable >>= 1;
    }

    city_free(backup);
    return false;
}
