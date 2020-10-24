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

#include "skyskrapers/city.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

bool
method_first_of_two(city_t *city)
{
    bool changed = false;

    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int clue = city_get_clue(city, side, pos);
            tower_t *tower = city_get_tower(city, side, pos, 0);

            if (clue != 2 || tower->height != 0) {
                continue;
            }

            int top = 1 << (city->size - 1);
            int limit = tower_get_max_height(tower);
            int mask = city->mask >> (city->size + 1 - limit);

            for (int i = 1; i < city->size; i++) {
                tower = city_get_tower(city, side, pos, i);

                if (tower->height > limit) {
                    break;
                }

                if (tower->height != 0) {
                    continue;
                }

                if ((tower->options & top) != 0) {
                    if (tower_has_floors(tower, top | mask) && tower_and_options(tower, top | mask)) {
                        changed = true;
                        city->changed = true;
                    }

                    break;
                }

                if (tower_has_floors(tower, mask) && tower_and_options(tower, mask)) {
                    changed = true;
                    city->changed = true;
                }
            }
        }
    }

    return  changed;
}
