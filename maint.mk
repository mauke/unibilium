CFLAGS=-std=gnu99 -pedantic -Wall -Wextra -Wno-missing-field-initializers -Wundef -Wshadow -Wbad-function-cast -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -Winline -Wdisabled-optimization -O2 -pipe -fstack-protector-all -fsanitize=address -fsanitize=undefined
LDFLAGS=-fsanitize=address -fsanitize=undefined
