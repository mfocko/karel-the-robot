# TODO

## NCurses alternatives

-   move from ncurses to s-lang
    -   avoid usage of movek() function for Karel's movement because of ncurses move() function
    -   or maybe termbox (but no support for windows :-( http://code.google.com/p/termbox/
-   SDL support for GUI
    -   more interesting ;)

## Internationalization

-   gettext support for error messages

## Karel and Super Karel

- karel should contain only the basic vocabulary:

    * `turn_on()`
    * `turn_off()`
    * `move()` or `step()`
    * `turn_left()`
    * `pick_beeper()`
    * `put_beeper()`

- karel should be equipped with following basic sensors:

    * `front_is_clear()`
    * `facing_north()`
    * `beepers_present()`
    * `beepers_in_bag()`

-   super karel extension
    -   "batteries included" Karel


## Naming Conventions

* rewrite from Java/C++ style to C style
