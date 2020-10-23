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
method_exclude(city_t *city)
{
    bool changed = false;

    for (int side = 0; side <= 1; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int options = city->mask;

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);

                //if (tower->options == 0) {
                //    return false;
                //}

                if (tower->height != 0) {
                    options &= ~tower->options;
                }
            }

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);

                if (tower->height == 0) {
                    if (tower_and_options(tower, options)) {
                        changed = true;
                        city->changed = true;
                        //return true;
                    }
                }
            }
        }
    }

    return  changed;
}
