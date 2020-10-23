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

#include <stdlib.h>
#include <string.h>

#include "skyskrapers/city.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/methods.h"

typedef struct _hill {
    int first;
    int last;
    int vacant;
    int mask;
    int shadow;
    int top;
    int bottom;
} hill_t;

bool
method_slope(city_t *city)
{
    bool changed = false;

    hill_t *hills = malloc(city->size * sizeof(hill_t));

    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int clue = city_get_clue(city, side, pos);

            if (clue < 2) {
                continue;
            }

            /* Получение индекса последнего доступного здания с максимальной высотой. */
            int last_highest = city->size - 1;

            for (int i = 0 ; i < city->size; i++) {
                tower_t *t = city_get_tower(city, side, pos, i);

                if (t->options & (city->mask ^ (city->mask >> 1))) {
                    last_highest = i;
                }

                if (t->height == city->size) {
                    break;
                }
            }

            /* Получение индекса первого здания с максимальной высотой. */
            int first_highest = 0;

            for (int i = 0 ; i < city->size; i++) {
                tower_t *t = city_get_tower(city, side, pos, i);

                if (t->options & (city->mask ^ (city->mask >> 1))) {
                    first_highest = i;
                    break;
                }
            }

            /* Количество однозначно видимых построенных зданий текущего ряда.*/
            int total_visible = 0;
            /* Общее количество строящихся зданий, которые могут повлиять на видимость.*/
            int total_vacant = 0;
            /* Нижняя граница влияния на видимость зданий. Что бы быть видимым, здание должно
             * быть выше этой границы и выше тени от предыдущего фрагмента (см hill_t::shadow).
             * С каждым новым видимым зданием граница инкрементируется а если нижний этаж
             * этого здания выше границы, то приравнивается нижнему этажу. Для нового фрагмента
             * равняется нулю. */
            int bottom_limit = 0;
            /* Количество фрагментов рельефа. Каждый фрагмент начинается с недостроенного
             * видимого здания и заканчиваетя недостроенным зданием, за которым следует
             * построенное здание выше максимальной высоты фрагмента.*/
            int hill_cnt = 0;
            /* Количество зданий фрагмента рельефа, включая незаметные. */
            int hill_size = 0;
            int bit = 1;
            memset(hills, 0, city->size * sizeof(hill_t));

            /* Сбор статистики идёт до последнего возможно самого высокого здания. */
            for (int i = 0; i  <= last_highest; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);
                int height = tower->height;

                if (height == city->size) {
                    total_visible++;
                    break;
                } else if (height > hills[hill_cnt].top) {
                    if (hill_size != 0) {
                        hills[hill_cnt].last = hills[hill_cnt].first + hill_size - 1;
                        hill_size = 0;
                        hill_cnt++;
                    }

                    total_visible++;
                    bottom_limit = height;
                    hills[hill_cnt].top = height;
                    bit = 1;
                    continue;
                } else if (height != 0 && hill_size == 0) {
                    continue;
                }

                if (hill_size++ == 0) {
                    hills[hill_cnt].first = i;
                    hills[hill_cnt].shadow = bottom_limit;
                    bottom_limit = 0;
                }

                int bottom = tower_get_min_height(tower);
                int top = tower_get_max_height(tower);

                if (top > hills[hill_cnt].shadow && top > bottom_limit) {
                    bottom_limit++;
                    bottom_limit = bottom_limit > bottom ? bottom_limit : bottom;
                    bottom = hills[hill_cnt].bottom == 0 ? bottom :
                             hills[hill_cnt].bottom < bottom ? hills[hill_cnt].bottom : bottom;
                    hills[hill_cnt].bottom = bottom;

                    hills[hill_cnt].top = hills[hill_cnt].top > top ? hills[hill_cnt].top : top;
                    hills[hill_cnt].vacant++;
                    total_vacant++;
                    hills[hill_cnt].mask |= bit;
                }

                bit <<= 1;
            }

            if (hill_size) {
                hills[hill_cnt].last = hills[hill_cnt].first + hill_size - 1;
                hill_cnt++;
            }

            /* Статистика собрана. */

            if (!total_vacant) {
                continue;
            }

            if (total_vacant == clue - total_visible) {
                /* Лестница (staircase) - ограничение и сверху и снизу. */
                for (int hl_i = 0; hl_i < hill_cnt; hl_i++) {
                    /* Не уверен, что это нужно. */
                    if (hills[hl_i].vacant == 0) {
                        continue;
                    }

                    int lim = hills[hl_i].shadow + 1;
                    int btm = hills[hl_i].bottom > lim ? hills[hl_i].bottom : lim;
                    int mask_and = tower_get_mask(btm, hills[hl_i].top + 1 - hills[hl_i].vacant);
                    int enable_bit = 1;
                    int enable_mask = hills[hl_i].mask;

                    for (int tw_i = hills[hl_i].first; tw_i <= hills[hl_i].last && tw_i <= first_highest; tw_i++) {
                        if ((enable_mask & enable_bit) != 0) {
                            tower_t *tower = city_get_tower(city, side, pos, tw_i);

                            if (tower->height == 0 && tower_and_options(tower, mask_and)) {
                                changed = true;
                                city->changed = true;
                            }

                            mask_and <<= 1;
                        }

                        enable_bit <<= 1;
                    }
                }
            } else if (clue - total_visible == hill_cnt) {
                /* Когда вариант остался один на каждый фрагмент - делаем одну ступеньку вниз
                 * после первого недостроенного здания.*/
                for (int hl_i = 0; hl_i < hill_cnt; hl_i++) {
                    if (hills[hl_i].vacant != 1) {
                        continue;
                    }

                    tower_t *tower = city_get_tower(city, side, pos, hills[hl_i].first);

                    if (hills[hl_i].shadow >= tower_get_min_height(tower)) {
                        continue;
                    }

                    int mask_and = tower_get_mask(1, tower_get_max_height(tower) - 1);
                    int enable_bit = 1;
                    int enable_mask = hills[hl_i].mask;

                    for (int tw_i = hills[hl_i].first + 1; tw_i <= hills[hl_i].last
                            && tw_i < first_highest; tw_i++) {
                        enable_bit <<= 1;

                        if ((enable_mask & enable_bit) != 0) {
                            tower_t *tower = city_get_tower(city, side, pos, tw_i);

                            if (tower->height == 0 && tower_and_options(tower, mask_and)) {
                                changed = true;
                                city->changed = true;
                            }
                        }
                    }
                }
            }
        }
    }

    free(hills);
    return  changed;
}
