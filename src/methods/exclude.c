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

#include "skyskrapers/street.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

bool
method_exclude(const street_t *street)
{
    bool changed = false;
    int sz = street->size;

    if (street->side > 1) {
        return false;
    }

    int options = tower_get_mask(1, sz);

    for (int i = 0; i < sz; i++) {
        tower_t *tower = street_get_tower(street, i);

        if (tower_get_height(tower) != 0) {
            options &= ~tower_get_options(tower);
        }
    }

    for (int i = 0; i < sz; i++) {
        tower_t *tower = street_get_tower(street, i);

        if (tower_has_floors(tower, options) && tower_and_options(tower, options)) {
            changed = true;
        }
    }

    return  changed;
}
