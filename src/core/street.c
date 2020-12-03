/* utf-8 */

/**
 * @file
 * @brief Представление ряда зданий.

 * @details Этот класс представляет ряд зданий от края пазла и содержит подсказку и
 * дополнительную информацию для эвристик.
 *
 * @date создан 18.10.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "skyskrapers/city.h"
#include "skyskrapers/tower.h"
#include "skyskrapers/street.h"

street_t *
street_make(street_t *in, city_t *parent, int side, int pos)
{
    assert(in != NULL);
    street_t *ret = in;
    assert(parent != NULL);
    ret->parent = parent;
    int size = parent->size;
    ret->size = size;
    assert(side >= 0 && side < size);
    assert(pos >= 0 && pos < size);
    ret->side = side;
    ret->pos = pos;
    ret->clue = 0;
    ret->valid = true;
    ret->highest_first = 0;
    ret->highest_last = 0;
    ret->visible = 0;
    ret->vacant = 0;
    ret->hill_count = 0;
    ret->hill_array = malloc((unsigned int) size * sizeof(hill_t));
    ret->towers = malloc((unsigned int) size * sizeof(tower_t *));

    for (int i = 0; i < size; i++) {
        ret->towers[i] = city_get_tower(parent, side, pos, i);
    }

    return ret;
}

street_t *
street_copy(street_t *dst, const street_t *src)
{
    assert(dst != NULL);
    street_t *ret = dst;
    assert(src != NULL);
    assert(src->size == ret->size);

    if (ret == src) {
        return ret;
    }

    ret->parent = src->parent;
    ret->side = src->side;
    ret->pos = src->pos;
    ret->clue = src->clue;
    ret->valid = src->valid;
    ret->highest_first = src->highest_first;
    ret->highest_last = src->highest_last;
    ret->visible = src->visible;
    ret->vacant = src->vacant;
    ret->hill_count = src->hill_count;
    unsigned int sz = (unsigned int) src->size;
    memcpy(ret->hill_array, src->hill_array, sz * sizeof(hill_t));
    memcpy(ret->towers, src->towers, sz * sizeof(tower_t *));
    return ret;
}

void
street_free(street_t *street)
{
    free(street->hill_array);
    free(street->towers);
}

void
street_set_clue(street_t *street, int clue)
{
    assert(street != NULL);
    assert(clue >= 0 && clue <= street->size);
    int old_clue = street->clue;
    street->clue = clue;

    if (old_clue != street->clue) {
        city_notify_of_street_change(street->parent, street->side, street->pos);
    }
}

int
street_get_clue(const street_t *street)
{
    assert(street != NULL);
    return street->clue;
}

tower_t *
street_get_tower(const street_t *street, int index)
{
    assert(street != NULL);
    assert(index >= 0 && index < street->size);
    return street->towers[index];
}

/**
 * @brief Устанавливает начальные ограничения.
 * @details После загрузки начальной конфигурации при помощи функций city_set_heights()
 * и/или city_set_clues() эта функция делает несколько простых ограничений зданий.
 *
 * @param street ряд для быстрых ограничений.
 */
void
street_fast_constraint(street_t *street)
{
    assert(street != NULL);
    tower_t *tower;
    int size = street->size;
    int clue = street->clue;
    int options = street->parent->mask;

    if (clue == 1) {
        tower = street_get_tower(street, 0);
        tower_set_height(tower, size);
        options >>= 1;

        for (int i = 1; i < size; i++) {
            tower = street_get_tower(street, i);

            if (tower_has_floors(tower, options)) {
                tower_and_options(tower, options);
            }
        }
    } else if (clue == size) {
        for (int i = 0; i < size; i++) {
            tower = street_get_tower(street, i);
            tower_set_height(tower, i + 1);
        }
    } else if (clue > 0) {
        for (int i = size; i > 0; i--) {
            if (i < clue) {
                options >>= 1;
            }

            tower = street_get_tower(street, i - 1);

            if (tower_has_floors(tower, options)) {
                tower_and_options(tower, options);
            }
        }
    }
}

/** Получение индекса первого здания с максимальной высотой. */
static int
find_highest_first(street_t *street);

/** Получение индекса последнего доступного здания с максимальной высотой. */
static int
find_highest_last(street_t *street);

static bool
update_hill(street_t *street);

static bool
check_valid(street_t *street);

bool
street_update(street_t *street)
{
    assert(street != NULL);
    street->highest_first = find_highest_first(street);
    street->highest_last = find_highest_last(street);

    if (!check_valid(street) || !update_hill(street)) {
        street->valid = false;
    }

    return street->valid;
}

/*+************************************
 *  PROTECTED
 **************************************/

int
find_highest_first(street_t *street)
{
    int size = street->size;
    int highest = size - 1;
    int mask = tower_get_mask(size, size);

    for (int i = 0; i  < size; i++) {
        tower_t *tower = street_get_tower(street, i);

        if (tower_has_floors(tower, mask)) {
            highest = i;
            break;
        }
    }

    return highest;
}

int
find_highest_last(street_t *street)
{
    int size = street->size;
    int highest = size - 1;
    int mask = tower_get_mask(size, size);

    for (int i = 0; i  < size; i++) {
        tower_t *tower = street_get_tower(street, i);

        if (tower_has_floors(tower, mask)) {
            highest = i;
        }

        if (tower_get_height(tower) == size) {
            break;
        }
    }

    return highest;
}

bool
update_hill(street_t *street)
{
    int size = street->size;
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
    hill_t *hills = street->hill_array;
    int bit = 1;
    memset(hills, 0, (unsigned int) size * sizeof(hill_t));

    /* Сбор статистики идёт до последнего возможно самого высокого здания. */
    for (int i = 0; i  <= street->highest_last; i++) {
        tower_t *tower = street_get_tower(street, i);
        int height = tower_get_height(tower);

        if (height == size) {
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
            hills[hill_cnt].action_mask |= bit;
        }

        bit <<= 1;
    }

    if (hill_size) {
        hills[hill_cnt].last = hills[hill_cnt].first + hill_size - 1;
        hill_cnt++;
    }

    street->hill_count = hill_cnt;
    street->vacant = total_vacant;
    street->visible = total_visible;

    return true;
}

typedef struct _check_info {
    int size;
    int highest;
    int visible;
    int foreground;
    int offstage;
    bool valid;
    int options;
} check_info_t;

static void
check_info_reset(check_info_t *info)
{
    info->highest = 0;
    info->visible = 0;
    info->foreground = 0;
    info->offstage = 0;
    info->valid = true;
    info->options = 0;
}

static void
check_info_add(check_info_t *info, tower_t *tower)
{
    int height = tower_get_height(tower);
    int options = tower_get_options(tower);

    if (options == 0) {
        info->valid = false;
    }

    if (height != 0) {
        info->options ^= options;

        if ((info->options & options) == 0) {
            info->valid = false;
        }
    } else if (info->highest < tower_get_size(tower)) {
        if (info->highest == 0) {
            info->foreground++;
        } else {
            info->offstage++;
        }
    }

    if (info->highest < height) {
        info->visible++;
        info->highest = height;
    }
}

bool
check_valid(street_t *street)
{
    check_info_t info;
    check_info_reset(&info);

    for (int i = 0; i < street->size; i++) {
        tower_t *tower = street_get_tower(street, i);
        check_info_add(&info, tower);

        if (!info.valid) {
            return false;
        }
    }

    int clue = street->clue;

    if (clue == 0) {
        return true;
    }

    if (info.visible > clue + info.foreground + info.offstage) {
        return false;
    }

    if (info.visible + info.foreground + info.offstage < clue) {
        return false;
    }

    return  true;
}
