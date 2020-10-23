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
method_obvious(city_t *city)
{
    bool changed = false;
    int options = 1 << city->size;

    for (int h = city->size; h > 0; h--) {
        options >>= 1;

        for (int side = 0; side <= 1; side++) {
            for (int pos = 0; pos < city->size; pos++) {
                tower_t *highest = 0;

                for (int i = 0; i < city->size; i++) {
                    tower_t *tower = city_get_tower(city, side, pos, i);

                    if (tower->height == h) {
                        highest = 0;
                        break;
                    }

                    if (tower->options & options) {
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
                    city->changed = true;
                    //return true;
                }
            }
        }
    }

    return  changed;
}
