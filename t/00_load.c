#include <unibilium.h>
#include <stdio.h>

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);

    puts("1..2");

    unibi_term *ut = unibi_dummy();
    puts("ok 1 - create dummy terminal");

    unibi_destroy(ut);
    puts("ok 2 - destroy dummy terminal");

    return 0;
}
