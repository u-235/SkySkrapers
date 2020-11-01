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

#include "skyskrapers/street.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

bool
method_first_of_two(const street_t *street)
{
    bool changed = false;
    int sz = street->size;

    int clue = street_get_clue(street);
    tower_t *tower = street_get_tower(street, 0);

    if (clue != 2 || tower_is_complete(tower)) {
        return false;
    }

    int top = tower_get_mask(sz, sz);
    int limit = tower_get_max_height(tower);
    int mask = tower_get_mask(1, limit - 1);

    for (int i = 1; i < sz; i++) {
        tower = street_get_tower(street, i);

        if (tower_get_height(tower) > limit) {
            break;
        }

        if (tower_is_complete(tower)) {
            continue;
        }

        if (tower_has_floors(tower, top)) {
            if (tower_has_floors(tower, top | mask) && tower_and_options(tower, top | mask)) {
                changed = true;
            }

            break;
        }

        if (tower_has_floors(tower, mask) && tower_and_options(tower, mask)) {
            changed = true;
        }
    }

    return  changed;
}
