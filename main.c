#include <matpreter.h>


int main(int argc, char** argv) {
    mp_parse("f(x) = 7.5x(6x + 4) + 3");
    return 0;
}