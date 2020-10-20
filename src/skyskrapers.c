/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkySkrapers
 * @details
 *
 * @date создан 25.09.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "skyskrapers.h"

bool
city_is_valid(city_t *city);

bool
city_is_solved(city_t *city);

bool
city_is_deadloop(city_t *city);

bool
city_do_obvious_highest(city_t *city);

bool
city_do_exclude(city_t *city);

/**
 * Ограничивает высоту недостроенных зданий в ряду с подсказкой "2". Высота этих зданий не может
 * быть выше чем максимальная возможная высота первого здания минус один этаж.
 *
 * @param city Проверяемый город
 *
 * @return true если были изменения в @p city.
 */
bool
city_do_first_of_two(city_t *city);

bool
city_do_slope(city_t *city);

bool
city_do_bruteforce(city_t *city);

unsigned long long
city_calc_iteration(city_t *city);

enum _sides {
    TOP, RIGHT, BOTTOM, LEFT
};

typedef struct _tower {
    int size;
    int height;
    int options;
    int weight;
} tower_t;

typedef struct _city {
    int size;
    int mask;
    tower_t *towers;
    int *clues;
    bool changed;
    bool must_free;
} city_t;

bool
city_solve(city_t *city)
{
    for (int pass = 1; !city_is_solved(city); pass++) {
        if (!city_is_valid(city)) {
            fprintf(stdout, "ERROR\nInvalid city.\n");
            return false;
        }

        fprintf(stdout, "Pass %i\n", pass);

        if (city_do_obvious_highest(city)) {
            fprintf(stdout, "Obvious highest\n");
            city_print(city);
        }

        if (city_do_exclude(city)) {
            fprintf(stdout, "Exclude\n");
            city_print(city);
        }

        if (city_do_first_of_two(city)) {
            fprintf(stdout, "First of two\n");
            city_print(city);
        }

        if (city_do_slope(city)) {
            fprintf(stdout, "Slope\n");
            city_print(city);
        }

        if (city_is_deadloop(city)) {
            fprintf(stdout, "The number of possible states is %llu\n", city_calc_iteration(city));
            return city_do_bruteforce(city);
        }
    }

    return true;
}

int
city_get_clue(city_t *city, int side, int pos)
{
    return city->clues[side * city->size + pos];
}

tower_t *
city_get_tower(city_t *city, int side, int pos, int index)
{
    int x = 0, y = 0;

    switch (side) {
    case TOP:
        x = pos;
        y = index;
        break;

    case RIGHT:
        x = city->size - 1 - index;
        y = pos;
        break;

    case BOTTOM:
        x = city->size - 1 - pos;
        y = city->size - 1 - index;
        break;

    case LEFT:
        x = index;
        y = city->size - 1 - pos;
        break;

    default:
        fprintf(stderr, "ERROR\nget_tower : bad side=%i\n", side);
        abort();
    }

    return &city->towers[x + y * city->size];
}

bool
tower_set_height(tower_t *tower, int height)
{
    if (height < 1 || height > tower->size) {
        fprintf(stderr, "ERROR\ntower_set_height : bad height=%i\n", height);
        abort();
    }

    if (tower->height != 0 && tower->height != height) {
        fprintf(stderr,
                "ERROR\ntower_set_height : height conflict old=%i, new=%i\n",
                tower->height, height);
        abort();
    }

    int old = tower->height;
    tower->height = height;
    tower->options = 1 << (height - 1);
    return old != tower->height;
}

bool
tower_and_options(tower_t *tower, int options)
{
    if (tower->height != 0) {
        fprintf(stdout, "WARNING\ntower_and_options : tower is finished\n");
    }

    if ((tower->options & options) == 0) {
        fprintf(stdout, "ERROR\ntower_and_options : no options\n");
        //abort();
    }

    int old = tower->options;
    tower->options &= options;

    int t = 1;

    for (int h = 1; h <= tower->size; h++) {
        if (t == tower->options) {
            tower->height = h;
            break;
        }

        t <<= 1;
    }

    return old != tower->options;
}

/**
 * Вычисление допустимой минимальной высоты здания.
 *
 * @param tower Указатель на здание.
 *
 * @return Минимальная высота здания, от 1 до @p tower->size.
 */
int
tower_get_min_height(const tower_t *tower)
{
    if (tower->options == 0) {
        return 0;
    }

    int mask = 1;

    for (int i = 1; i <= tower->size; i++) {
        if (tower->options & mask) {
            return i;
        }

        mask <<= 1;
    }

    // ! Error
    return -1;
}

/**
 * Вычисление допустимой максимальной высоты здания.
 *
 * @param tower Указатель на здание.
 *
 * @return Максимальная высота здания, от 1 до @p tower->size.
 */

int
tower_get_max_height(const tower_t *tower)
{
    if (tower->options == 0) {
        return 0;
    }

    int mask = 1 << (tower->size - 1);

    for (int i = tower->size; i > 0; i--) {
        if (tower->options & mask) {
            return i;
        }

        mask >>= 1;
    }

    // ! Error
    return -1;
}

int
tower_get_mask(int bottom, int top)
{
    if (bottom > top) {
        // ! Error
        return 0;
    }

    int mask = 1 << (bottom - 1);

    for (; top > bottom; top--) {
        mask |= mask << 1;
    }

    return mask;
}

city_t *
city_init(city_t *in, int size)
{
    city_t *ret;

    if (in == 0) {
        ret = malloc(sizeof(city_t));
        ret->must_free = true;
    } else {
        ret = in;
        ret->must_free = false;
    }

    ret->clues = 0;
    ret->size = size;
    ret->changed = false;
    ret->towers = malloc(size * size * sizeof(tower_t));

    int m = 1;

    for (int i = 0; i < size; i++) {
        ret->mask |= m;
        m <<= 1;
    }

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            tower_t *tower = city_get_tower(ret, 0, x, y);
            tower->size = size;
            tower->height = 0;
            tower->options = ret->mask;
        }
    }

    return ret;
}

city_t *
city_new(int size)
{
    return city_init(0, size);
}

void
city_free(city_t *city)
{
    free(city->towers);

    if (city->must_free) {
        free(city);
    }
}

city_t *
city_copy(city_t *dst, const city_t *src)
{
    city_t *ret = dst;

    if (dst == 0) {
        ret = city_init(0, src->size);
    }

    ret->size = src->size;
    ret->mask = src->mask;
    ret->clues = src->clues;
    ret->changed = src->changed;
    memcpy(ret->towers, src->towers, src->size * src->size * sizeof(tower_t));

    return ret;
}

void
city_load(city_t *city, int *clues)
{
    city->clues = clues;

    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int clue = city_get_clue(city, side, pos);

            if (clue == 0) {
                continue;
            }

            tower_t *tower;
            int options = city->mask;

            if (clue == 1) {
                tower = city_get_tower(city, side, pos, 0);
                tower_set_height(tower, city->size);
                options >>= 1;

                for (int i = 1; i < city->size; i++) {
                    tower = city_get_tower(city, side, pos, i);

                    if (tower->height == 0) {
                        tower_and_options(tower, options);
                    }
                }
            } else if (clue == city->size) {
                for (int i = 0; i < city->size; i++) {
                    tower = city_get_tower(city, side, pos, i);
                    tower_set_height(tower, i + 1);
                }
            } else {
                for (int i = city->size; i > 0; i--) {
                    if (i < clue) {
                        options >>= 1;
                    }

                    tower = city_get_tower(city, side, pos, i - 1);

                    if (tower->height == 0) {
                        tower_and_options(tower, options);
                    }
                }
            }
        }
    }
}

typedef struct _view_info {
    int size;
    int highest;
    int visible;
    int foreground;
    int offstage;
    bool valid;
    int options;
} view_info_t;

void
view_info_reset(view_info_t *info, int size)
{
    info->size = size;
    info->highest = 0;
    info->visible = 0;
    info->foreground = 0;
    info->offstage = 0;
    info->valid = true;
    info->options = 0;
}

void
view_info_add(view_info_t *info, int height, int options)
{
    if (height != 0) {
        info->options ^= options;

        if ((info->options & options) == 0) {
            info->valid = false;
        }
    } else if (info->highest < info->size) {
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
city_is_valid(city_t *city)
{
    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {

            view_info_t info;
            view_info_reset(&info, city->size);

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);
                view_info_add(&info, tower->height, tower->options);

                if (!info.valid) {
                    return false;
                }
            }

            int clue = city_get_clue(city, side, pos);

            if (clue == 0) {
                continue;
            }

            if (info.visible > clue) {
                return false;
            }

            if (info.visible + info.foreground + info.offstage < clue) {
                return false;
            }

            if ((info.visible + info.foreground) == 0 && info.offstage != clue) {
                return false;
            }
        }
    }

    return  true;
}

bool
city_is_solved(city_t *city)
{
    if (!city_is_valid(city)) {
        return false;
    }

    for (int x = 0; x < city->size; x++) {
        for (int y = 0; y < city->size; y++) {
            if (city_get_tower(city, 0, x, y)->height == 0) {
                return false;
            }
        }
    }

    return true;
}

bool
city_is_deadloop(city_t *city)
{
    bool changes = city->changed;
    city->changed = false;
    return !changes;
}

bool
city_do_obvious_highest(city_t *city)
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

bool
city_do_exclude(city_t *city)
{
    bool changed = false;

    for (int side = 0; side <= 1; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int options = city->mask;

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);

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

bool
city_do_first_of_two(city_t *city)
{
    bool changed = false;

    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int clue = city_get_clue(city, side, pos);
            tower_t *tower = city_get_tower(city, side, pos, 0);

            if (clue != 2 || tower->height != 0) {
                continue;
            }

            int top = 1 << (city->size - 1);
            int mask = city->mask >> (city->size + 1 - tower_get_max_height(tower));
            int limit = tower->height;

            for (int i = 1; i < city->size; i++) {
                tower = city_get_tower(city, side, pos, i);

                if (tower->height > limit) {
                    break;
                }

                if ((tower->options & top) != 0) {
                    if (tower->height == 0 && tower_and_options(tower, top | mask)) {
                        changed = true;
                        city->changed = true;
                    }

                    break;
                }

                if (tower_and_options(tower, mask)) {
                    changed = true;
                    city->changed = true;
                }
            }
        }
    }

    return  changed;
}

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
city_do_slope(city_t *city)
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

                            if (tower_and_options(tower, mask_and)) {
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
                    if (hills[hl_i].vacant == 0) {
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
                        if ((enable_mask & enable_bit) != 0) {
                            tower_t *tower = city_get_tower(city, side, pos, tw_i);

                            if (tower_and_options(tower, mask_and)) {
                                changed = true;
                                city->changed = true;
                            }
                        }

                        enable_bit <<= 1;
                    }
                }
            }
        }
    }

    free(hills);
    return  changed;
}

bool
city_do_bruteforce(city_t *city)
{
    tower_t *tower;
    int x = 0, y = 0, max = 0;

    /* Вычисление оптимальной точки для перебора.
     * Сначала в каждой колонке этажи недостроенных зданий суммируются и эта сумма записывается
     * в поле weight зданий колонки.*/
    for (int iy = 0; iy < city->size; iy++) {
        int sum = 0;

        for (int ix = 0; ix < city->size; ix++) {
            tower = city_get_tower(city, 0, ix, iy);

            if (tower->height == 0) {
                sum += tower->options;
            }
        }

        for (int ix = 0; ix < city->size; ix++) {
            city_get_tower(city, 0, ix, iy)->weight = sum;
        }
    }

    max = 0;

    /* Затем находится такие же суммы для строк и эти суммы плюсуются с полем weight. Попутно
     * запоминается недостроенное здание с самым большим весом. */
    for (int ix = 0; ix < city->size; ix++) {
        int sum = 0;

        for (int iy = 0; iy < city->size; iy++) {
            tower = city_get_tower(city, 0, ix, iy);

            if (tower->height == 0) {
                sum += tower->options;
            }
        }

        for (int iy = 0; iy < city->size; iy++) {
            tower = city_get_tower(city, 0, ix, iy);
            int w = tower->weight + sum;

            if (tower->height == 0 && w > max) {
                x = ix;
                y = iy;
                max = w;
            }
        }
    }

    /* Для возврата состояния города при неудачной попытке перебора делаем копию.
     * А для перебора только допустимых высот заводим bit_enable. Это немного ускорит поиск. */
    tower = city_get_tower(city, 0, x, y);
    city_t *backup = city_copy(0, city);
    int bit_enable = 1 << (city->size - 1);

    for (int i = city->size; i > 0 ; i--) {
        if ((bit_enable & tower->options) != 0) {
            tower_set_height(tower, i);

            if (city_solve(city)) {
                city_free(backup);
                return true;
            }
        }

        bit_enable >>= 1;
        city_copy(city, backup);
    }

    city_free(backup);
    return false;
}

int **
city_export(city_t *city)
{
    int **ret = malloc(sizeof(int *) * city->size);

    for (int y = 0; y < city->size; y++) {
        int *t = malloc(sizeof(int) * city->size);
        ret[y] = t;

        for (int x = 0; x < city->size; x++) {
            tower_t *tower = city_get_tower(city, 0, x, y);
            t[x] = tower->height;
        }
    }

    return ret;
}

unsigned long long
city_calc_iteration(city_t *city)
{
    unsigned long long result = 1;
    unsigned i = 0;

    for (int x = 0; x < city->size; x++) {
        for (int y = 0; y < city->size; y++) {
            tower_t *tower = city_get_tower(city, 0, x, y);

            if (tower->height == 0) {
                unsigned int v = 0;
                int m = 1;

                for (int h = 0; h < city->size; h++) {
                    if ((tower->options & m) != 0) {
                        v++;
                    }

                    m <<= 1;
                }

                i++;
                result *= v;
            }
        }
    }

    /* Раньше возвращался просто result.
     * Как посчитать реальное значение я не знаю. */
    return result * 2 / i / 3;
}

void
city_print(city_t *city)
{
    FILE *io = stdout;

    fprintf(io, "==============================\n  ");

    for (int x = 0; x < city->size; x++) {
        fprintf(io, "  %3.1i ", city_get_clue(city, 0, x));
    }

    fprintf(io, "\n");

    for (int y = 0; y < city->size; y++) {
        for (int b = city->size; b > 0; b--) {
            if (b == 3) {
                fprintf(io, "%3.1i", city_get_clue(city, 3, city->size - 1 - y));
            } else {
                fprintf(io, "   ");
            }

            for (int x = 0; x < city->size; x++) {
                tower_t *tower = city_get_tower(city, 0, x, y);
                int h = tower->height;
                int o = tower->options;

                if (h == 0) {
                    fprintf(io, " %s ", (o & (1 << (b - 1))) == 0 ? " -- " : " ++ ");
                } else {
                    fprintf(io, " %s ", h >= b ? "####" : "    ");
                }
            }

            if (b == 3) {
                fprintf(io, "%-3.1i", city_get_clue(city, 1, y));
            }

            fprintf(io, "\n");
        }

        if (y < city->size - 1) {
            fprintf(io, "\n");
        }
    }

    fprintf(io, "  ");

    for (int x = 0; x < city->size; x++) {
        fprintf(io, "  %3.1i ", city_get_clue(city, 2, city->size - 1 - x));
    }

    fprintf(io, "\n------------------------------\n\n");
    fflush(io);
}
