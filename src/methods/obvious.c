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
method_obvious(const street_t *street)
{
    bool changed = false;
    int sz = street->size;
    int options = 1 << sz;

    for (int h = sz; h > 0; h--) {
        options >>= 1;

        tower_t *highest = 0;

        for (int i = 0; i < sz; i++) {
            tower_t *tower = street_get_tower(street, i);

            if (tower_get_height(tower) == h) {
                highest = 0;
                break;
            }

            if (tower_has_floors(tower, options)) {
                if (highest == 0) {
                    highest = tower;
                } else {
                    highest = 0;
                    break;
                }
            }
        }

        if (highest != 0) {
            tower_set_height(highest, h);
            changed = true;
        }
    }

    return  changed;
}
