cc tiny_battle.c tiles.c structs.c logic.c util.c input.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Wextra -Wpedantic -O0 -g3 -fsanitize=address -fno-common -fno-omit-frame-pointer && ./a.out