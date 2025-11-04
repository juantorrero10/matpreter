#include <matpreter.h>


int main(void) {
    INFO("%lld", sizeof(token_t));
    mp_parse("f(a, b, c, d) = (2.5a - (3b + 4*(c - d/2))^2) / ((a + b) * (c + d)) + 1.75");
    return 0;
}