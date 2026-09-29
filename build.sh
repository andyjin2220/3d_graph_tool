#!/bin/bash
NC=$(brew --prefix ncurses)
gcc *.c -o app -I$NC/include -L$NC/lib -lncurses -lm && ./app
