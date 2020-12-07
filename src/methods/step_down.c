/* utf-8 */

/**
 * @file
 * @brief Ступенька вниз
 * @details Когда вариант остался один на каждый фрагмент - делаем одну ступеньку вниз
 * после первого недостроенного здания.
 *
 * @date создан 18.10.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include "skyskrapers/street.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

bool
method_step_down(const street_t *street)
{
    bool changed = false;
    int clue = street_get_clue(street);

    if (clue < 2) {
        return false;
    }

    /* Количество однозначно видимых построенных зданий текущего ряда.*/
    int total_visible = street->visible;
    /* Общее количество строящихся зданий, которые могут повлиять на видимость.*/
    int total_vacant = street->vacant;
    hill_t *hills = street->hill_array;
    /* Количество фрагментов рельефа. Каждый фрагмент начинается с недостроенного
     * видимого здания и заканчиваетя недостроенным зданием, за которым следует
     * построенное здание выше максимальной высоты фрагмента.*/
    int hill_cnt = street->hill_count;


    if (!total_vacant || clue - total_visible != 1) {
        return false;
    }

    for (int hl_i = 0; hl_i < hill_cnt; hl_i++) {
        if (hills[hl_i].vacant != total_vacant) {
            continue;
        }

        tower_t *tower = street_get_tower(street, hills[hl_i].first);

        if (hills[hl_i].shadow >= tower_get_min_height(tower)) {
            continue;
        }

        int mask_and = tower_get_mask(1, tower_get_max_height(tower) - 1);
        int enable_bit = 1;
        int enable_mask = hills[hl_i].action_mask;

        for (int tw_i = hills[hl_i].first + 1; tw_i <= hills[hl_i].last; tw_i++) {
            enable_bit <<= 1;

            if ((enable_mask & enable_bit) != 0) {
                tower_t *tower = street_get_tower(street, tw_i);

                /* TODO без проверки tower_has_floors в некоторых случаях tower_and_options
                 * вызывает ошибку "tower_set_options: Assertion `options != 0' failed."
                 * Скорее всего это происходит при переборе в неправильном направлении. */
                if (!tower_is_complete(tower) &&
                        tower_has_floors(tower, mask_and) &&
                        tower_and_options(tower, mask_and)) {
                    changed = true;
                }
            }
        }
    }

    return  changed;
}
