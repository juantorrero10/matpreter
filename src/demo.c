/**
 * Simple demo for testing the project.
 * 
 * In the future this'll problably be some sort of CLI
 * where you can especifiy functions and execute and make
 * calculations.
 */
#include <matpreter.h>


int main(void) {
    mp_parse("f(a, b, c, d) = (2.5a - (3b + 4*(c - d/2))^2) / ((a + b) * (c + d)) + 1.75");
    return 0;
}