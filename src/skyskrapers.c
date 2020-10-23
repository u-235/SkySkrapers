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

#include <stdio.h>

#include "skyskrapers/skyskrapers.h"
#include "skyskrapers/city.h"
#include "skyskrapers/methods.h"

bool
city_solve(city_t *city)
{
    for (int pass = 1; !city_is_solved(city); pass++) {
        if (!city_is_valid(city)) {
            fprintf(stdout, "ERROR\nInvalid city.\n");
            return false;
        }

        fprintf(stdout, "Pass %i\n", pass);

        if (method_obvious(city)) {
            fprintf(stdout, "Obvious highest\n");
            city_print(city);
        }

        if (method_exclude(city)) {
            fprintf(stdout, "Exclude\n");
            city_print(city);
        }

        if (method_first_of_two(city)) {
            fprintf(stdout, "First of two\n");
            city_print(city);
        }

        if (method_slope(city)) {
            fprintf(stdout, "Slope\n");
            city_print(city);
        }

        if (city_is_deadloop(city)) {
            fprintf(stdout, "The number of possible states is %llu\n", city_calc_iteration(city));
            return method_bruteforce(city);
        }
    }

    return true;
}
