/**
 * Simple demo for testing the project.
 * 
 * In the future this'll problably be some sort of CLI
 * where you can especifiy functions and execute and make
 * calculations.
 */
#include <matpreter.h>


int main(void) {
    token_btree_t* f = mp_parse("f(x) = x^3 - 4x^2 + 2");
    
    //Evaluate f(0) to f(9)
    for (size_t i = 0; i < 10; i++)
    {
        double out_r = 0.0f;
        int64_t out_i = 0.0f;
        token_t x = {.type=LITERAL_CONSTANT, .value_i = 0, .value_r = (double)i};
        mp_evaluateAST(f, &x, 1, &out_r, &out_i);
        INFO("f(%llu) = %f", i, out_r);
    }
    

    mp_freeAST(f, free);
    return 0;
}