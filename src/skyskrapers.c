/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkyScrapers
 * @details
 *
 * @date создан 25.09.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdio.h>

#include "skyskrapers/skyskrapers.h"
#include "skyskrapers/city.h"
#include "skyskrapers/street.h"
#include "skyskrapers/methods.h"

struct _handler {
    char *name;
    bool (* func)(const street_t *street);
} handlers[] = {
    {"obvious", method_obvious},
    {"exclude", method_exclude},
    {"first of two", method_first_of_two},
    {"staircase", method_staircase},
    {"step_down", method_step_down}
};

bool
city_solve_step(city_t *city)
{
    for (int i = 0; i < 4 * city->size; i++) {
        if (!city->need_handle[i]) {
            continue;
        }

        city->need_handle[i] = false;
        street_t *street = &city->streets[i];

        for (size_t j = 0; j < sizeof(handlers) / sizeof(struct _handler); j++) {
            if (handlers[j].func(street)) {
                fprintf(stdout, "Pass %s\n", handlers[j].name);
                return true;
            }
        }
    }

    return false;
}

bool
city_solve(city_t *city)
{
    while (!city_is_solved(city)) {
        if (!city_is_valid(city)) {
            fprintf(stdout, "ERROR\nInvalid city.\n");
            return false;
        }

        if (!city_solve_step(city)) {
            fprintf(stdout, "Bruteforce.\n");
            return method_bruteforce(city);
        }
    }

    return true;
}
