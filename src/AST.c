#include <matpreter.h>

/**
 * @todo function bodies
 */
static token_btree_t* create_binary_tree(token_t root, token_btree_t lhs, token_btree_t rhs) {}
static token_array_t create_token_array(token_t* ptr, size_t sz) {}
static void append_token_array(token_array_t* arr, token_t t) {}
static void split_token_array(
    token_array_t og, size_t idx, 
    _out_ token_array_t* left, _out_ token_array_t* right
) 
{

}
token_btree_t* mp_createAST(token_array_t array, errcode* control) {}