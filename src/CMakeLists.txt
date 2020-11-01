
set(SOURCE_C
   main.c
   skyskrapers.c
   core/city.c
   core/street.c
   core/tower.c
   methods/exclude.c
   methods/obvious.c
   methods/first_of_two.c
   methods/staircase.c
   methods/step_down.c
   methods/bruteforce.c)

add_executable(skyskrapers ${SOURCE_C})
