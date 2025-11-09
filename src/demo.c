/**
 * Simple demo for testing the project.
 * 
 * In the future this'll problably be some sort of CLI
 * where you can especifiy functions and execute and make
 * calculations.
 */
#include <matpreter.h>


int main(void) {
    function_t f = mp_parse("f(x) = -0.2x^5 + 1.5x^4 - 3x^3 + 2x^2 + 4x - 5");
    if (!f.body) return 1;
    //Evaluate f(0) to f(9)
    for (size_t i = 0; i < 10; i++)
    {
        double out_r = 0.0f;
        int64_t out_i = 0.0f;
        token_t x = {.type=LITERAL_CONSTANT, .value_i = 0, .value_r = (double)i};
        mp_evaluateAST(&f.body, &x, 1, &out_r, &out_i);
        INFO("f(%lu) = %f", i, out_r);
    }
    

    mp_free_func(&f, free);
    return 0;
}