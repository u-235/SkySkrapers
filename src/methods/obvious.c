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
    int options = 0;
    int once = 0;
    int many = 0;

    for (int i = 0; i < sz; i++) {
        tower_t *tower = street_get_tower(street, i);
        options = tower_get_options(tower);
        once ^= options & (~ many);
        many |= options & (~ once);
    }

    for (int i = 0; i < sz; i++) {
        tower_t *tower = street_get_tower(street, i);
        int hit = tower_get_options(tower) & once;

        if (hit != 0) {
            tower_set_options(tower, hit);
        }
    }

    return  changed;
}
