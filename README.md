# SkySkrapers

  Решение логической головоломки «Небоскребы», также известной как «Башни».


## Правила головоломки

  Цель состоит в том, чтобы разместить небоскребы во всех ячейках сетки в
соответствии с правилами:

- Высота небоскребов составляет от 1 до размера сетки, то есть от 1 до 4 для
  головоломки 4x4.
- Нельзя разместить два небоскреба одинаковой высоты в одной строке или столбце.
- Цифры по сторонам сетки показывают, сколько небоскребов вы увидите, если
  посмотрите в направлении стрелки. Отсутствие цифры или ноль означает, что для
  данного направления подсказки нет.

Например, простой вариант 4x4:

```
     2   2   1   3
  -+---+---+---+---+-
3  |   |   |   |   |  2
  -+---+---+---+---+-
1  |   |   |   |   |  2
  -+---+---+---+---+-
2  |   |   |   |   |  3
  -+---+---+---+---+-
3  |   |   |   |   |  1
  -+---+---+---+---+-
     3   2   2   1
```

Результат решения головоломки:

```
     2   2   1   3
  -+---+---+---+---+-
3  | 1 | 3 | 4 | 2 |  2
  -+---+---+---+---+-
1  | 4 | 2 | 1 | 3 |  2
  -+---+---+---+---+-
2  | 3 | 4 | 2 | 1 |  3
  -+---+---+---+---+-
3  | 2 | 1 | 3 | 4 |  1
  -+---+---+---+---+-
     3   2   2   1
```


## Программная модель

  Модель головоломки состоит из двух основных структуры - `tower_r` и `city_t`,
для представления ячейки (или башни) и всей головоломки соответственно.

  Структура `tower_r` содержит поле `height` для обозначения известной высоты
башни и поле `options`, которое является набором битовых флагов допустимых
этажей. Наглядно:

```
Высота здания известна и равна 3.
height  = 0x03
options = 0x04   0000 0100

Высота здания может быть 1 или 3.
height  = 0x00
options = 0x05   0000 0101

Ошибочное состояние - высота неизвестна и вариантов нет.
height  = 0x00
options = 0x00   0000 0000

```

  Структура `city_t` содержит указатель на подсказки `clues`, указатель на
массив структур `towers` и флаг изменений `changed`. Этот флаг нужен для
предотвращения зацикливания при поиске эвристических решений.


## Базовое решение 4x4

  Самый очевидный способ решить головоломку -- это метод перебора (brute force).
И этот способ имеет очевидный же недостаток: даже для небольшого поля 4x4 может
потребоваться до 4,29e+9 итераций, а для поля 6x6 число комбинаций возрастает
примерно до 1,0e+28. Поэтому программа для решения 4x4 использует только
эвристические методы.

### Метод безусловной высоты

  Функция `city_do_obvious_highest` ищет в ряду здание с возможной высотой,
которой нет у других зданий ряда, и, если такое здание есть, присваивает зданию
эту высоту. Этот метод только немного ускоряет поиск решения.

### Метод исключения

  Функция `city_do_exclude` ищет в ряду здания с известной высотой и исключает
эти высоты из недостроенных зданий ряда.

### Метод ограничения первого

  Функция `city_do_first_of_many` ограничивает минимальную высоту первого здания
в ряду, где есть подсказка больше двух.


## Решение 5x5

  Головоломки этого уровня выявили ошибочное поведение функции
`city_do_first_of_many`.

### Метод горки

  Частный случай этого метода используется при загрузке подсказок, реализация
в виде отдельной функции должна позволить отказаться от `city_do_first_of_many`.

## Сборка

```
cd build
cmake ../
cmake --build .
```

## Полезные ссылки

- [Codewars :: 4 By 4 Skyscrapers](https://www.codewars.com/kata/5671d975d81d6c1c87000022)
- [Codewars :: 6 By 6 Skyscrapers](https://www.codewars.com/kata/5679d5a3f2272011d700000d)
- [Codewars :: 7x7 Skyscrapers](https://www.codewars.com/kata/5917a2205ffc30ec3a0000a8)
- [Codewars :: N By N Skyscrapers](https://www.codewars.com/kata/5f7f023834659f0015581df1)

