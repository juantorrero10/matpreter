#include <matpreter.h>


int main(void) {
    mp_parse("f(a, b, c, d) = (2.5a - (3b + 4*(c - d/2))^2) / ((a + b) * (c + d)) + 1.75");
    return 0;
}