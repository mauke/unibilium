#include <unibilium.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static unsigned test_counter;

static void plan(unsigned n) {
    printf("1..%u\n", n);
}

static void ok(int b, const char *s) {
    printf("%sok %u - %s\n", b ? "" : "not ", test_counter++, s);
}

static void bail_out(const char *s) {
    printf("Bail out! %s\n", s);
    exit(EXIT_FAILURE);
}

static void diag(const char *s) {
    printf("# %s\n", s);
}

const char terminfo[] = {
    26, 1, 48, 0, 38, 0, 15, 0, -99, 1, 108, 5, 120, 116, 101, 114, 109, 124, 120, 116,
    101, 114, 109, 32, 116, 101, 114, 109, 105, 110, 97, 108, 32, 101, 109, 117, 108, 97, 116, 111,
    114, 32, 40, 88, 32, 87, 105, 110, 100, 111, 119, 32, 83, 121, 115, 116, 101, 109, 41, 0,
    0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 80, 0,
    8, 0, 24, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 8, 0, 64, 0, 0, 0, 4, 0, 6, 0, 8, 0, 25, 0, 30, 0,
    38, 0, 42, 0, 46, 0, -1, -1, 57, 0, 74, 0, 76, 0, 80, 0, 87, 0, -1, -1,
    89, 0, 102, 0, -1, -1, 106, 0, 110, 0, 120, 0, 124, 0, -1, -1, -1, -1, -128, 0,
    -124, 0, -119, 0, -114, 0, -1, -1, -1, -1, -105, 0, -100, 0, -1, -1, -95, 0, -90, 0,
    -85, 0, -80, 0, -71, 0, -67, 0, -60, 0, -1, -1, -51, 0, -46, 0, -40, 0, -34, 0,
    -1, -1, -1, -1, -1, -1, -16, 0, -1, -1, -1, -1, -1, -1, 2, 1, -1, -1, 6, 1,
    -1, -1, -1, -1, -1, -1, 8, 1, -1, -1, 13, 1, -1, -1, -1, -1, -1, -1, -1, -1,
    17, 1, 21, 1, 27, 1, 31, 1, 35, 1, 39, 1, 45, 1, 51, 1, 57, 1, 63, 1,
    69, 1, 73, 1, -1, -1, 78, 1, -1, -1, 82, 1, 87, 1, 92, 1, 96, 1, 103, 1,
    -1, -1, 110, 1, 114, 1, 122, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -126, 1, -117, 1, -1, -1, -1, -1, -108, 1,
    -99, 1, -90, 1, -81, 1, -72, 1, -63, 1, -54, 1, -45, 1, -36, 1, -27, 1, -1, -1,
    -1, -1, -1, -1, -18, 1, -14, 1, -9, 1, -1, -1, -4, 1, -1, 1, -1, -1, -1, -1,
    17, 2, 20, 2, 31, 2, 34, 2, 36, 2, 39, 2, 121, 2, -1, -1, 124, 2, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 126, 2, -1, -1, -1, -1, -1, -1, -1, -1,
    -126, 2, -1, -1, -73, 2, -1, -1, -1, -1, -69, 2, -63, 2, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -57, 2, -53, 2,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -49, 2, -1, -1, -1, -1, -42, 2, -1, -1,
    -1, -1, -1, -1, -1, -1, -35, 2, -28, 2, -21, 2, -1, -1, -1, -1, -14, 2, -1, -1,
    -7, 2, -1, -1, -1, -1, -1, -1, 0, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    7, 3, 13, 3, 19, 3, 26, 3, 33, 3, 40, 3, 47, 3, 55, 3, 63, 3, 71, 3,
    79, 3, 87, 3, 95, 3, 103, 3, 111, 3, 118, 3, 125, 3, -124, 3, -117, 3, -109, 3,
    -101, 3, -93, 3, -85, 3, -77, 3, -69, 3, -61, 3, -53, 3, -46, 3, -39, 3, -32, 3,
    -25, 3, -17, 3, -9, 3, -1, 3, 7, 4, 15, 4, 23, 4, 31, 4, 39, 4, 46, 4,
    53, 4, 60, 4, 67, 4, 75, 4, 83, 4, 91, 4, 99, 4, 107, 4, 115, 4, 123, 4,
    -125, 4, -118, 4, -111, 4, -104, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -99, 4, -88, 4, -83, 4,
    -75, 4, -71, 4, -1, -1, -1, -1, -1, -1, -1, -1, -62, 4, 8, 5, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 78, 5,
    -1, -1, -1, -1, -1, -1, 82, 5, 92, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 102, 5, 105, 5, 27, 91, 90, 0, 7, 0,
    13, 0, 27, 91, 37, 105, 37, 112, 49, 37, 100, 59, 37, 112, 50, 37, 100, 114, 0, 27,
    91, 51, 103, 0, 27, 91, 72, 27, 91, 50, 74, 0, 27, 91, 75, 0, 27, 91, 74, 0,
    27, 91, 37, 105, 37, 112, 49, 37, 100, 71, 0, 27, 91, 37, 105, 37, 112, 49, 37, 100,
    59, 37, 112, 50, 37, 100, 72, 0, 10, 0, 27, 91, 72, 0, 27, 91, 63, 50, 53, 108,
    0, 8, 0, 27, 91, 63, 49, 50, 108, 27, 91, 63, 50, 53, 104, 0, 27, 91, 67, 0,
    27, 91, 65, 0, 27, 91, 63, 49, 50, 59, 50, 53, 104, 0, 27, 91, 80, 0, 27, 91,
    77, 0, 27, 40, 48, 0, 27, 91, 53, 109, 0, 27, 91, 49, 109, 0, 27, 91, 63, 49,
    48, 52, 57, 104, 0, 27, 91, 52, 104, 0, 27, 91, 56, 109, 0, 27, 91, 55, 109, 0,
    27, 91, 55, 109, 0, 27, 91, 52, 109, 0, 27, 91, 37, 112, 49, 37, 100, 88, 0, 27,
    40, 66, 0, 27, 40, 66, 27, 91, 109, 0, 27, 91, 63, 49, 48, 52, 57, 108, 0, 27,
    91, 52, 108, 0, 27, 91, 50, 55, 109, 0, 27, 91, 50, 52, 109, 0, 27, 91, 63, 53,
    104, 36, 60, 49, 48, 48, 47, 62, 27, 91, 63, 53, 108, 0, 27, 91, 33, 112, 27, 91,
    63, 51, 59, 52, 108, 27, 91, 52, 108, 27, 62, 0, 27, 91, 76, 0, 8, 0, 27, 91,
    51, 126, 0, 27, 79, 66, 0, 27, 79, 80, 0, 27, 91, 50, 49, 126, 0, 27, 79, 81,
    0, 27, 79, 82, 0, 27, 79, 83, 0, 27, 91, 49, 53, 126, 0, 27, 91, 49, 55, 126,
    0, 27, 91, 49, 56, 126, 0, 27, 91, 49, 57, 126, 0, 27, 91, 50, 48, 126, 0, 27,
    79, 72, 0, 27, 91, 50, 126, 0, 27, 79, 68, 0, 27, 91, 54, 126, 0, 27, 91, 53,
    126, 0, 27, 79, 67, 0, 27, 91, 49, 59, 50, 66, 0, 27, 91, 49, 59, 50, 65, 0,
    27, 79, 65, 0, 27, 91, 63, 49, 108, 27, 62, 0, 27, 91, 63, 49, 104, 27, 61, 0,
    27, 91, 63, 49, 48, 51, 52, 108, 0, 27, 91, 63, 49, 48, 51, 52, 104, 0, 27, 91,
    37, 112, 49, 37, 100, 80, 0, 27, 91, 37, 112, 49, 37, 100, 77, 0, 27, 91, 37, 112,
    49, 37, 100, 66, 0, 27, 91, 37, 112, 49, 37, 100, 64, 0, 27, 91, 37, 112, 49, 37,
    100, 83, 0, 27, 91, 37, 112, 49, 37, 100, 76, 0, 27, 91, 37, 112, 49, 37, 100, 68,
    0, 27, 91, 37, 112, 49, 37, 100, 67, 0, 27, 91, 37, 112, 49, 37, 100, 84, 0, 27,
    91, 37, 112, 49, 37, 100, 65, 0, 27, 91, 105, 0, 27, 91, 52, 105, 0, 27, 91, 53,
    105, 0, 27, 99, 0, 27, 91, 33, 112, 27, 91, 63, 51, 59, 52, 108, 27, 91, 52, 108,
    27, 62, 0, 27, 56, 0, 27, 91, 37, 105, 37, 112, 49, 37, 100, 100, 0, 27, 55, 0,
    10, 0, 27, 77, 0, 37, 63, 37, 112, 57, 37, 116, 27, 40, 48, 37, 101, 27, 40, 66,
    37, 59, 27, 91, 48, 37, 63, 37, 112, 54, 37, 116, 59, 49, 37, 59, 37, 63, 37, 112,
    50, 37, 116, 59, 52, 37, 59, 37, 63, 37, 112, 49, 37, 112, 51, 37, 124, 37, 116, 59,
    55, 37, 59, 37, 63, 37, 112, 52, 37, 116, 59, 53, 37, 59, 37, 63, 37, 112, 55, 37,
    116, 59, 56, 37, 59, 109, 0, 27, 72, 0, 9, 0, 27, 79, 69, 0, 96, 96, 97, 97,
    102, 102, 103, 103, 105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 111, 111, 112, 112,
    113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118, 119, 119, 120, 120, 121, 121, 122, 122,
    123, 123, 124, 124, 125, 125, 126, 126, 0, 27, 91, 90, 0, 27, 91, 63, 55, 104, 0, 27,
    91, 63, 55, 108, 0, 27, 79, 70, 0, 27, 79, 77, 0, 27, 91, 51, 59, 50, 126, 0,
    27, 91, 49, 59, 50, 70, 0, 27, 91, 49, 59, 50, 72, 0, 27, 91, 50, 59, 50, 126,
    0, 27, 91, 49, 59, 50, 68, 0, 27, 91, 54, 59, 50, 126, 0, 27, 91, 53, 59, 50,
    126, 0, 27, 91, 49, 59, 50, 67, 0, 27, 91, 50, 51, 126, 0, 27, 91, 50, 52, 126,
    0, 27, 91, 49, 59, 50, 80, 0, 27, 91, 49, 59, 50, 81, 0, 27, 91, 49, 59, 50,
    82, 0, 27, 91, 49, 59, 50, 83, 0, 27, 91, 49, 53, 59, 50, 126, 0, 27, 91, 49,
    55, 59, 50, 126, 0, 27, 91, 49, 56, 59, 50, 126, 0, 27, 91, 49, 57, 59, 50, 126,
    0, 27, 91, 50, 48, 59, 50, 126, 0, 27, 91, 50, 49, 59, 50, 126, 0, 27, 91, 50,
    51, 59, 50, 126, 0, 27, 91, 50, 52, 59, 50, 126, 0, 27, 91, 49, 59, 53, 80, 0,
    27, 91, 49, 59, 53, 81, 0, 27, 91, 49, 59, 53, 82, 0, 27, 91, 49, 59, 53, 83,
    0, 27, 91, 49, 53, 59, 53, 126, 0, 27, 91, 49, 55, 59, 53, 126, 0, 27, 91, 49,
    56, 59, 53, 126, 0, 27, 91, 49, 57, 59, 53, 126, 0, 27, 91, 50, 48, 59, 53, 126,
    0, 27, 91, 50, 49, 59, 53, 126, 0, 27, 91, 50, 51, 59, 53, 126, 0, 27, 91, 50,
    52, 59, 53, 126, 0, 27, 91, 49, 59, 54, 80, 0, 27, 91, 49, 59, 54, 81, 0, 27,
    91, 49, 59, 54, 82, 0, 27, 91, 49, 59, 54, 83, 0, 27, 91, 49, 53, 59, 54, 126,
    0, 27, 91, 49, 55, 59, 54, 126, 0, 27, 91, 49, 56, 59, 54, 126, 0, 27, 91, 49,
    57, 59, 54, 126, 0, 27, 91, 50, 48, 59, 54, 126, 0, 27, 91, 50, 49, 59, 54, 126,
    0, 27, 91, 50, 51, 59, 54, 126, 0, 27, 91, 50, 52, 59, 54, 126, 0, 27, 91, 49,
    59, 51, 80, 0, 27, 91, 49, 59, 51, 81, 0, 27, 91, 49, 59, 51, 82, 0, 27, 91,
    49, 59, 51, 83, 0, 27, 91, 49, 53, 59, 51, 126, 0, 27, 91, 49, 55, 59, 51, 126,
    0, 27, 91, 49, 56, 59, 51, 126, 0, 27, 91, 49, 57, 59, 51, 126, 0, 27, 91, 50,
    48, 59, 51, 126, 0, 27, 91, 50, 49, 59, 51, 126, 0, 27, 91, 50, 51, 59, 51, 126,
    0, 27, 91, 50, 52, 59, 51, 126, 0, 27, 91, 49, 59, 52, 80, 0, 27, 91, 49, 59,
    52, 81, 0, 27, 91, 49, 59, 52, 82, 0, 27, 91, 49, 75, 0, 27, 91, 37, 105, 37,
    100, 59, 37, 100, 82, 0, 27, 91, 54, 110, 0, 27, 91, 63, 49, 59, 50, 99, 0, 27,
    91, 99, 0, 27, 91, 51, 57, 59, 52, 57, 109, 0, 27, 91, 51, 37, 63, 37, 112, 49,
    37, 123, 49, 125, 37, 61, 37, 116, 52, 37, 101, 37, 112, 49, 37, 123, 51, 125, 37, 61,
    37, 116, 54, 37, 101, 37, 112, 49, 37, 123, 52, 125, 37, 61, 37, 116, 49, 37, 101, 37,
    112, 49, 37, 123, 54, 125, 37, 61, 37, 116, 51, 37, 101, 37, 112, 49, 37, 100, 37, 59,
    109, 0, 27, 91, 52, 37, 63, 37, 112, 49, 37, 123, 49, 125, 37, 61, 37, 116, 52, 37,
    101, 37, 112, 49, 37, 123, 51, 125, 37, 61, 37, 116, 54, 37, 101, 37, 112, 49, 37, 123,
    52, 125, 37, 61, 37, 116, 49, 37, 101, 37, 112, 49, 37, 123, 54, 125, 37, 61, 37, 116,
    51, 37, 101, 37, 112, 49, 37, 100, 37, 59, 109, 0, 27, 91, 77, 0, 27, 91, 51, 37,
    112, 49, 37, 100, 109, 0, 27, 91, 52, 37, 112, 49, 37, 100, 109, 0, 27, 108, 0, 27,
    109, 0, 2, 0, 1, 0, 57, 0, 117, 0, -88, 2, 1, 0, -1, -1, -1, -1, 0, 0,
    7, 0, 14, 0, 21, 0, 28, 0, 35, 0, 42, 0, 49, 0, 56, 0, 63, 0, 70, 0,
    77, 0, 84, 0, 91, 0, 98, 0, 105, 0, 112, 0, 119, 0, 126, 0, -123, 0, -116, 0,
    -109, 0, -102, 0, -95, 0, -88, 0, -81, 0, -74, 0, -67, 0, -60, 0, -53, 0, -46, 0,
    -39, 0, -32, 0, -25, 0, -18, 0, -11, 0, -4, 0, 3, 1, 10, 1, 17, 1, 24, 1,
    31, 1, 38, 1, 45, 1, 52, 1, 59, 1, 66, 1, 73, 1, 80, 1, 87, 1, 94, 1,
    101, 1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 3, 0, 6, 0, 9, 0, 12, 0,
    17, 0, 22, 0, 27, 0, 32, 0, 37, 0, 41, 0, 46, 0, 51, 0, 56, 0, 61, 0,
    66, 0, 72, 0, 78, 0, 84, 0, 90, 0, 96, 0, 102, 0, 108, 0, 114, 0, 120, 0,
    126, 0, -125, 0, -120, 0, -115, 0, -110, 0, -105, 0, -99, 0, -93, 0, -87, 0, -81, 0,
    -75, 0, -69, 0, -63, 0, -57, 0, -51, 0, -45, 0, -39, 0, -33, 0, -27, 0, -21, 0,
    -15, 0, -9, 0, -3, 0, 3, 1, 9, 1, 15, 1, 19, 1, 24, 1, 29, 1, 34, 1,
    39, 1, 44, 1, 48, 1, 52, 1, 56, 1, 27, 91, 51, 59, 51, 126, 0, 27, 91, 51,
    59, 52, 126, 0, 27, 91, 51, 59, 53, 126, 0, 27, 91, 51, 59, 54, 126, 0, 27, 91,
    51, 59, 55, 126, 0, 27, 91, 49, 59, 50, 66, 0, 27, 91, 49, 59, 51, 66, 0, 27,
    91, 49, 59, 52, 66, 0, 27, 91, 49, 59, 53, 66, 0, 27, 91, 49, 59, 54, 66, 0,
    27, 91, 49, 59, 55, 66, 0, 27, 91, 49, 59, 51, 70, 0, 27, 91, 49, 59, 52, 70,
    0, 27, 91, 49, 59, 53, 70, 0, 27, 91, 49, 59, 54, 70, 0, 27, 91, 49, 59, 55,
    70, 0, 27, 91, 49, 59, 51, 72, 0, 27, 91, 49, 59, 52, 72, 0, 27, 91, 49, 59,
    53, 72, 0, 27, 91, 49, 59, 54, 72, 0, 27, 91, 49, 59, 55, 72, 0, 27, 91, 50,
    59, 51, 126, 0, 27, 91, 50, 59, 52, 126, 0, 27, 91, 50, 59, 53, 126, 0, 27, 91,
    50, 59, 54, 126, 0, 27, 91, 50, 59, 55, 126, 0, 27, 91, 49, 59, 51, 68, 0, 27,
    91, 49, 59, 52, 68, 0, 27, 91, 49, 59, 53, 68, 0, 27, 91, 49, 59, 54, 68, 0,
    27, 91, 49, 59, 55, 68, 0, 27, 91, 54, 59, 51, 126, 0, 27, 91, 54, 59, 52, 126,
    0, 27, 91, 54, 59, 53, 126, 0, 27, 91, 54, 59, 54, 126, 0, 27, 91, 54, 59, 55,
    126, 0, 27, 91, 53, 59, 51, 126, 0, 27, 91, 53, 59, 52, 126, 0, 27, 91, 53, 59,
    53, 126, 0, 27, 91, 53, 59, 54, 126, 0, 27, 91, 53, 59, 55, 126, 0, 27, 91, 49,
    59, 51, 67, 0, 27, 91, 49, 59, 52, 67, 0, 27, 91, 49, 59, 53, 67, 0, 27, 91,
    49, 59, 54, 67, 0, 27, 91, 49, 59, 55, 67, 0, 27, 91, 49, 59, 50, 65, 0, 27,
    91, 49, 59, 51, 65, 0, 27, 91, 49, 59, 52, 65, 0, 27, 91, 49, 59, 53, 65, 0,
    27, 91, 49, 59, 54, 65, 0, 27, 91, 49, 59, 55, 65, 0, 65, 88, 0, 88, 84, 0,
    85, 56, 0, 88, 77, 0, 107, 68, 67, 51, 0, 107, 68, 67, 52, 0, 107, 68, 67, 53,
    0, 107, 68, 67, 54, 0, 107, 68, 67, 55, 0, 107, 68, 78, 0, 107, 68, 78, 51, 0,
    107, 68, 78, 52, 0, 107, 68, 78, 53, 0, 107, 68, 78, 54, 0, 107, 68, 78, 55, 0,
    107, 69, 78, 68, 51, 0, 107, 69, 78, 68, 52, 0, 107, 69, 78, 68, 53, 0, 107, 69,
    78, 68, 54, 0, 107, 69, 78, 68, 55, 0, 107, 72, 79, 77, 51, 0, 107, 72, 79, 77,
    52, 0, 107, 72, 79, 77, 53, 0, 107, 72, 79, 77, 54, 0, 107, 72, 79, 77, 55, 0,
    107, 73, 67, 51, 0, 107, 73, 67, 52, 0, 107, 73, 67, 53, 0, 107, 73, 67, 54, 0,
    107, 73, 67, 55, 0, 107, 76, 70, 84, 51, 0, 107, 76, 70, 84, 52, 0, 107, 76, 70,
    84, 53, 0, 107, 76, 70, 84, 54, 0, 107, 76, 70, 84, 55, 0, 107, 78, 88, 84, 51,
    0, 107, 78, 88, 84, 52, 0, 107, 78, 88, 84, 53, 0, 107, 78, 88, 84, 54, 0, 107,
    78, 88, 84, 55, 0, 107, 80, 82, 86, 51, 0, 107, 80, 82, 86, 52, 0, 107, 80, 82,
    86, 53, 0, 107, 80, 82, 86, 54, 0, 107, 80, 82, 86, 55, 0, 107, 82, 73, 84, 51,
    0, 107, 82, 73, 84, 52, 0, 107, 82, 73, 84, 53, 0, 107, 82, 73, 84, 54, 0, 107,
    82, 73, 84, 55, 0, 107, 85, 80, 0, 107, 85, 80, 51, 0, 107, 85, 80, 52, 0, 107,
    85, 80, 53, 0, 107, 85, 80, 54, 0, 107, 85, 80, 55, 0, 107, 97, 50, 0, 107, 98,
    49, 0, 107, 98, 51, 0, 107, 99, 50, 0
};

static void setup(void);

int main(void) {
    setup();

    int e;
    unibi_term *dt = unibi_dummy();
    e = errno;
    ok(dt != NULL, "dummy constructed");
    if (!dt) {
        bail_out(strerror(e));
    }
    unibi_term *ut = unibi_from_mem(terminfo, sizeof terminfo);
    e = errno;
    ok(ut != NULL, "terminfo loaded");
    if (!ut) {
        bail_out(strerror(e));
    }

    diag("terminal name");
    ok(strcmp(unibi_get_name(ut), "xterm terminal emulator (X Window System)") == 0, "terminal name = \"xterm terminal emulator (X Window System)\"");
    unibi_set_name(dt, "xterm terminal emulator (X Window System)");
    {
        static const char *def_aliases[] = {"xterm", NULL};
        const char **aliases = unibi_get_aliases(ut);
        ok(strcmp(aliases[0], def_aliases[0]) == 0, "terminal alias #0 = \"xterm\"");
        ok(aliases[1] == NULL, "terminal alias #1 = null");
        unibi_set_aliases(dt, def_aliases);
    }

    diag("boolean capabilities");
    ok(unibi_get_bool(ut, unibi_auto_left_margin) == 0, "auto_left_margin = false");
    ok(unibi_get_bool(ut, unibi_auto_right_margin) == 1, "auto_right_margin = true");
    unibi_set_bool(dt, unibi_auto_right_margin, 1);
    ok(unibi_get_bool(ut, unibi_no_esc_ctlc) == 0, "no_esc_ctlc = false");
    ok(unibi_get_bool(ut, unibi_ceol_standout_glitch) == 0, "ceol_standout_glitch = false");
    ok(unibi_get_bool(ut, unibi_eat_newline_glitch) == 1, "eat_newline_glitch = true");
    unibi_set_bool(dt, unibi_eat_newline_glitch, 1);
    ok(unibi_get_bool(ut, unibi_erase_overstrike) == 0, "erase_overstrike = false");
    ok(unibi_get_bool(ut, unibi_generic_type) == 0, "generic_type = false");
    ok(unibi_get_bool(ut, unibi_hard_copy) == 0, "hard_copy = false");
    ok(unibi_get_bool(ut, unibi_has_meta_key) == 1, "has_meta_key = true");
    unibi_set_bool(dt, unibi_has_meta_key, 1);
    ok(unibi_get_bool(ut, unibi_has_status_line) == 0, "has_status_line = false");
    ok(unibi_get_bool(ut, unibi_insert_null_glitch) == 0, "insert_null_glitch = false");
    ok(unibi_get_bool(ut, unibi_memory_above) == 0, "memory_above = false");
    ok(unibi_get_bool(ut, unibi_memory_below) == 0, "memory_below = false");
    ok(unibi_get_bool(ut, unibi_move_insert_mode) == 1, "move_insert_mode = true");
    unibi_set_bool(dt, unibi_move_insert_mode, 1);
    ok(unibi_get_bool(ut, unibi_move_standout_mode) == 1, "move_standout_mode = true");
    unibi_set_bool(dt, unibi_move_standout_mode, 1);
    ok(unibi_get_bool(ut, unibi_over_strike) == 0, "over_strike = false");
    ok(unibi_get_bool(ut, unibi_status_line_esc_ok) == 0, "status_line_esc_ok = false");
    ok(unibi_get_bool(ut, unibi_dest_tabs_magic_smso) == 0, "dest_tabs_magic_smso = false");
    ok(unibi_get_bool(ut, unibi_tilde_glitch) == 0, "tilde_glitch = false");
    ok(unibi_get_bool(ut, unibi_transparent_underline) == 0, "transparent_underline = false");
    ok(unibi_get_bool(ut, unibi_xon_xoff) == 0, "xon_xoff = false");
    ok(unibi_get_bool(ut, unibi_needs_xon_xoff) == 0, "needs_xon_xoff = false");
    ok(unibi_get_bool(ut, unibi_prtr_silent) == 1, "prtr_silent = true");
    unibi_set_bool(dt, unibi_prtr_silent, 1);
    ok(unibi_get_bool(ut, unibi_hard_cursor) == 0, "hard_cursor = false");
    ok(unibi_get_bool(ut, unibi_non_rev_rmcup) == 0, "non_rev_rmcup = false");
    ok(unibi_get_bool(ut, unibi_no_pad_char) == 1, "no_pad_char = true");
    unibi_set_bool(dt, unibi_no_pad_char, 1);
    ok(unibi_get_bool(ut, unibi_non_dest_scroll_region) == 0, "non_dest_scroll_region = false");
    ok(unibi_get_bool(ut, unibi_can_change) == 0, "can_change = false");
    ok(unibi_get_bool(ut, unibi_back_color_erase) == 1, "back_color_erase = true");
    unibi_set_bool(dt, unibi_back_color_erase, 1);
    ok(unibi_get_bool(ut, unibi_hue_lightness_saturation) == 0, "hue_lightness_saturation = false");
    ok(unibi_get_bool(ut, unibi_col_addr_glitch) == 0, "col_addr_glitch = false");
    ok(unibi_get_bool(ut, unibi_cr_cancels_micro_mode) == 0, "cr_cancels_micro_mode = false");
    ok(unibi_get_bool(ut, unibi_has_print_wheel) == 0, "has_print_wheel = false");
    ok(unibi_get_bool(ut, unibi_row_addr_glitch) == 0, "row_addr_glitch = false");
    ok(unibi_get_bool(ut, unibi_semi_auto_right_margin) == 0, "semi_auto_right_margin = false");
    ok(unibi_get_bool(ut, unibi_cpi_changes_res) == 0, "cpi_changes_res = false");
    ok(unibi_get_bool(ut, unibi_lpi_changes_res) == 0, "lpi_changes_res = false");
    ok(unibi_get_bool(ut, unibi_backspaces_with_bs) == 1, "backspaces_with_bs = true");
    unibi_set_bool(dt, unibi_backspaces_with_bs, 1);
    ok(unibi_get_bool(ut, unibi_crt_no_scrolling) == 0, "crt_no_scrolling = false");
    ok(unibi_get_bool(ut, unibi_no_correctly_working_cr) == 0, "no_correctly_working_cr = false");
    ok(unibi_get_bool(ut, unibi_gnu_has_meta_key) == 0, "gnu_has_meta_key = false");
    ok(unibi_get_bool(ut, unibi_linefeed_is_newline) == 0, "linefeed_is_newline = false");
    ok(unibi_get_bool(ut, unibi_has_hardware_tabs) == 0, "has_hardware_tabs = false");
    ok(unibi_get_bool(ut, unibi_return_does_clr_eol) == 0, "return_does_clr_eol = false");

    diag("numeric capabilities");
    ok(unibi_get_num(ut, unibi_columns) == 80, "columns = 80");
    unibi_set_num(dt, unibi_columns, 80);
    ok(unibi_get_num(ut, unibi_init_tabs) == 8, "init_tabs = 8");
    unibi_set_num(dt, unibi_init_tabs, 8);
    ok(unibi_get_num(ut, unibi_lines) == 24, "lines = 24");
    unibi_set_num(dt, unibi_lines, 24);
    ok(unibi_get_num(ut, unibi_lines_of_memory) == -1, "lines_of_memory = -1");
    ok(unibi_get_num(ut, unibi_magic_cookie_glitch) == -1, "magic_cookie_glitch = -1");
    ok(unibi_get_num(ut, unibi_padding_baud_rate) == -1, "padding_baud_rate = -1");
    ok(unibi_get_num(ut, unibi_virtual_terminal) == -1, "virtual_terminal = -1");
    ok(unibi_get_num(ut, unibi_width_status_line) == -1, "width_status_line = -1");
    ok(unibi_get_num(ut, unibi_num_labels) == -1, "num_labels = -1");
    ok(unibi_get_num(ut, unibi_label_height) == -1, "label_height = -1");
    ok(unibi_get_num(ut, unibi_label_width) == -1, "label_width = -1");
    ok(unibi_get_num(ut, unibi_max_attributes) == -1, "max_attributes = -1");
    ok(unibi_get_num(ut, unibi_maximum_windows) == -1, "maximum_windows = -1");
    ok(unibi_get_num(ut, unibi_max_colors) == 8, "max_colors = 8");
    unibi_set_num(dt, unibi_max_colors, 8);
    ok(unibi_get_num(ut, unibi_max_pairs) == 64, "max_pairs = 64");
    unibi_set_num(dt, unibi_max_pairs, 64);
    ok(unibi_get_num(ut, unibi_no_color_video) == -1, "no_color_video = -1");
    ok(unibi_get_num(ut, unibi_buffer_capacity) == -1, "buffer_capacity = -1");
    ok(unibi_get_num(ut, unibi_dot_vert_spacing) == -1, "dot_vert_spacing = -1");
    ok(unibi_get_num(ut, unibi_dot_horz_spacing) == -1, "dot_horz_spacing = -1");
    ok(unibi_get_num(ut, unibi_max_micro_address) == -1, "max_micro_address = -1");
    ok(unibi_get_num(ut, unibi_max_micro_jump) == -1, "max_micro_jump = -1");
    ok(unibi_get_num(ut, unibi_micro_col_size) == -1, "micro_col_size = -1");
    ok(unibi_get_num(ut, unibi_micro_line_size) == -1, "micro_line_size = -1");
    ok(unibi_get_num(ut, unibi_number_of_pins) == -1, "number_of_pins = -1");
    ok(unibi_get_num(ut, unibi_output_res_char) == -1, "output_res_char = -1");
    ok(unibi_get_num(ut, unibi_output_res_line) == -1, "output_res_line = -1");
    ok(unibi_get_num(ut, unibi_output_res_horz_inch) == -1, "output_res_horz_inch = -1");
    ok(unibi_get_num(ut, unibi_output_res_vert_inch) == -1, "output_res_vert_inch = -1");
    ok(unibi_get_num(ut, unibi_print_rate) == -1, "print_rate = -1");
    ok(unibi_get_num(ut, unibi_wide_char_size) == -1, "wide_char_size = -1");
    ok(unibi_get_num(ut, unibi_buttons) == -1, "buttons = -1");
    ok(unibi_get_num(ut, unibi_bit_image_entwining) == -1, "bit_image_entwining = -1");
    ok(unibi_get_num(ut, unibi_bit_image_type) == -1, "bit_image_type = -1");
    ok(unibi_get_num(ut, unibi_magic_cookie_glitch_ul) == -1, "magic_cookie_glitch_ul = -1");
    ok(unibi_get_num(ut, unibi_carriage_return_delay) == -1, "carriage_return_delay = -1");
    ok(unibi_get_num(ut, unibi_new_line_delay) == -1, "new_line_delay = -1");
    ok(unibi_get_num(ut, unibi_backspace_delay) == -1, "backspace_delay = -1");
    ok(unibi_get_num(ut, unibi_horizontal_tab_delay) == -1, "horizontal_tab_delay = -1");
    ok(unibi_get_num(ut, unibi_number_of_function_keys) == -1, "number_of_function_keys = -1");

    diag("string capabilities");
    ok(strcmp(unibi_get_str(ut, unibi_back_tab), "\033[Z") == 0, "back_tab = \"\\033[Z\"");
    unibi_set_str(dt, unibi_back_tab, "\033[Z");
    ok(strcmp(unibi_get_str(ut, unibi_bell), "\007") == 0, "bell = \"\\007\"");
    unibi_set_str(dt, unibi_bell, "\007");
    ok(strcmp(unibi_get_str(ut, unibi_carriage_return), "\015") == 0, "carriage_return = \"\\015\"");
    unibi_set_str(dt, unibi_carriage_return, "\015");
    ok(strcmp(unibi_get_str(ut, unibi_change_scroll_region), "\033[%i%p1%d;%p2%dr") == 0, "change_scroll_region = \"\\033[%i%p1%d;%p2%dr\"");
    unibi_set_str(dt, unibi_change_scroll_region, "\033[%i%p1%d;%p2%dr");
    ok(strcmp(unibi_get_str(ut, unibi_clear_all_tabs), "\033[3g") == 0, "clear_all_tabs = \"\\033[3g\"");
    unibi_set_str(dt, unibi_clear_all_tabs, "\033[3g");
    ok(strcmp(unibi_get_str(ut, unibi_clear_screen), "\033[H\033[2J") == 0, "clear_screen = \"\\033[H\\033[2J\"");
    unibi_set_str(dt, unibi_clear_screen, "\033[H\033[2J");
    ok(strcmp(unibi_get_str(ut, unibi_clr_eol), "\033[K") == 0, "clr_eol = \"\\033[K\"");
    unibi_set_str(dt, unibi_clr_eol, "\033[K");
    ok(strcmp(unibi_get_str(ut, unibi_clr_eos), "\033[J") == 0, "clr_eos = \"\\033[J\"");
    unibi_set_str(dt, unibi_clr_eos, "\033[J");
    ok(strcmp(unibi_get_str(ut, unibi_column_address), "\033[%i%p1%dG") == 0, "column_address = \"\\033[%i%p1%dG\"");
    unibi_set_str(dt, unibi_column_address, "\033[%i%p1%dG");
    ok(unibi_get_str(ut, unibi_command_character) == NULL, "command_character = null");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_address), "\033[%i%p1%d;%p2%dH") == 0, "cursor_address = \"\\033[%i%p1%d;%p2%dH\"");
    unibi_set_str(dt, unibi_cursor_address, "\033[%i%p1%d;%p2%dH");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_down), "\012") == 0, "cursor_down = \"\\012\"");
    unibi_set_str(dt, unibi_cursor_down, "\012");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_home), "\033[H") == 0, "cursor_home = \"\\033[H\"");
    unibi_set_str(dt, unibi_cursor_home, "\033[H");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_invisible), "\033[?25l") == 0, "cursor_invisible = \"\\033[?25l\"");
    unibi_set_str(dt, unibi_cursor_invisible, "\033[?25l");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_left), "\010") == 0, "cursor_left = \"\\010\"");
    unibi_set_str(dt, unibi_cursor_left, "\010");
    ok(unibi_get_str(ut, unibi_cursor_mem_address) == NULL, "cursor_mem_address = null");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_normal), "\033[?12l\033[?25h") == 0, "cursor_normal = \"\\033[?12l\\033[?25h\"");
    unibi_set_str(dt, unibi_cursor_normal, "\033[?12l\033[?25h");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_right), "\033[C") == 0, "cursor_right = \"\\033[C\"");
    unibi_set_str(dt, unibi_cursor_right, "\033[C");
    ok(unibi_get_str(ut, unibi_cursor_to_ll) == NULL, "cursor_to_ll = null");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_up), "\033[A") == 0, "cursor_up = \"\\033[A\"");
    unibi_set_str(dt, unibi_cursor_up, "\033[A");
    ok(strcmp(unibi_get_str(ut, unibi_cursor_visible), "\033[?12;25h") == 0, "cursor_visible = \"\\033[?12;25h\"");
    unibi_set_str(dt, unibi_cursor_visible, "\033[?12;25h");
    ok(strcmp(unibi_get_str(ut, unibi_delete_character), "\033[P") == 0, "delete_character = \"\\033[P\"");
    unibi_set_str(dt, unibi_delete_character, "\033[P");
    ok(strcmp(unibi_get_str(ut, unibi_delete_line), "\033[M") == 0, "delete_line = \"\\033[M\"");
    unibi_set_str(dt, unibi_delete_line, "\033[M");
    ok(unibi_get_str(ut, unibi_dis_status_line) == NULL, "dis_status_line = null");
    ok(unibi_get_str(ut, unibi_down_half_line) == NULL, "down_half_line = null");
    ok(strcmp(unibi_get_str(ut, unibi_enter_alt_charset_mode), "\033(0") == 0, "enter_alt_charset_mode = \"\\033(0\"");
    unibi_set_str(dt, unibi_enter_alt_charset_mode, "\033(0");
    ok(strcmp(unibi_get_str(ut, unibi_enter_blink_mode), "\033[5m") == 0, "enter_blink_mode = \"\\033[5m\"");
    unibi_set_str(dt, unibi_enter_blink_mode, "\033[5m");
    ok(strcmp(unibi_get_str(ut, unibi_enter_bold_mode), "\033[1m") == 0, "enter_bold_mode = \"\\033[1m\"");
    unibi_set_str(dt, unibi_enter_bold_mode, "\033[1m");
    ok(strcmp(unibi_get_str(ut, unibi_enter_ca_mode), "\033[?1049h") == 0, "enter_ca_mode = \"\\033[?1049h\"");
    unibi_set_str(dt, unibi_enter_ca_mode, "\033[?1049h");
    ok(unibi_get_str(ut, unibi_enter_delete_mode) == NULL, "enter_delete_mode = null");
    ok(unibi_get_str(ut, unibi_enter_dim_mode) == NULL, "enter_dim_mode = null");
    ok(strcmp(unibi_get_str(ut, unibi_enter_insert_mode), "\033[4h") == 0, "enter_insert_mode = \"\\033[4h\"");
    unibi_set_str(dt, unibi_enter_insert_mode, "\033[4h");
    ok(strcmp(unibi_get_str(ut, unibi_enter_secure_mode), "\033[8m") == 0, "enter_secure_mode = \"\\033[8m\"");
    unibi_set_str(dt, unibi_enter_secure_mode, "\033[8m");
    ok(unibi_get_str(ut, unibi_enter_protected_mode) == NULL, "enter_protected_mode = null");
    ok(strcmp(unibi_get_str(ut, unibi_enter_reverse_mode), "\033[7m") == 0, "enter_reverse_mode = \"\\033[7m\"");
    unibi_set_str(dt, unibi_enter_reverse_mode, "\033[7m");
    ok(strcmp(unibi_get_str(ut, unibi_enter_standout_mode), "\033[7m") == 0, "enter_standout_mode = \"\\033[7m\"");
    unibi_set_str(dt, unibi_enter_standout_mode, "\033[7m");
    ok(strcmp(unibi_get_str(ut, unibi_enter_underline_mode), "\033[4m") == 0, "enter_underline_mode = \"\\033[4m\"");
    unibi_set_str(dt, unibi_enter_underline_mode, "\033[4m");
    ok(strcmp(unibi_get_str(ut, unibi_erase_chars), "\033[%p1%dX") == 0, "erase_chars = \"\\033[%p1%dX\"");
    unibi_set_str(dt, unibi_erase_chars, "\033[%p1%dX");
    ok(strcmp(unibi_get_str(ut, unibi_exit_alt_charset_mode), "\033(B") == 0, "exit_alt_charset_mode = \"\\033(B\"");
    unibi_set_str(dt, unibi_exit_alt_charset_mode, "\033(B");
    ok(strcmp(unibi_get_str(ut, unibi_exit_attribute_mode), "\033(B\033[m") == 0, "exit_attribute_mode = \"\\033(B\\033[m\"");
    unibi_set_str(dt, unibi_exit_attribute_mode, "\033(B\033[m");
    ok(strcmp(unibi_get_str(ut, unibi_exit_ca_mode), "\033[?1049l") == 0, "exit_ca_mode = \"\\033[?1049l\"");
    unibi_set_str(dt, unibi_exit_ca_mode, "\033[?1049l");
    ok(unibi_get_str(ut, unibi_exit_delete_mode) == NULL, "exit_delete_mode = null");
    ok(strcmp(unibi_get_str(ut, unibi_exit_insert_mode), "\033[4l") == 0, "exit_insert_mode = \"\\033[4l\"");
    unibi_set_str(dt, unibi_exit_insert_mode, "\033[4l");
    ok(strcmp(unibi_get_str(ut, unibi_exit_standout_mode), "\033[27m") == 0, "exit_standout_mode = \"\\033[27m\"");
    unibi_set_str(dt, unibi_exit_standout_mode, "\033[27m");
    ok(strcmp(unibi_get_str(ut, unibi_exit_underline_mode), "\033[24m") == 0, "exit_underline_mode = \"\\033[24m\"");
    unibi_set_str(dt, unibi_exit_underline_mode, "\033[24m");
    ok(strcmp(unibi_get_str(ut, unibi_flash_screen), "\033[?5h$<100/>\033[?5l") == 0, "flash_screen = \"\\033[?5h$<100/>\\033[?5l\"");
    unibi_set_str(dt, unibi_flash_screen, "\033[?5h$<100/>\033[?5l");
    ok(unibi_get_str(ut, unibi_form_feed) == NULL, "form_feed = null");
    ok(unibi_get_str(ut, unibi_from_status_line) == NULL, "from_status_line = null");
    ok(unibi_get_str(ut, unibi_init_1string) == NULL, "init_1string = null");
    ok(strcmp(unibi_get_str(ut, unibi_init_2string), "\033[!p\033[?3;4l\033[4l\033>") == 0, "init_2string = \"\\033[!p\\033[?3;4l\\033[4l\\033>\"");
    unibi_set_str(dt, unibi_init_2string, "\033[!p\033[?3;4l\033[4l\033>");
    ok(unibi_get_str(ut, unibi_init_3string) == NULL, "init_3string = null");
    ok(unibi_get_str(ut, unibi_init_file) == NULL, "init_file = null");
    ok(unibi_get_str(ut, unibi_insert_character) == NULL, "insert_character = null");
    ok(strcmp(unibi_get_str(ut, unibi_insert_line), "\033[L") == 0, "insert_line = \"\\033[L\"");
    unibi_set_str(dt, unibi_insert_line, "\033[L");
    ok(unibi_get_str(ut, unibi_insert_padding) == NULL, "insert_padding = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_backspace), "\010") == 0, "key_backspace = \"\\010\"");
    unibi_set_str(dt, unibi_key_backspace, "\010");
    ok(unibi_get_str(ut, unibi_key_catab) == NULL, "key_catab = null");
    ok(unibi_get_str(ut, unibi_key_clear) == NULL, "key_clear = null");
    ok(unibi_get_str(ut, unibi_key_ctab) == NULL, "key_ctab = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_dc), "\033[3~") == 0, "key_dc = \"\\033[3~\"");
    unibi_set_str(dt, unibi_key_dc, "\033[3~");
    ok(unibi_get_str(ut, unibi_key_dl) == NULL, "key_dl = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_down), "\033OB") == 0, "key_down = \"\\033OB\"");
    unibi_set_str(dt, unibi_key_down, "\033OB");
    ok(unibi_get_str(ut, unibi_key_eic) == NULL, "key_eic = null");
    ok(unibi_get_str(ut, unibi_key_eol) == NULL, "key_eol = null");
    ok(unibi_get_str(ut, unibi_key_eos) == NULL, "key_eos = null");
    ok(unibi_get_str(ut, unibi_key_f0) == NULL, "key_f0 = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_f1), "\033OP") == 0, "key_f1 = \"\\033OP\"");
    unibi_set_str(dt, unibi_key_f1, "\033OP");
    ok(strcmp(unibi_get_str(ut, unibi_key_f10), "\033[21~") == 0, "key_f10 = \"\\033[21~\"");
    unibi_set_str(dt, unibi_key_f10, "\033[21~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f2), "\033OQ") == 0, "key_f2 = \"\\033OQ\"");
    unibi_set_str(dt, unibi_key_f2, "\033OQ");
    ok(strcmp(unibi_get_str(ut, unibi_key_f3), "\033OR") == 0, "key_f3 = \"\\033OR\"");
    unibi_set_str(dt, unibi_key_f3, "\033OR");
    ok(strcmp(unibi_get_str(ut, unibi_key_f4), "\033OS") == 0, "key_f4 = \"\\033OS\"");
    unibi_set_str(dt, unibi_key_f4, "\033OS");
    ok(strcmp(unibi_get_str(ut, unibi_key_f5), "\033[15~") == 0, "key_f5 = \"\\033[15~\"");
    unibi_set_str(dt, unibi_key_f5, "\033[15~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f6), "\033[17~") == 0, "key_f6 = \"\\033[17~\"");
    unibi_set_str(dt, unibi_key_f6, "\033[17~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f7), "\033[18~") == 0, "key_f7 = \"\\033[18~\"");
    unibi_set_str(dt, unibi_key_f7, "\033[18~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f8), "\033[19~") == 0, "key_f8 = \"\\033[19~\"");
    unibi_set_str(dt, unibi_key_f8, "\033[19~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f9), "\033[20~") == 0, "key_f9 = \"\\033[20~\"");
    unibi_set_str(dt, unibi_key_f9, "\033[20~");
    ok(strcmp(unibi_get_str(ut, unibi_key_home), "\033OH") == 0, "key_home = \"\\033OH\"");
    unibi_set_str(dt, unibi_key_home, "\033OH");
    ok(strcmp(unibi_get_str(ut, unibi_key_ic), "\033[2~") == 0, "key_ic = \"\\033[2~\"");
    unibi_set_str(dt, unibi_key_ic, "\033[2~");
    ok(unibi_get_str(ut, unibi_key_il) == NULL, "key_il = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_left), "\033OD") == 0, "key_left = \"\\033OD\"");
    unibi_set_str(dt, unibi_key_left, "\033OD");
    ok(unibi_get_str(ut, unibi_key_ll) == NULL, "key_ll = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_npage), "\033[6~") == 0, "key_npage = \"\\033[6~\"");
    unibi_set_str(dt, unibi_key_npage, "\033[6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_ppage), "\033[5~") == 0, "key_ppage = \"\\033[5~\"");
    unibi_set_str(dt, unibi_key_ppage, "\033[5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_right), "\033OC") == 0, "key_right = \"\\033OC\"");
    unibi_set_str(dt, unibi_key_right, "\033OC");
    ok(strcmp(unibi_get_str(ut, unibi_key_sf), "\033[1;2B") == 0, "key_sf = \"\\033[1;2B\"");
    unibi_set_str(dt, unibi_key_sf, "\033[1;2B");
    ok(strcmp(unibi_get_str(ut, unibi_key_sr), "\033[1;2A") == 0, "key_sr = \"\\033[1;2A\"");
    unibi_set_str(dt, unibi_key_sr, "\033[1;2A");
    ok(unibi_get_str(ut, unibi_key_stab) == NULL, "key_stab = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_up), "\033OA") == 0, "key_up = \"\\033OA\"");
    unibi_set_str(dt, unibi_key_up, "\033OA");
    ok(strcmp(unibi_get_str(ut, unibi_keypad_local), "\033[?1l\033>") == 0, "keypad_local = \"\\033[?1l\\033>\"");
    unibi_set_str(dt, unibi_keypad_local, "\033[?1l\033>");
    ok(strcmp(unibi_get_str(ut, unibi_keypad_xmit), "\033[?1h\033=") == 0, "keypad_xmit = \"\\033[?1h\\033=\"");
    unibi_set_str(dt, unibi_keypad_xmit, "\033[?1h\033=");
    ok(unibi_get_str(ut, unibi_lab_f0) == NULL, "lab_f0 = null");
    ok(unibi_get_str(ut, unibi_lab_f1) == NULL, "lab_f1 = null");
    ok(unibi_get_str(ut, unibi_lab_f10) == NULL, "lab_f10 = null");
    ok(unibi_get_str(ut, unibi_lab_f2) == NULL, "lab_f2 = null");
    ok(unibi_get_str(ut, unibi_lab_f3) == NULL, "lab_f3 = null");
    ok(unibi_get_str(ut, unibi_lab_f4) == NULL, "lab_f4 = null");
    ok(unibi_get_str(ut, unibi_lab_f5) == NULL, "lab_f5 = null");
    ok(unibi_get_str(ut, unibi_lab_f6) == NULL, "lab_f6 = null");
    ok(unibi_get_str(ut, unibi_lab_f7) == NULL, "lab_f7 = null");
    ok(unibi_get_str(ut, unibi_lab_f8) == NULL, "lab_f8 = null");
    ok(unibi_get_str(ut, unibi_lab_f9) == NULL, "lab_f9 = null");
    ok(strcmp(unibi_get_str(ut, unibi_meta_off), "\033[?1034l") == 0, "meta_off = \"\\033[?1034l\"");
    unibi_set_str(dt, unibi_meta_off, "\033[?1034l");
    ok(strcmp(unibi_get_str(ut, unibi_meta_on), "\033[?1034h") == 0, "meta_on = \"\\033[?1034h\"");
    unibi_set_str(dt, unibi_meta_on, "\033[?1034h");
    ok(unibi_get_str(ut, unibi_newline) == NULL, "newline = null");
    ok(unibi_get_str(ut, unibi_pad_char) == NULL, "pad_char = null");
    ok(strcmp(unibi_get_str(ut, unibi_parm_dch), "\033[%p1%dP") == 0, "parm_dch = \"\\033[%p1%dP\"");
    unibi_set_str(dt, unibi_parm_dch, "\033[%p1%dP");
    ok(strcmp(unibi_get_str(ut, unibi_parm_delete_line), "\033[%p1%dM") == 0, "parm_delete_line = \"\\033[%p1%dM\"");
    unibi_set_str(dt, unibi_parm_delete_line, "\033[%p1%dM");
    ok(strcmp(unibi_get_str(ut, unibi_parm_down_cursor), "\033[%p1%dB") == 0, "parm_down_cursor = \"\\033[%p1%dB\"");
    unibi_set_str(dt, unibi_parm_down_cursor, "\033[%p1%dB");
    ok(strcmp(unibi_get_str(ut, unibi_parm_ich), "\033[%p1%d@") == 0, "parm_ich = \"\\033[%p1%d@\"");
    unibi_set_str(dt, unibi_parm_ich, "\033[%p1%d@");
    ok(strcmp(unibi_get_str(ut, unibi_parm_index), "\033[%p1%dS") == 0, "parm_index = \"\\033[%p1%dS\"");
    unibi_set_str(dt, unibi_parm_index, "\033[%p1%dS");
    ok(strcmp(unibi_get_str(ut, unibi_parm_insert_line), "\033[%p1%dL") == 0, "parm_insert_line = \"\\033[%p1%dL\"");
    unibi_set_str(dt, unibi_parm_insert_line, "\033[%p1%dL");
    ok(strcmp(unibi_get_str(ut, unibi_parm_left_cursor), "\033[%p1%dD") == 0, "parm_left_cursor = \"\\033[%p1%dD\"");
    unibi_set_str(dt, unibi_parm_left_cursor, "\033[%p1%dD");
    ok(strcmp(unibi_get_str(ut, unibi_parm_right_cursor), "\033[%p1%dC") == 0, "parm_right_cursor = \"\\033[%p1%dC\"");
    unibi_set_str(dt, unibi_parm_right_cursor, "\033[%p1%dC");
    ok(strcmp(unibi_get_str(ut, unibi_parm_rindex), "\033[%p1%dT") == 0, "parm_rindex = \"\\033[%p1%dT\"");
    unibi_set_str(dt, unibi_parm_rindex, "\033[%p1%dT");
    ok(strcmp(unibi_get_str(ut, unibi_parm_up_cursor), "\033[%p1%dA") == 0, "parm_up_cursor = \"\\033[%p1%dA\"");
    unibi_set_str(dt, unibi_parm_up_cursor, "\033[%p1%dA");
    ok(unibi_get_str(ut, unibi_pkey_key) == NULL, "pkey_key = null");
    ok(unibi_get_str(ut, unibi_pkey_local) == NULL, "pkey_local = null");
    ok(unibi_get_str(ut, unibi_pkey_xmit) == NULL, "pkey_xmit = null");
    ok(strcmp(unibi_get_str(ut, unibi_print_screen), "\033[i") == 0, "print_screen = \"\\033[i\"");
    unibi_set_str(dt, unibi_print_screen, "\033[i");
    ok(strcmp(unibi_get_str(ut, unibi_prtr_off), "\033[4i") == 0, "prtr_off = \"\\033[4i\"");
    unibi_set_str(dt, unibi_prtr_off, "\033[4i");
    ok(strcmp(unibi_get_str(ut, unibi_prtr_on), "\033[5i") == 0, "prtr_on = \"\\033[5i\"");
    unibi_set_str(dt, unibi_prtr_on, "\033[5i");
    ok(unibi_get_str(ut, unibi_repeat_char) == NULL, "repeat_char = null");
    ok(strcmp(unibi_get_str(ut, unibi_reset_1string), "\033c") == 0, "reset_1string = \"\\033c\"");
    unibi_set_str(dt, unibi_reset_1string, "\033c");
    ok(strcmp(unibi_get_str(ut, unibi_reset_2string), "\033[!p\033[?3;4l\033[4l\033>") == 0, "reset_2string = \"\\033[!p\\033[?3;4l\\033[4l\\033>\"");
    unibi_set_str(dt, unibi_reset_2string, "\033[!p\033[?3;4l\033[4l\033>");
    ok(unibi_get_str(ut, unibi_reset_3string) == NULL, "reset_3string = null");
    ok(unibi_get_str(ut, unibi_reset_file) == NULL, "reset_file = null");
    ok(strcmp(unibi_get_str(ut, unibi_restore_cursor), "\0338") == 0, "restore_cursor = \"\\0338\"");
    unibi_set_str(dt, unibi_restore_cursor, "\0338");
    ok(strcmp(unibi_get_str(ut, unibi_row_address), "\033[%i%p1%dd") == 0, "row_address = \"\\033[%i%p1%dd\"");
    unibi_set_str(dt, unibi_row_address, "\033[%i%p1%dd");
    ok(strcmp(unibi_get_str(ut, unibi_save_cursor), "\0337") == 0, "save_cursor = \"\\0337\"");
    unibi_set_str(dt, unibi_save_cursor, "\0337");
    ok(strcmp(unibi_get_str(ut, unibi_scroll_forward), "\012") == 0, "scroll_forward = \"\\012\"");
    unibi_set_str(dt, unibi_scroll_forward, "\012");
    ok(strcmp(unibi_get_str(ut, unibi_scroll_reverse), "\033M") == 0, "scroll_reverse = \"\\033M\"");
    unibi_set_str(dt, unibi_scroll_reverse, "\033M");
    ok(strcmp(unibi_get_str(ut, unibi_set_attributes), "%?%p9%t\033(0%e\033(B%;\033[0%?%p6%t;1%;%?%p2%t;4%;%?%p1%p3%|%t;7%;%?%p4%t;5%;%?%p7%t;8%;m") == 0, "set_attributes = \"%?%p9%t\\033(0%e\\033(B%;\\033[0%?%p6%t;1%;%?%p2%t;4%;%?%p1%p3%|%t;7%;%?%p4%t;5%;%?%p7%t;8%;m\"");
    unibi_set_str(dt, unibi_set_attributes, "%?%p9%t\033(0%e\033(B%;\033[0%?%p6%t;1%;%?%p2%t;4%;%?%p1%p3%|%t;7%;%?%p4%t;5%;%?%p7%t;8%;m");
    ok(strcmp(unibi_get_str(ut, unibi_set_tab), "\033H") == 0, "set_tab = \"\\033H\"");
    unibi_set_str(dt, unibi_set_tab, "\033H");
    ok(unibi_get_str(ut, unibi_set_window) == NULL, "set_window = null");
    ok(strcmp(unibi_get_str(ut, unibi_tab), "\011") == 0, "tab = \"\\011\"");
    unibi_set_str(dt, unibi_tab, "\011");
    ok(unibi_get_str(ut, unibi_to_status_line) == NULL, "to_status_line = null");
    ok(unibi_get_str(ut, unibi_underline_char) == NULL, "underline_char = null");
    ok(unibi_get_str(ut, unibi_up_half_line) == NULL, "up_half_line = null");
    ok(unibi_get_str(ut, unibi_init_prog) == NULL, "init_prog = null");
    ok(unibi_get_str(ut, unibi_key_a1) == NULL, "key_a1 = null");
    ok(unibi_get_str(ut, unibi_key_a3) == NULL, "key_a3 = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_b2), "\033OE") == 0, "key_b2 = \"\\033OE\"");
    unibi_set_str(dt, unibi_key_b2, "\033OE");
    ok(unibi_get_str(ut, unibi_key_c1) == NULL, "key_c1 = null");
    ok(unibi_get_str(ut, unibi_key_c3) == NULL, "key_c3 = null");
    ok(unibi_get_str(ut, unibi_prtr_non) == NULL, "prtr_non = null");
    ok(unibi_get_str(ut, unibi_char_padding) == NULL, "char_padding = null");
    ok(strcmp(unibi_get_str(ut, unibi_acs_chars), "``aaffggiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz{{||}}~~") == 0, "acs_chars = \"``aaffggiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz{{||}}~~\"");
    unibi_set_str(dt, unibi_acs_chars, "``aaffggiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz{{||}}~~");
    ok(unibi_get_str(ut, unibi_plab_norm) == NULL, "plab_norm = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_btab), "\033[Z") == 0, "key_btab = \"\\033[Z\"");
    unibi_set_str(dt, unibi_key_btab, "\033[Z");
    ok(unibi_get_str(ut, unibi_enter_xon_mode) == NULL, "enter_xon_mode = null");
    ok(unibi_get_str(ut, unibi_exit_xon_mode) == NULL, "exit_xon_mode = null");
    ok(strcmp(unibi_get_str(ut, unibi_enter_am_mode), "\033[?7h") == 0, "enter_am_mode = \"\\033[?7h\"");
    unibi_set_str(dt, unibi_enter_am_mode, "\033[?7h");
    ok(strcmp(unibi_get_str(ut, unibi_exit_am_mode), "\033[?7l") == 0, "exit_am_mode = \"\\033[?7l\"");
    unibi_set_str(dt, unibi_exit_am_mode, "\033[?7l");
    ok(unibi_get_str(ut, unibi_xon_character) == NULL, "xon_character = null");
    ok(unibi_get_str(ut, unibi_xoff_character) == NULL, "xoff_character = null");
    ok(unibi_get_str(ut, unibi_ena_acs) == NULL, "ena_acs = null");
    ok(unibi_get_str(ut, unibi_label_on) == NULL, "label_on = null");
    ok(unibi_get_str(ut, unibi_label_off) == NULL, "label_off = null");
    ok(unibi_get_str(ut, unibi_key_beg) == NULL, "key_beg = null");
    ok(unibi_get_str(ut, unibi_key_cancel) == NULL, "key_cancel = null");
    ok(unibi_get_str(ut, unibi_key_close) == NULL, "key_close = null");
    ok(unibi_get_str(ut, unibi_key_command) == NULL, "key_command = null");
    ok(unibi_get_str(ut, unibi_key_copy) == NULL, "key_copy = null");
    ok(unibi_get_str(ut, unibi_key_create) == NULL, "key_create = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_end), "\033OF") == 0, "key_end = \"\\033OF\"");
    unibi_set_str(dt, unibi_key_end, "\033OF");
    ok(strcmp(unibi_get_str(ut, unibi_key_enter), "\033OM") == 0, "key_enter = \"\\033OM\"");
    unibi_set_str(dt, unibi_key_enter, "\033OM");
    ok(unibi_get_str(ut, unibi_key_exit) == NULL, "key_exit = null");
    ok(unibi_get_str(ut, unibi_key_find) == NULL, "key_find = null");
    ok(unibi_get_str(ut, unibi_key_help) == NULL, "key_help = null");
    ok(unibi_get_str(ut, unibi_key_mark) == NULL, "key_mark = null");
    ok(unibi_get_str(ut, unibi_key_message) == NULL, "key_message = null");
    ok(unibi_get_str(ut, unibi_key_move) == NULL, "key_move = null");
    ok(unibi_get_str(ut, unibi_key_next) == NULL, "key_next = null");
    ok(unibi_get_str(ut, unibi_key_open) == NULL, "key_open = null");
    ok(unibi_get_str(ut, unibi_key_options) == NULL, "key_options = null");
    ok(unibi_get_str(ut, unibi_key_previous) == NULL, "key_previous = null");
    ok(unibi_get_str(ut, unibi_key_print) == NULL, "key_print = null");
    ok(unibi_get_str(ut, unibi_key_redo) == NULL, "key_redo = null");
    ok(unibi_get_str(ut, unibi_key_reference) == NULL, "key_reference = null");
    ok(unibi_get_str(ut, unibi_key_refresh) == NULL, "key_refresh = null");
    ok(unibi_get_str(ut, unibi_key_replace) == NULL, "key_replace = null");
    ok(unibi_get_str(ut, unibi_key_restart) == NULL, "key_restart = null");
    ok(unibi_get_str(ut, unibi_key_resume) == NULL, "key_resume = null");
    ok(unibi_get_str(ut, unibi_key_save) == NULL, "key_save = null");
    ok(unibi_get_str(ut, unibi_key_suspend) == NULL, "key_suspend = null");
    ok(unibi_get_str(ut, unibi_key_undo) == NULL, "key_undo = null");
    ok(unibi_get_str(ut, unibi_key_sbeg) == NULL, "key_sbeg = null");
    ok(unibi_get_str(ut, unibi_key_scancel) == NULL, "key_scancel = null");
    ok(unibi_get_str(ut, unibi_key_scommand) == NULL, "key_scommand = null");
    ok(unibi_get_str(ut, unibi_key_scopy) == NULL, "key_scopy = null");
    ok(unibi_get_str(ut, unibi_key_screate) == NULL, "key_screate = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_sdc), "\033[3;2~") == 0, "key_sdc = \"\\033[3;2~\"");
    unibi_set_str(dt, unibi_key_sdc, "\033[3;2~");
    ok(unibi_get_str(ut, unibi_key_sdl) == NULL, "key_sdl = null");
    ok(unibi_get_str(ut, unibi_key_select) == NULL, "key_select = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_send), "\033[1;2F") == 0, "key_send = \"\\033[1;2F\"");
    unibi_set_str(dt, unibi_key_send, "\033[1;2F");
    ok(unibi_get_str(ut, unibi_key_seol) == NULL, "key_seol = null");
    ok(unibi_get_str(ut, unibi_key_sexit) == NULL, "key_sexit = null");
    ok(unibi_get_str(ut, unibi_key_sfind) == NULL, "key_sfind = null");
    ok(unibi_get_str(ut, unibi_key_shelp) == NULL, "key_shelp = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_shome), "\033[1;2H") == 0, "key_shome = \"\\033[1;2H\"");
    unibi_set_str(dt, unibi_key_shome, "\033[1;2H");
    ok(strcmp(unibi_get_str(ut, unibi_key_sic), "\033[2;2~") == 0, "key_sic = \"\\033[2;2~\"");
    unibi_set_str(dt, unibi_key_sic, "\033[2;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_sleft), "\033[1;2D") == 0, "key_sleft = \"\\033[1;2D\"");
    unibi_set_str(dt, unibi_key_sleft, "\033[1;2D");
    ok(unibi_get_str(ut, unibi_key_smessage) == NULL, "key_smessage = null");
    ok(unibi_get_str(ut, unibi_key_smove) == NULL, "key_smove = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_snext), "\033[6;2~") == 0, "key_snext = \"\\033[6;2~\"");
    unibi_set_str(dt, unibi_key_snext, "\033[6;2~");
    ok(unibi_get_str(ut, unibi_key_soptions) == NULL, "key_soptions = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_sprevious), "\033[5;2~") == 0, "key_sprevious = \"\\033[5;2~\"");
    unibi_set_str(dt, unibi_key_sprevious, "\033[5;2~");
    ok(unibi_get_str(ut, unibi_key_sprint) == NULL, "key_sprint = null");
    ok(unibi_get_str(ut, unibi_key_sredo) == NULL, "key_sredo = null");
    ok(unibi_get_str(ut, unibi_key_sreplace) == NULL, "key_sreplace = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_sright), "\033[1;2C") == 0, "key_sright = \"\\033[1;2C\"");
    unibi_set_str(dt, unibi_key_sright, "\033[1;2C");
    ok(unibi_get_str(ut, unibi_key_srsume) == NULL, "key_srsume = null");
    ok(unibi_get_str(ut, unibi_key_ssave) == NULL, "key_ssave = null");
    ok(unibi_get_str(ut, unibi_key_ssuspend) == NULL, "key_ssuspend = null");
    ok(unibi_get_str(ut, unibi_key_sundo) == NULL, "key_sundo = null");
    ok(unibi_get_str(ut, unibi_req_for_input) == NULL, "req_for_input = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_f11), "\033[23~") == 0, "key_f11 = \"\\033[23~\"");
    unibi_set_str(dt, unibi_key_f11, "\033[23~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f12), "\033[24~") == 0, "key_f12 = \"\\033[24~\"");
    unibi_set_str(dt, unibi_key_f12, "\033[24~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f13), "\033[1;2P") == 0, "key_f13 = \"\\033[1;2P\"");
    unibi_set_str(dt, unibi_key_f13, "\033[1;2P");
    ok(strcmp(unibi_get_str(ut, unibi_key_f14), "\033[1;2Q") == 0, "key_f14 = \"\\033[1;2Q\"");
    unibi_set_str(dt, unibi_key_f14, "\033[1;2Q");
    ok(strcmp(unibi_get_str(ut, unibi_key_f15), "\033[1;2R") == 0, "key_f15 = \"\\033[1;2R\"");
    unibi_set_str(dt, unibi_key_f15, "\033[1;2R");
    ok(strcmp(unibi_get_str(ut, unibi_key_f16), "\033[1;2S") == 0, "key_f16 = \"\\033[1;2S\"");
    unibi_set_str(dt, unibi_key_f16, "\033[1;2S");
    ok(strcmp(unibi_get_str(ut, unibi_key_f17), "\033[15;2~") == 0, "key_f17 = \"\\033[15;2~\"");
    unibi_set_str(dt, unibi_key_f17, "\033[15;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f18), "\033[17;2~") == 0, "key_f18 = \"\\033[17;2~\"");
    unibi_set_str(dt, unibi_key_f18, "\033[17;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f19), "\033[18;2~") == 0, "key_f19 = \"\\033[18;2~\"");
    unibi_set_str(dt, unibi_key_f19, "\033[18;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f20), "\033[19;2~") == 0, "key_f20 = \"\\033[19;2~\"");
    unibi_set_str(dt, unibi_key_f20, "\033[19;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f21), "\033[20;2~") == 0, "key_f21 = \"\\033[20;2~\"");
    unibi_set_str(dt, unibi_key_f21, "\033[20;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f22), "\033[21;2~") == 0, "key_f22 = \"\\033[21;2~\"");
    unibi_set_str(dt, unibi_key_f22, "\033[21;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f23), "\033[23;2~") == 0, "key_f23 = \"\\033[23;2~\"");
    unibi_set_str(dt, unibi_key_f23, "\033[23;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f24), "\033[24;2~") == 0, "key_f24 = \"\\033[24;2~\"");
    unibi_set_str(dt, unibi_key_f24, "\033[24;2~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f25), "\033[1;5P") == 0, "key_f25 = \"\\033[1;5P\"");
    unibi_set_str(dt, unibi_key_f25, "\033[1;5P");
    ok(strcmp(unibi_get_str(ut, unibi_key_f26), "\033[1;5Q") == 0, "key_f26 = \"\\033[1;5Q\"");
    unibi_set_str(dt, unibi_key_f26, "\033[1;5Q");
    ok(strcmp(unibi_get_str(ut, unibi_key_f27), "\033[1;5R") == 0, "key_f27 = \"\\033[1;5R\"");
    unibi_set_str(dt, unibi_key_f27, "\033[1;5R");
    ok(strcmp(unibi_get_str(ut, unibi_key_f28), "\033[1;5S") == 0, "key_f28 = \"\\033[1;5S\"");
    unibi_set_str(dt, unibi_key_f28, "\033[1;5S");
    ok(strcmp(unibi_get_str(ut, unibi_key_f29), "\033[15;5~") == 0, "key_f29 = \"\\033[15;5~\"");
    unibi_set_str(dt, unibi_key_f29, "\033[15;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f30), "\033[17;5~") == 0, "key_f30 = \"\\033[17;5~\"");
    unibi_set_str(dt, unibi_key_f30, "\033[17;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f31), "\033[18;5~") == 0, "key_f31 = \"\\033[18;5~\"");
    unibi_set_str(dt, unibi_key_f31, "\033[18;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f32), "\033[19;5~") == 0, "key_f32 = \"\\033[19;5~\"");
    unibi_set_str(dt, unibi_key_f32, "\033[19;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f33), "\033[20;5~") == 0, "key_f33 = \"\\033[20;5~\"");
    unibi_set_str(dt, unibi_key_f33, "\033[20;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f34), "\033[21;5~") == 0, "key_f34 = \"\\033[21;5~\"");
    unibi_set_str(dt, unibi_key_f34, "\033[21;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f35), "\033[23;5~") == 0, "key_f35 = \"\\033[23;5~\"");
    unibi_set_str(dt, unibi_key_f35, "\033[23;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f36), "\033[24;5~") == 0, "key_f36 = \"\\033[24;5~\"");
    unibi_set_str(dt, unibi_key_f36, "\033[24;5~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f37), "\033[1;6P") == 0, "key_f37 = \"\\033[1;6P\"");
    unibi_set_str(dt, unibi_key_f37, "\033[1;6P");
    ok(strcmp(unibi_get_str(ut, unibi_key_f38), "\033[1;6Q") == 0, "key_f38 = \"\\033[1;6Q\"");
    unibi_set_str(dt, unibi_key_f38, "\033[1;6Q");
    ok(strcmp(unibi_get_str(ut, unibi_key_f39), "\033[1;6R") == 0, "key_f39 = \"\\033[1;6R\"");
    unibi_set_str(dt, unibi_key_f39, "\033[1;6R");
    ok(strcmp(unibi_get_str(ut, unibi_key_f40), "\033[1;6S") == 0, "key_f40 = \"\\033[1;6S\"");
    unibi_set_str(dt, unibi_key_f40, "\033[1;6S");
    ok(strcmp(unibi_get_str(ut, unibi_key_f41), "\033[15;6~") == 0, "key_f41 = \"\\033[15;6~\"");
    unibi_set_str(dt, unibi_key_f41, "\033[15;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f42), "\033[17;6~") == 0, "key_f42 = \"\\033[17;6~\"");
    unibi_set_str(dt, unibi_key_f42, "\033[17;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f43), "\033[18;6~") == 0, "key_f43 = \"\\033[18;6~\"");
    unibi_set_str(dt, unibi_key_f43, "\033[18;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f44), "\033[19;6~") == 0, "key_f44 = \"\\033[19;6~\"");
    unibi_set_str(dt, unibi_key_f44, "\033[19;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f45), "\033[20;6~") == 0, "key_f45 = \"\\033[20;6~\"");
    unibi_set_str(dt, unibi_key_f45, "\033[20;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f46), "\033[21;6~") == 0, "key_f46 = \"\\033[21;6~\"");
    unibi_set_str(dt, unibi_key_f46, "\033[21;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f47), "\033[23;6~") == 0, "key_f47 = \"\\033[23;6~\"");
    unibi_set_str(dt, unibi_key_f47, "\033[23;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f48), "\033[24;6~") == 0, "key_f48 = \"\\033[24;6~\"");
    unibi_set_str(dt, unibi_key_f48, "\033[24;6~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f49), "\033[1;3P") == 0, "key_f49 = \"\\033[1;3P\"");
    unibi_set_str(dt, unibi_key_f49, "\033[1;3P");
    ok(strcmp(unibi_get_str(ut, unibi_key_f50), "\033[1;3Q") == 0, "key_f50 = \"\\033[1;3Q\"");
    unibi_set_str(dt, unibi_key_f50, "\033[1;3Q");
    ok(strcmp(unibi_get_str(ut, unibi_key_f51), "\033[1;3R") == 0, "key_f51 = \"\\033[1;3R\"");
    unibi_set_str(dt, unibi_key_f51, "\033[1;3R");
    ok(strcmp(unibi_get_str(ut, unibi_key_f52), "\033[1;3S") == 0, "key_f52 = \"\\033[1;3S\"");
    unibi_set_str(dt, unibi_key_f52, "\033[1;3S");
    ok(strcmp(unibi_get_str(ut, unibi_key_f53), "\033[15;3~") == 0, "key_f53 = \"\\033[15;3~\"");
    unibi_set_str(dt, unibi_key_f53, "\033[15;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f54), "\033[17;3~") == 0, "key_f54 = \"\\033[17;3~\"");
    unibi_set_str(dt, unibi_key_f54, "\033[17;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f55), "\033[18;3~") == 0, "key_f55 = \"\\033[18;3~\"");
    unibi_set_str(dt, unibi_key_f55, "\033[18;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f56), "\033[19;3~") == 0, "key_f56 = \"\\033[19;3~\"");
    unibi_set_str(dt, unibi_key_f56, "\033[19;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f57), "\033[20;3~") == 0, "key_f57 = \"\\033[20;3~\"");
    unibi_set_str(dt, unibi_key_f57, "\033[20;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f58), "\033[21;3~") == 0, "key_f58 = \"\\033[21;3~\"");
    unibi_set_str(dt, unibi_key_f58, "\033[21;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f59), "\033[23;3~") == 0, "key_f59 = \"\\033[23;3~\"");
    unibi_set_str(dt, unibi_key_f59, "\033[23;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f60), "\033[24;3~") == 0, "key_f60 = \"\\033[24;3~\"");
    unibi_set_str(dt, unibi_key_f60, "\033[24;3~");
    ok(strcmp(unibi_get_str(ut, unibi_key_f61), "\033[1;4P") == 0, "key_f61 = \"\\033[1;4P\"");
    unibi_set_str(dt, unibi_key_f61, "\033[1;4P");
    ok(strcmp(unibi_get_str(ut, unibi_key_f62), "\033[1;4Q") == 0, "key_f62 = \"\\033[1;4Q\"");
    unibi_set_str(dt, unibi_key_f62, "\033[1;4Q");
    ok(strcmp(unibi_get_str(ut, unibi_key_f63), "\033[1;4R") == 0, "key_f63 = \"\\033[1;4R\"");
    unibi_set_str(dt, unibi_key_f63, "\033[1;4R");
    ok(strcmp(unibi_get_str(ut, unibi_clr_bol), "\033[1K") == 0, "clr_bol = \"\\033[1K\"");
    unibi_set_str(dt, unibi_clr_bol, "\033[1K");
    ok(unibi_get_str(ut, unibi_clear_margins) == NULL, "clear_margins = null");
    ok(unibi_get_str(ut, unibi_set_left_margin) == NULL, "set_left_margin = null");
    ok(unibi_get_str(ut, unibi_set_right_margin) == NULL, "set_right_margin = null");
    ok(unibi_get_str(ut, unibi_label_format) == NULL, "label_format = null");
    ok(unibi_get_str(ut, unibi_set_clock) == NULL, "set_clock = null");
    ok(unibi_get_str(ut, unibi_display_clock) == NULL, "display_clock = null");
    ok(unibi_get_str(ut, unibi_remove_clock) == NULL, "remove_clock = null");
    ok(unibi_get_str(ut, unibi_create_window) == NULL, "create_window = null");
    ok(unibi_get_str(ut, unibi_goto_window) == NULL, "goto_window = null");
    ok(unibi_get_str(ut, unibi_hangup) == NULL, "hangup = null");
    ok(unibi_get_str(ut, unibi_dial_phone) == NULL, "dial_phone = null");
    ok(unibi_get_str(ut, unibi_quick_dial) == NULL, "quick_dial = null");
    ok(unibi_get_str(ut, unibi_tone) == NULL, "tone = null");
    ok(unibi_get_str(ut, unibi_pulse) == NULL, "pulse = null");
    ok(unibi_get_str(ut, unibi_flash_hook) == NULL, "flash_hook = null");
    ok(unibi_get_str(ut, unibi_fixed_pause) == NULL, "fixed_pause = null");
    ok(unibi_get_str(ut, unibi_wait_tone) == NULL, "wait_tone = null");
    ok(unibi_get_str(ut, unibi_user0) == NULL, "user0 = null");
    ok(unibi_get_str(ut, unibi_user1) == NULL, "user1 = null");
    ok(unibi_get_str(ut, unibi_user2) == NULL, "user2 = null");
    ok(unibi_get_str(ut, unibi_user3) == NULL, "user3 = null");
    ok(unibi_get_str(ut, unibi_user4) == NULL, "user4 = null");
    ok(unibi_get_str(ut, unibi_user5) == NULL, "user5 = null");
    ok(strcmp(unibi_get_str(ut, unibi_user6), "\033[%i%d;%dR") == 0, "user6 = \"\\033[%i%d;%dR\"");
    unibi_set_str(dt, unibi_user6, "\033[%i%d;%dR");
    ok(strcmp(unibi_get_str(ut, unibi_user7), "\033[6n") == 0, "user7 = \"\\033[6n\"");
    unibi_set_str(dt, unibi_user7, "\033[6n");
    ok(strcmp(unibi_get_str(ut, unibi_user8), "\033[?1;2c") == 0, "user8 = \"\\033[?1;2c\"");
    unibi_set_str(dt, unibi_user8, "\033[?1;2c");
    ok(strcmp(unibi_get_str(ut, unibi_user9), "\033[c") == 0, "user9 = \"\\033[c\"");
    unibi_set_str(dt, unibi_user9, "\033[c");
    ok(strcmp(unibi_get_str(ut, unibi_orig_pair), "\033[39;49m") == 0, "orig_pair = \"\\033[39;49m\"");
    unibi_set_str(dt, unibi_orig_pair, "\033[39;49m");
    ok(unibi_get_str(ut, unibi_orig_colors) == NULL, "orig_colors = null");
    ok(unibi_get_str(ut, unibi_initialize_color) == NULL, "initialize_color = null");
    ok(unibi_get_str(ut, unibi_initialize_pair) == NULL, "initialize_pair = null");
    ok(unibi_get_str(ut, unibi_set_color_pair) == NULL, "set_color_pair = null");
    ok(strcmp(unibi_get_str(ut, unibi_set_foreground), "\033[3%?%p1%{1}%=%t4%e%p1%{3}%=%t6%e%p1%{4}%=%t1%e%p1%{6}%=%t3%e%p1%d%;m") == 0, "set_foreground = \"\\033[3%?%p1%{1}%=%t4%e%p1%{3}%=%t6%e%p1%{4}%=%t1%e%p1%{6}%=%t3%e%p1%d%;m\"");
    unibi_set_str(dt, unibi_set_foreground, "\033[3%?%p1%{1}%=%t4%e%p1%{3}%=%t6%e%p1%{4}%=%t1%e%p1%{6}%=%t3%e%p1%d%;m");
    ok(strcmp(unibi_get_str(ut, unibi_set_background), "\033[4%?%p1%{1}%=%t4%e%p1%{3}%=%t6%e%p1%{4}%=%t1%e%p1%{6}%=%t3%e%p1%d%;m") == 0, "set_background = \"\\033[4%?%p1%{1}%=%t4%e%p1%{3}%=%t6%e%p1%{4}%=%t1%e%p1%{6}%=%t3%e%p1%d%;m\"");
    unibi_set_str(dt, unibi_set_background, "\033[4%?%p1%{1}%=%t4%e%p1%{3}%=%t6%e%p1%{4}%=%t1%e%p1%{6}%=%t3%e%p1%d%;m");
    ok(unibi_get_str(ut, unibi_change_char_pitch) == NULL, "change_char_pitch = null");
    ok(unibi_get_str(ut, unibi_change_line_pitch) == NULL, "change_line_pitch = null");
    ok(unibi_get_str(ut, unibi_change_res_horz) == NULL, "change_res_horz = null");
    ok(unibi_get_str(ut, unibi_change_res_vert) == NULL, "change_res_vert = null");
    ok(unibi_get_str(ut, unibi_define_char) == NULL, "define_char = null");
    ok(unibi_get_str(ut, unibi_enter_doublewide_mode) == NULL, "enter_doublewide_mode = null");
    ok(unibi_get_str(ut, unibi_enter_draft_quality) == NULL, "enter_draft_quality = null");
    ok(unibi_get_str(ut, unibi_enter_italics_mode) == NULL, "enter_italics_mode = null");
    ok(unibi_get_str(ut, unibi_enter_leftward_mode) == NULL, "enter_leftward_mode = null");
    ok(unibi_get_str(ut, unibi_enter_micro_mode) == NULL, "enter_micro_mode = null");
    ok(unibi_get_str(ut, unibi_enter_near_letter_quality) == NULL, "enter_near_letter_quality = null");
    ok(unibi_get_str(ut, unibi_enter_normal_quality) == NULL, "enter_normal_quality = null");
    ok(unibi_get_str(ut, unibi_enter_shadow_mode) == NULL, "enter_shadow_mode = null");
    ok(unibi_get_str(ut, unibi_enter_subscript_mode) == NULL, "enter_subscript_mode = null");
    ok(unibi_get_str(ut, unibi_enter_superscript_mode) == NULL, "enter_superscript_mode = null");
    ok(unibi_get_str(ut, unibi_enter_upward_mode) == NULL, "enter_upward_mode = null");
    ok(unibi_get_str(ut, unibi_exit_doublewide_mode) == NULL, "exit_doublewide_mode = null");
    ok(unibi_get_str(ut, unibi_exit_italics_mode) == NULL, "exit_italics_mode = null");
    ok(unibi_get_str(ut, unibi_exit_leftward_mode) == NULL, "exit_leftward_mode = null");
    ok(unibi_get_str(ut, unibi_exit_micro_mode) == NULL, "exit_micro_mode = null");
    ok(unibi_get_str(ut, unibi_exit_shadow_mode) == NULL, "exit_shadow_mode = null");
    ok(unibi_get_str(ut, unibi_exit_subscript_mode) == NULL, "exit_subscript_mode = null");
    ok(unibi_get_str(ut, unibi_exit_superscript_mode) == NULL, "exit_superscript_mode = null");
    ok(unibi_get_str(ut, unibi_exit_upward_mode) == NULL, "exit_upward_mode = null");
    ok(unibi_get_str(ut, unibi_micro_column_address) == NULL, "micro_column_address = null");
    ok(unibi_get_str(ut, unibi_micro_down) == NULL, "micro_down = null");
    ok(unibi_get_str(ut, unibi_micro_left) == NULL, "micro_left = null");
    ok(unibi_get_str(ut, unibi_micro_right) == NULL, "micro_right = null");
    ok(unibi_get_str(ut, unibi_micro_row_address) == NULL, "micro_row_address = null");
    ok(unibi_get_str(ut, unibi_micro_up) == NULL, "micro_up = null");
    ok(unibi_get_str(ut, unibi_order_of_pins) == NULL, "order_of_pins = null");
    ok(unibi_get_str(ut, unibi_parm_down_micro) == NULL, "parm_down_micro = null");
    ok(unibi_get_str(ut, unibi_parm_left_micro) == NULL, "parm_left_micro = null");
    ok(unibi_get_str(ut, unibi_parm_right_micro) == NULL, "parm_right_micro = null");
    ok(unibi_get_str(ut, unibi_parm_up_micro) == NULL, "parm_up_micro = null");
    ok(unibi_get_str(ut, unibi_select_char_set) == NULL, "select_char_set = null");
    ok(unibi_get_str(ut, unibi_set_bottom_margin) == NULL, "set_bottom_margin = null");
    ok(unibi_get_str(ut, unibi_set_bottom_margin_parm) == NULL, "set_bottom_margin_parm = null");
    ok(unibi_get_str(ut, unibi_set_left_margin_parm) == NULL, "set_left_margin_parm = null");
    ok(unibi_get_str(ut, unibi_set_right_margin_parm) == NULL, "set_right_margin_parm = null");
    ok(unibi_get_str(ut, unibi_set_top_margin) == NULL, "set_top_margin = null");
    ok(unibi_get_str(ut, unibi_set_top_margin_parm) == NULL, "set_top_margin_parm = null");
    ok(unibi_get_str(ut, unibi_start_bit_image) == NULL, "start_bit_image = null");
    ok(unibi_get_str(ut, unibi_start_char_set_def) == NULL, "start_char_set_def = null");
    ok(unibi_get_str(ut, unibi_stop_bit_image) == NULL, "stop_bit_image = null");
    ok(unibi_get_str(ut, unibi_stop_char_set_def) == NULL, "stop_char_set_def = null");
    ok(unibi_get_str(ut, unibi_subscript_characters) == NULL, "subscript_characters = null");
    ok(unibi_get_str(ut, unibi_superscript_characters) == NULL, "superscript_characters = null");
    ok(unibi_get_str(ut, unibi_these_cause_cr) == NULL, "these_cause_cr = null");
    ok(unibi_get_str(ut, unibi_zero_motion) == NULL, "zero_motion = null");
    ok(unibi_get_str(ut, unibi_char_set_names) == NULL, "char_set_names = null");
    ok(strcmp(unibi_get_str(ut, unibi_key_mouse), "\033[M") == 0, "key_mouse = \"\\033[M\"");
    unibi_set_str(dt, unibi_key_mouse, "\033[M");
    ok(unibi_get_str(ut, unibi_mouse_info) == NULL, "mouse_info = null");
    ok(unibi_get_str(ut, unibi_req_mouse_pos) == NULL, "req_mouse_pos = null");
    ok(unibi_get_str(ut, unibi_get_mouse) == NULL, "get_mouse = null");
    ok(strcmp(unibi_get_str(ut, unibi_set_a_foreground), "\033[3%p1%dm") == 0, "set_a_foreground = \"\\033[3%p1%dm\"");
    unibi_set_str(dt, unibi_set_a_foreground, "\033[3%p1%dm");
    ok(strcmp(unibi_get_str(ut, unibi_set_a_background), "\033[4%p1%dm") == 0, "set_a_background = \"\\033[4%p1%dm\"");
    unibi_set_str(dt, unibi_set_a_background, "\033[4%p1%dm");
    ok(unibi_get_str(ut, unibi_pkey_plab) == NULL, "pkey_plab = null");
    ok(unibi_get_str(ut, unibi_device_type) == NULL, "device_type = null");
    ok(unibi_get_str(ut, unibi_code_set_init) == NULL, "code_set_init = null");
    ok(unibi_get_str(ut, unibi_set0_des_seq) == NULL, "set0_des_seq = null");
    ok(unibi_get_str(ut, unibi_set1_des_seq) == NULL, "set1_des_seq = null");
    ok(unibi_get_str(ut, unibi_set2_des_seq) == NULL, "set2_des_seq = null");
    ok(unibi_get_str(ut, unibi_set3_des_seq) == NULL, "set3_des_seq = null");
    ok(unibi_get_str(ut, unibi_set_lr_margin) == NULL, "set_lr_margin = null");
    ok(unibi_get_str(ut, unibi_set_tb_margin) == NULL, "set_tb_margin = null");
    ok(unibi_get_str(ut, unibi_bit_image_repeat) == NULL, "bit_image_repeat = null");
    ok(unibi_get_str(ut, unibi_bit_image_newline) == NULL, "bit_image_newline = null");
    ok(unibi_get_str(ut, unibi_bit_image_carriage_return) == NULL, "bit_image_carriage_return = null");
    ok(unibi_get_str(ut, unibi_color_names) == NULL, "color_names = null");
    ok(unibi_get_str(ut, unibi_define_bit_image_region) == NULL, "define_bit_image_region = null");
    ok(unibi_get_str(ut, unibi_end_bit_image_region) == NULL, "end_bit_image_region = null");
    ok(unibi_get_str(ut, unibi_set_color_band) == NULL, "set_color_band = null");
    ok(unibi_get_str(ut, unibi_set_page_length) == NULL, "set_page_length = null");
    ok(unibi_get_str(ut, unibi_display_pc_char) == NULL, "display_pc_char = null");
    ok(unibi_get_str(ut, unibi_enter_pc_charset_mode) == NULL, "enter_pc_charset_mode = null");
    ok(unibi_get_str(ut, unibi_exit_pc_charset_mode) == NULL, "exit_pc_charset_mode = null");
    ok(unibi_get_str(ut, unibi_enter_scancode_mode) == NULL, "enter_scancode_mode = null");
    ok(unibi_get_str(ut, unibi_exit_scancode_mode) == NULL, "exit_scancode_mode = null");
    ok(unibi_get_str(ut, unibi_pc_term_options) == NULL, "pc_term_options = null");
    ok(unibi_get_str(ut, unibi_scancode_escape) == NULL, "scancode_escape = null");
    ok(unibi_get_str(ut, unibi_alt_scancode_esc) == NULL, "alt_scancode_esc = null");
    ok(unibi_get_str(ut, unibi_enter_horizontal_hl_mode) == NULL, "enter_horizontal_hl_mode = null");
    ok(unibi_get_str(ut, unibi_enter_left_hl_mode) == NULL, "enter_left_hl_mode = null");
    ok(unibi_get_str(ut, unibi_enter_low_hl_mode) == NULL, "enter_low_hl_mode = null");
    ok(unibi_get_str(ut, unibi_enter_right_hl_mode) == NULL, "enter_right_hl_mode = null");
    ok(unibi_get_str(ut, unibi_enter_top_hl_mode) == NULL, "enter_top_hl_mode = null");
    ok(unibi_get_str(ut, unibi_enter_vertical_hl_mode) == NULL, "enter_vertical_hl_mode = null");
    ok(unibi_get_str(ut, unibi_set_a_attributes) == NULL, "set_a_attributes = null");
    ok(unibi_get_str(ut, unibi_set_pglen_inch) == NULL, "set_pglen_inch = null");
    ok(unibi_get_str(ut, unibi_termcap_init2) == NULL, "termcap_init2 = null");
    ok(unibi_get_str(ut, unibi_termcap_reset) == NULL, "termcap_reset = null");
    ok(unibi_get_str(ut, unibi_linefeed_if_not_lf) == NULL, "linefeed_if_not_lf = null");
    ok(unibi_get_str(ut, unibi_backspace_if_not_bs) == NULL, "backspace_if_not_bs = null");
    ok(unibi_get_str(ut, unibi_other_non_function_keys) == NULL, "other_non_function_keys = null");
    ok(unibi_get_str(ut, unibi_arrow_key_map) == NULL, "arrow_key_map = null");
    ok(unibi_get_str(ut, unibi_acs_ulcorner) == NULL, "acs_ulcorner = null");
    ok(unibi_get_str(ut, unibi_acs_llcorner) == NULL, "acs_llcorner = null");
    ok(unibi_get_str(ut, unibi_acs_urcorner) == NULL, "acs_urcorner = null");
    ok(unibi_get_str(ut, unibi_acs_lrcorner) == NULL, "acs_lrcorner = null");
    ok(unibi_get_str(ut, unibi_acs_ltee) == NULL, "acs_ltee = null");
    ok(unibi_get_str(ut, unibi_acs_rtee) == NULL, "acs_rtee = null");
    ok(unibi_get_str(ut, unibi_acs_btee) == NULL, "acs_btee = null");
    ok(unibi_get_str(ut, unibi_acs_ttee) == NULL, "acs_ttee = null");
    ok(unibi_get_str(ut, unibi_acs_hline) == NULL, "acs_hline = null");
    ok(unibi_get_str(ut, unibi_acs_vline) == NULL, "acs_vline = null");
    ok(unibi_get_str(ut, unibi_acs_plus) == NULL, "acs_plus = null");
    ok(strcmp(unibi_get_str(ut, unibi_memory_lock), "\033l") == 0, "memory_lock = \"\\033l\"");
    unibi_set_str(dt, unibi_memory_lock, "\033l");
    ok(strcmp(unibi_get_str(ut, unibi_memory_unlock), "\033m") == 0, "memory_unlock = \"\\033m\"");
    unibi_set_str(dt, unibi_memory_unlock, "\033m");
    ok(unibi_get_str(ut, unibi_box_chars_1) == NULL, "box_chars_1 = null");

    diag("extended boolean capabilities");
    {
        const size_t n_ext = unibi_count_ext_bool(ut);
        ok(n_ext == 2, "#ext_bool = 2");
        ok(0 < n_ext && unibi_get_ext_bool(ut, 0) == 1, "ext_bool[0].value = 1");
        ok(0 < n_ext && strcmp(unibi_get_ext_bool_name(ut, 0), "AX") == 0, "ext_bool[0].name = \"AX\"");
        unibi_add_ext_bool(dt, "AX", 1);
        ok(1 < n_ext && unibi_get_ext_bool(ut, 1) == 0, "ext_bool[1].value = 0");
        ok(1 < n_ext && strcmp(unibi_get_ext_bool_name(ut, 1), "XT") == 0, "ext_bool[1].name = \"XT\"");
        unibi_add_ext_bool(dt, "XT", 0);
    }

    diag("extended numeric capabilities");
    {
        const size_t n_ext = unibi_count_ext_num(ut);
        ok(n_ext == 1, "#ext_num = 1");
        ok(0 < n_ext && unibi_get_ext_num(ut, 0) == -1, "ext_num[0].value = -1");
        ok(0 < n_ext && strcmp(unibi_get_ext_num_name(ut, 0), "U8") == 0, "ext_num[0].name = \"U8\"");
        unibi_add_ext_num(dt, "U8", -1);
    }

    diag("extended string capabilities");
    {
        const size_t n_ext = unibi_count_ext_str(ut);
        ok(n_ext == 57, "#ext_str = 57");
        ok(0 < n_ext && unibi_get_ext_str(ut, 0) == NULL, "ext_str[0].value = null");
        unibi_add_ext_str(dt, "XM", NULL);
        ok(0 < n_ext && strcmp(unibi_get_ext_str_name(ut, 0), "XM") == 0, "ext_str[0].name = \"XM\"");
        ok(1 < n_ext && strcmp(unibi_get_ext_str(ut, 1), "\033[3;3~") == 0, "ext_str[1].value = \"\\033[3;3~\"");
        unibi_add_ext_str(dt, "kDC3", "\033[3;3~");
        ok(1 < n_ext && strcmp(unibi_get_ext_str_name(ut, 1), "kDC3") == 0, "ext_str[1].name = \"kDC3\"");
        ok(2 < n_ext && strcmp(unibi_get_ext_str(ut, 2), "\033[3;4~") == 0, "ext_str[2].value = \"\\033[3;4~\"");
        unibi_add_ext_str(dt, "kDC4", "\033[3;4~");
        ok(2 < n_ext && strcmp(unibi_get_ext_str_name(ut, 2), "kDC4") == 0, "ext_str[2].name = \"kDC4\"");
        ok(3 < n_ext && strcmp(unibi_get_ext_str(ut, 3), "\033[3;5~") == 0, "ext_str[3].value = \"\\033[3;5~\"");
        unibi_add_ext_str(dt, "kDC5", "\033[3;5~");
        ok(3 < n_ext && strcmp(unibi_get_ext_str_name(ut, 3), "kDC5") == 0, "ext_str[3].name = \"kDC5\"");
        ok(4 < n_ext && strcmp(unibi_get_ext_str(ut, 4), "\033[3;6~") == 0, "ext_str[4].value = \"\\033[3;6~\"");
        unibi_add_ext_str(dt, "kDC6", "\033[3;6~");
        ok(4 < n_ext && strcmp(unibi_get_ext_str_name(ut, 4), "kDC6") == 0, "ext_str[4].name = \"kDC6\"");
        ok(5 < n_ext && strcmp(unibi_get_ext_str(ut, 5), "\033[3;7~") == 0, "ext_str[5].value = \"\\033[3;7~\"");
        unibi_add_ext_str(dt, "kDC7", "\033[3;7~");
        ok(5 < n_ext && strcmp(unibi_get_ext_str_name(ut, 5), "kDC7") == 0, "ext_str[5].name = \"kDC7\"");
        ok(6 < n_ext && strcmp(unibi_get_ext_str(ut, 6), "\033[1;2B") == 0, "ext_str[6].value = \"\\033[1;2B\"");
        unibi_add_ext_str(dt, "kDN", "\033[1;2B");
        ok(6 < n_ext && strcmp(unibi_get_ext_str_name(ut, 6), "kDN") == 0, "ext_str[6].name = \"kDN\"");
        ok(7 < n_ext && strcmp(unibi_get_ext_str(ut, 7), "\033[1;3B") == 0, "ext_str[7].value = \"\\033[1;3B\"");
        unibi_add_ext_str(dt, "kDN3", "\033[1;3B");
        ok(7 < n_ext && strcmp(unibi_get_ext_str_name(ut, 7), "kDN3") == 0, "ext_str[7].name = \"kDN3\"");
        ok(8 < n_ext && strcmp(unibi_get_ext_str(ut, 8), "\033[1;4B") == 0, "ext_str[8].value = \"\\033[1;4B\"");
        unibi_add_ext_str(dt, "kDN4", "\033[1;4B");
        ok(8 < n_ext && strcmp(unibi_get_ext_str_name(ut, 8), "kDN4") == 0, "ext_str[8].name = \"kDN4\"");
        ok(9 < n_ext && strcmp(unibi_get_ext_str(ut, 9), "\033[1;5B") == 0, "ext_str[9].value = \"\\033[1;5B\"");
        unibi_add_ext_str(dt, "kDN5", "\033[1;5B");
        ok(9 < n_ext && strcmp(unibi_get_ext_str_name(ut, 9), "kDN5") == 0, "ext_str[9].name = \"kDN5\"");
        ok(10 < n_ext && strcmp(unibi_get_ext_str(ut, 10), "\033[1;6B") == 0, "ext_str[10].value = \"\\033[1;6B\"");
        unibi_add_ext_str(dt, "kDN6", "\033[1;6B");
        ok(10 < n_ext && strcmp(unibi_get_ext_str_name(ut, 10), "kDN6") == 0, "ext_str[10].name = \"kDN6\"");
        ok(11 < n_ext && strcmp(unibi_get_ext_str(ut, 11), "\033[1;7B") == 0, "ext_str[11].value = \"\\033[1;7B\"");
        unibi_add_ext_str(dt, "kDN7", "\033[1;7B");
        ok(11 < n_ext && strcmp(unibi_get_ext_str_name(ut, 11), "kDN7") == 0, "ext_str[11].name = \"kDN7\"");
        ok(12 < n_ext && strcmp(unibi_get_ext_str(ut, 12), "\033[1;3F") == 0, "ext_str[12].value = \"\\033[1;3F\"");
        unibi_add_ext_str(dt, "kEND3", "\033[1;3F");
        ok(12 < n_ext && strcmp(unibi_get_ext_str_name(ut, 12), "kEND3") == 0, "ext_str[12].name = \"kEND3\"");
        ok(13 < n_ext && strcmp(unibi_get_ext_str(ut, 13), "\033[1;4F") == 0, "ext_str[13].value = \"\\033[1;4F\"");
        unibi_add_ext_str(dt, "kEND4", "\033[1;4F");
        ok(13 < n_ext && strcmp(unibi_get_ext_str_name(ut, 13), "kEND4") == 0, "ext_str[13].name = \"kEND4\"");
        ok(14 < n_ext && strcmp(unibi_get_ext_str(ut, 14), "\033[1;5F") == 0, "ext_str[14].value = \"\\033[1;5F\"");
        unibi_add_ext_str(dt, "kEND5", "\033[1;5F");
        ok(14 < n_ext && strcmp(unibi_get_ext_str_name(ut, 14), "kEND5") == 0, "ext_str[14].name = \"kEND5\"");
        ok(15 < n_ext && strcmp(unibi_get_ext_str(ut, 15), "\033[1;6F") == 0, "ext_str[15].value = \"\\033[1;6F\"");
        unibi_add_ext_str(dt, "kEND6", "\033[1;6F");
        ok(15 < n_ext && strcmp(unibi_get_ext_str_name(ut, 15), "kEND6") == 0, "ext_str[15].name = \"kEND6\"");
        ok(16 < n_ext && strcmp(unibi_get_ext_str(ut, 16), "\033[1;7F") == 0, "ext_str[16].value = \"\\033[1;7F\"");
        unibi_add_ext_str(dt, "kEND7", "\033[1;7F");
        ok(16 < n_ext && strcmp(unibi_get_ext_str_name(ut, 16), "kEND7") == 0, "ext_str[16].name = \"kEND7\"");
        ok(17 < n_ext && strcmp(unibi_get_ext_str(ut, 17), "\033[1;3H") == 0, "ext_str[17].value = \"\\033[1;3H\"");
        unibi_add_ext_str(dt, "kHOM3", "\033[1;3H");
        ok(17 < n_ext && strcmp(unibi_get_ext_str_name(ut, 17), "kHOM3") == 0, "ext_str[17].name = \"kHOM3\"");
        ok(18 < n_ext && strcmp(unibi_get_ext_str(ut, 18), "\033[1;4H") == 0, "ext_str[18].value = \"\\033[1;4H\"");
        unibi_add_ext_str(dt, "kHOM4", "\033[1;4H");
        ok(18 < n_ext && strcmp(unibi_get_ext_str_name(ut, 18), "kHOM4") == 0, "ext_str[18].name = \"kHOM4\"");
        ok(19 < n_ext && strcmp(unibi_get_ext_str(ut, 19), "\033[1;5H") == 0, "ext_str[19].value = \"\\033[1;5H\"");
        unibi_add_ext_str(dt, "kHOM5", "\033[1;5H");
        ok(19 < n_ext && strcmp(unibi_get_ext_str_name(ut, 19), "kHOM5") == 0, "ext_str[19].name = \"kHOM5\"");
        ok(20 < n_ext && strcmp(unibi_get_ext_str(ut, 20), "\033[1;6H") == 0, "ext_str[20].value = \"\\033[1;6H\"");
        unibi_add_ext_str(dt, "kHOM6", "\033[1;6H");
        ok(20 < n_ext && strcmp(unibi_get_ext_str_name(ut, 20), "kHOM6") == 0, "ext_str[20].name = \"kHOM6\"");
        ok(21 < n_ext && strcmp(unibi_get_ext_str(ut, 21), "\033[1;7H") == 0, "ext_str[21].value = \"\\033[1;7H\"");
        unibi_add_ext_str(dt, "kHOM7", "\033[1;7H");
        ok(21 < n_ext && strcmp(unibi_get_ext_str_name(ut, 21), "kHOM7") == 0, "ext_str[21].name = \"kHOM7\"");
        ok(22 < n_ext && strcmp(unibi_get_ext_str(ut, 22), "\033[2;3~") == 0, "ext_str[22].value = \"\\033[2;3~\"");
        unibi_add_ext_str(dt, "kIC3", "\033[2;3~");
        ok(22 < n_ext && strcmp(unibi_get_ext_str_name(ut, 22), "kIC3") == 0, "ext_str[22].name = \"kIC3\"");
        ok(23 < n_ext && strcmp(unibi_get_ext_str(ut, 23), "\033[2;4~") == 0, "ext_str[23].value = \"\\033[2;4~\"");
        unibi_add_ext_str(dt, "kIC4", "\033[2;4~");
        ok(23 < n_ext && strcmp(unibi_get_ext_str_name(ut, 23), "kIC4") == 0, "ext_str[23].name = \"kIC4\"");
        ok(24 < n_ext && strcmp(unibi_get_ext_str(ut, 24), "\033[2;5~") == 0, "ext_str[24].value = \"\\033[2;5~\"");
        unibi_add_ext_str(dt, "kIC5", "\033[2;5~");
        ok(24 < n_ext && strcmp(unibi_get_ext_str_name(ut, 24), "kIC5") == 0, "ext_str[24].name = \"kIC5\"");
        ok(25 < n_ext && strcmp(unibi_get_ext_str(ut, 25), "\033[2;6~") == 0, "ext_str[25].value = \"\\033[2;6~\"");
        unibi_add_ext_str(dt, "kIC6", "\033[2;6~");
        ok(25 < n_ext && strcmp(unibi_get_ext_str_name(ut, 25), "kIC6") == 0, "ext_str[25].name = \"kIC6\"");
        ok(26 < n_ext && strcmp(unibi_get_ext_str(ut, 26), "\033[2;7~") == 0, "ext_str[26].value = \"\\033[2;7~\"");
        unibi_add_ext_str(dt, "kIC7", "\033[2;7~");
        ok(26 < n_ext && strcmp(unibi_get_ext_str_name(ut, 26), "kIC7") == 0, "ext_str[26].name = \"kIC7\"");
        ok(27 < n_ext && strcmp(unibi_get_ext_str(ut, 27), "\033[1;3D") == 0, "ext_str[27].value = \"\\033[1;3D\"");
        unibi_add_ext_str(dt, "kLFT3", "\033[1;3D");
        ok(27 < n_ext && strcmp(unibi_get_ext_str_name(ut, 27), "kLFT3") == 0, "ext_str[27].name = \"kLFT3\"");
        ok(28 < n_ext && strcmp(unibi_get_ext_str(ut, 28), "\033[1;4D") == 0, "ext_str[28].value = \"\\033[1;4D\"");
        unibi_add_ext_str(dt, "kLFT4", "\033[1;4D");
        ok(28 < n_ext && strcmp(unibi_get_ext_str_name(ut, 28), "kLFT4") == 0, "ext_str[28].name = \"kLFT4\"");
        ok(29 < n_ext && strcmp(unibi_get_ext_str(ut, 29), "\033[1;5D") == 0, "ext_str[29].value = \"\\033[1;5D\"");
        unibi_add_ext_str(dt, "kLFT5", "\033[1;5D");
        ok(29 < n_ext && strcmp(unibi_get_ext_str_name(ut, 29), "kLFT5") == 0, "ext_str[29].name = \"kLFT5\"");
        ok(30 < n_ext && strcmp(unibi_get_ext_str(ut, 30), "\033[1;6D") == 0, "ext_str[30].value = \"\\033[1;6D\"");
        unibi_add_ext_str(dt, "kLFT6", "\033[1;6D");
        ok(30 < n_ext && strcmp(unibi_get_ext_str_name(ut, 30), "kLFT6") == 0, "ext_str[30].name = \"kLFT6\"");
        ok(31 < n_ext && strcmp(unibi_get_ext_str(ut, 31), "\033[1;7D") == 0, "ext_str[31].value = \"\\033[1;7D\"");
        unibi_add_ext_str(dt, "kLFT7", "\033[1;7D");
        ok(31 < n_ext && strcmp(unibi_get_ext_str_name(ut, 31), "kLFT7") == 0, "ext_str[31].name = \"kLFT7\"");
        ok(32 < n_ext && strcmp(unibi_get_ext_str(ut, 32), "\033[6;3~") == 0, "ext_str[32].value = \"\\033[6;3~\"");
        unibi_add_ext_str(dt, "kNXT3", "\033[6;3~");
        ok(32 < n_ext && strcmp(unibi_get_ext_str_name(ut, 32), "kNXT3") == 0, "ext_str[32].name = \"kNXT3\"");
        ok(33 < n_ext && strcmp(unibi_get_ext_str(ut, 33), "\033[6;4~") == 0, "ext_str[33].value = \"\\033[6;4~\"");
        unibi_add_ext_str(dt, "kNXT4", "\033[6;4~");
        ok(33 < n_ext && strcmp(unibi_get_ext_str_name(ut, 33), "kNXT4") == 0, "ext_str[33].name = \"kNXT4\"");
        ok(34 < n_ext && strcmp(unibi_get_ext_str(ut, 34), "\033[6;5~") == 0, "ext_str[34].value = \"\\033[6;5~\"");
        unibi_add_ext_str(dt, "kNXT5", "\033[6;5~");
        ok(34 < n_ext && strcmp(unibi_get_ext_str_name(ut, 34), "kNXT5") == 0, "ext_str[34].name = \"kNXT5\"");
        ok(35 < n_ext && strcmp(unibi_get_ext_str(ut, 35), "\033[6;6~") == 0, "ext_str[35].value = \"\\033[6;6~\"");
        unibi_add_ext_str(dt, "kNXT6", "\033[6;6~");
        ok(35 < n_ext && strcmp(unibi_get_ext_str_name(ut, 35), "kNXT6") == 0, "ext_str[35].name = \"kNXT6\"");
        ok(36 < n_ext && strcmp(unibi_get_ext_str(ut, 36), "\033[6;7~") == 0, "ext_str[36].value = \"\\033[6;7~\"");
        unibi_add_ext_str(dt, "kNXT7", "\033[6;7~");
        ok(36 < n_ext && strcmp(unibi_get_ext_str_name(ut, 36), "kNXT7") == 0, "ext_str[36].name = \"kNXT7\"");
        ok(37 < n_ext && strcmp(unibi_get_ext_str(ut, 37), "\033[5;3~") == 0, "ext_str[37].value = \"\\033[5;3~\"");
        unibi_add_ext_str(dt, "kPRV3", "\033[5;3~");
        ok(37 < n_ext && strcmp(unibi_get_ext_str_name(ut, 37), "kPRV3") == 0, "ext_str[37].name = \"kPRV3\"");
        ok(38 < n_ext && strcmp(unibi_get_ext_str(ut, 38), "\033[5;4~") == 0, "ext_str[38].value = \"\\033[5;4~\"");
        unibi_add_ext_str(dt, "kPRV4", "\033[5;4~");
        ok(38 < n_ext && strcmp(unibi_get_ext_str_name(ut, 38), "kPRV4") == 0, "ext_str[38].name = \"kPRV4\"");
        ok(39 < n_ext && strcmp(unibi_get_ext_str(ut, 39), "\033[5;5~") == 0, "ext_str[39].value = \"\\033[5;5~\"");
        unibi_add_ext_str(dt, "kPRV5", "\033[5;5~");
        ok(39 < n_ext && strcmp(unibi_get_ext_str_name(ut, 39), "kPRV5") == 0, "ext_str[39].name = \"kPRV5\"");
        ok(40 < n_ext && strcmp(unibi_get_ext_str(ut, 40), "\033[5;6~") == 0, "ext_str[40].value = \"\\033[5;6~\"");
        unibi_add_ext_str(dt, "kPRV6", "\033[5;6~");
        ok(40 < n_ext && strcmp(unibi_get_ext_str_name(ut, 40), "kPRV6") == 0, "ext_str[40].name = \"kPRV6\"");
        ok(41 < n_ext && strcmp(unibi_get_ext_str(ut, 41), "\033[5;7~") == 0, "ext_str[41].value = \"\\033[5;7~\"");
        unibi_add_ext_str(dt, "kPRV7", "\033[5;7~");
        ok(41 < n_ext && strcmp(unibi_get_ext_str_name(ut, 41), "kPRV7") == 0, "ext_str[41].name = \"kPRV7\"");
        ok(42 < n_ext && strcmp(unibi_get_ext_str(ut, 42), "\033[1;3C") == 0, "ext_str[42].value = \"\\033[1;3C\"");
        unibi_add_ext_str(dt, "kRIT3", "\033[1;3C");
        ok(42 < n_ext && strcmp(unibi_get_ext_str_name(ut, 42), "kRIT3") == 0, "ext_str[42].name = \"kRIT3\"");
        ok(43 < n_ext && strcmp(unibi_get_ext_str(ut, 43), "\033[1;4C") == 0, "ext_str[43].value = \"\\033[1;4C\"");
        unibi_add_ext_str(dt, "kRIT4", "\033[1;4C");
        ok(43 < n_ext && strcmp(unibi_get_ext_str_name(ut, 43), "kRIT4") == 0, "ext_str[43].name = \"kRIT4\"");
        ok(44 < n_ext && strcmp(unibi_get_ext_str(ut, 44), "\033[1;5C") == 0, "ext_str[44].value = \"\\033[1;5C\"");
        unibi_add_ext_str(dt, "kRIT5", "\033[1;5C");
        ok(44 < n_ext && strcmp(unibi_get_ext_str_name(ut, 44), "kRIT5") == 0, "ext_str[44].name = \"kRIT5\"");
        ok(45 < n_ext && strcmp(unibi_get_ext_str(ut, 45), "\033[1;6C") == 0, "ext_str[45].value = \"\\033[1;6C\"");
        unibi_add_ext_str(dt, "kRIT6", "\033[1;6C");
        ok(45 < n_ext && strcmp(unibi_get_ext_str_name(ut, 45), "kRIT6") == 0, "ext_str[45].name = \"kRIT6\"");
        ok(46 < n_ext && strcmp(unibi_get_ext_str(ut, 46), "\033[1;7C") == 0, "ext_str[46].value = \"\\033[1;7C\"");
        unibi_add_ext_str(dt, "kRIT7", "\033[1;7C");
        ok(46 < n_ext && strcmp(unibi_get_ext_str_name(ut, 46), "kRIT7") == 0, "ext_str[46].name = \"kRIT7\"");
        ok(47 < n_ext && strcmp(unibi_get_ext_str(ut, 47), "\033[1;2A") == 0, "ext_str[47].value = \"\\033[1;2A\"");
        unibi_add_ext_str(dt, "kUP", "\033[1;2A");
        ok(47 < n_ext && strcmp(unibi_get_ext_str_name(ut, 47), "kUP") == 0, "ext_str[47].name = \"kUP\"");
        ok(48 < n_ext && strcmp(unibi_get_ext_str(ut, 48), "\033[1;3A") == 0, "ext_str[48].value = \"\\033[1;3A\"");
        unibi_add_ext_str(dt, "kUP3", "\033[1;3A");
        ok(48 < n_ext && strcmp(unibi_get_ext_str_name(ut, 48), "kUP3") == 0, "ext_str[48].name = \"kUP3\"");
        ok(49 < n_ext && strcmp(unibi_get_ext_str(ut, 49), "\033[1;4A") == 0, "ext_str[49].value = \"\\033[1;4A\"");
        unibi_add_ext_str(dt, "kUP4", "\033[1;4A");
        ok(49 < n_ext && strcmp(unibi_get_ext_str_name(ut, 49), "kUP4") == 0, "ext_str[49].name = \"kUP4\"");
        ok(50 < n_ext && strcmp(unibi_get_ext_str(ut, 50), "\033[1;5A") == 0, "ext_str[50].value = \"\\033[1;5A\"");
        unibi_add_ext_str(dt, "kUP5", "\033[1;5A");
        ok(50 < n_ext && strcmp(unibi_get_ext_str_name(ut, 50), "kUP5") == 0, "ext_str[50].name = \"kUP5\"");
        ok(51 < n_ext && strcmp(unibi_get_ext_str(ut, 51), "\033[1;6A") == 0, "ext_str[51].value = \"\\033[1;6A\"");
        unibi_add_ext_str(dt, "kUP6", "\033[1;6A");
        ok(51 < n_ext && strcmp(unibi_get_ext_str_name(ut, 51), "kUP6") == 0, "ext_str[51].name = \"kUP6\"");
        ok(52 < n_ext && strcmp(unibi_get_ext_str(ut, 52), "\033[1;7A") == 0, "ext_str[52].value = \"\\033[1;7A\"");
        unibi_add_ext_str(dt, "kUP7", "\033[1;7A");
        ok(52 < n_ext && strcmp(unibi_get_ext_str_name(ut, 52), "kUP7") == 0, "ext_str[52].name = \"kUP7\"");
        ok(53 < n_ext && unibi_get_ext_str(ut, 53) == NULL, "ext_str[53].value = null");
        unibi_add_ext_str(dt, "ka2", NULL);
        ok(53 < n_ext && strcmp(unibi_get_ext_str_name(ut, 53), "ka2") == 0, "ext_str[53].name = \"ka2\"");
        ok(54 < n_ext && unibi_get_ext_str(ut, 54) == NULL, "ext_str[54].value = null");
        unibi_add_ext_str(dt, "kb1", NULL);
        ok(54 < n_ext && strcmp(unibi_get_ext_str_name(ut, 54), "kb1") == 0, "ext_str[54].name = \"kb1\"");
        ok(55 < n_ext && unibi_get_ext_str(ut, 55) == NULL, "ext_str[55].value = null");
        unibi_add_ext_str(dt, "kb3", NULL);
        ok(55 < n_ext && strcmp(unibi_get_ext_str_name(ut, 55), "kb3") == 0, "ext_str[55].name = \"kb3\"");
        ok(56 < n_ext && unibi_get_ext_str(ut, 56) == NULL, "ext_str[56].value = null");
        unibi_add_ext_str(dt, "kc2", NULL);
        ok(56 < n_ext && strcmp(unibi_get_ext_str_name(ut, 56), "kc2") == 0, "ext_str[56].name = \"kc2\"");
    }

    {
        char buf[sizeof terminfo];
        size_t r = unibi_dump(ut, buf, sizeof buf);
        ok(r == sizeof terminfo, "redump size == orig size");
        ok(memcmp(terminfo, buf, sizeof buf) == 0, "redump == orig");
    }

    {
        char buf[sizeof terminfo];
        size_t r = unibi_dump(dt, buf, sizeof buf);
        ok(r == sizeof terminfo, "dummy redump size == orig size");
        ok(memcmp(terminfo, buf, sizeof buf) == 0, "dummy redump == orig");
    }

    unibi_destroy(ut);
    ok(1, "object destroyed");

    unibi_destroy(dt);
    ok(1, "dummy object destroyed");

    return 0;
}

static void setup(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);

    plan(631);
    test_counter = 1;
}
