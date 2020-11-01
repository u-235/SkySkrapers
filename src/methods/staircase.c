/* utf-8 */

/**
 * @file
 * @brief Лестница - ограничение и сверху и снизу.
 * @details
 *
 * @date создан 01.11.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include "skyskrapers/street.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

bool
method_staircase(const street_t *street)
{
    bool changed = false;
    int clue = street_get_clue(street);

    if (clue < 2) {
        return false;
    }

    int first_highest = street->highest_first;
    /* Количество однозначно видимых построенных зданий текущего ряда.*/
    int visible = street->visible;
    /* Общее количество строящихся зданий, которые могут повлиять на видимость.*/
    int vacant = street->vacant;
    hill_t *hills = street->hill_array;
    /* Количество фрагментов рельефа. Каждый фрагмент начинается с недостроенного
     * видимого здания и заканчиваетя недостроенным зданием, за которым следует
     * построенное здание выше максимальной высоты фрагмента.*/
    int hill_cnt = street->hill_count;

    if (!vacant || vacant != clue - visible) {
        return false;
    }

    for (int i = 0; i < hill_cnt; i++) {
        /* Не уверен, что это нужно. */
        if (hills[i].vacant == 0) {
            continue;
        }

        int lim = hills[i].shadow + 1;
        int btm = hills[i].bottom > lim ? hills[i].bottom : lim;
        int mask_and = tower_get_mask(btm, hills[i].top + 1 - hills[i].vacant);
        int enable_bit = 1;
        int enable_mask = hills[i].action_mask;

        for (int tw_i = hills[i].first; tw_i <= hills[i].last && tw_i <= first_highest; tw_i++) {
            if ((enable_mask & enable_bit) != 0) {
                tower_t *tower = street_get_tower(street, tw_i);

                if (tower_has_floors(tower, mask_and) && tower_and_options(tower, mask_and)) {
                    changed = true;
                }

                mask_and <<= 1;
            }

            enable_bit <<= 1;
        }
    }

    return  changed;
}
