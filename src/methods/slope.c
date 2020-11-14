/* utf-8 */

/**
 * @file
 * @brief Склон - ограничение только сверху.
 * @details
 *
 * @date создан 14.11.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include "skyskrapers/street.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

bool
method_slope(const street_t *street)
{
    bool changed = false;
    hill_t *hills = street->hill_array;
    int clue = street_get_clue(street) - street->visible;

    for (int i = 0; i < street->hill_count; i++) {
        if (hills[i].vacant != 0) {
            clue--;
        }
    }

    if (clue < 1) {
        return false;
    }

    for (int i = 0; i < street->hill_count; i++) {
        int steps = street->vacant - hills[i].vacant;
        steps = steps < clue ? clue - steps : 0;

        if (hills[i].vacant == 0 || steps == 0) {
            continue;
        }

        int mask_and = tower_get_mask(hills[i].bottom, hills[i].top - 1);
        int enable_mask = hills[i].action_mask;
        int enable_bit = 1 << (hills[i].last - hills[i].first);
        int vacant = hills[i].vacant;

        for (int tw_i = hills[i].last; tw_i >= hills[i].first; tw_i--) {
            if ((enable_mask & enable_bit) != 0 && vacant-- <= steps) {
                tower_t *tower = street_get_tower(street, tw_i);

                if (tower_has_floors(tower, mask_and) && tower_and_options(tower, mask_and)) {
                    changed = true;
                }

                mask_and >>= 1;
            }

            enable_bit >>= 1;
        }
    }

    return  changed;
}
