/**
 * Simple demo for testing the project.
 * 
 * In the future this'll problably be some sort of CLI
 * where you can especifiy functions and execute and make
 * calculations.
 */
#include <matpreter.h>


int main(void) {
    mp_parse("f(x, y) = 7x(-3 + y) * 3y + 4.6x^2 + y/2");
    return 0;
}