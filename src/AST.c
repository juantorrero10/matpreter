#include <matpreter.h>

/**
 * @todo function bodies
 */
static token_btree_t* create_binary_tree(token_t root, token_btree_t* lhs, token_btree_t* rhs) {
    token_btree_t* ret = (token_btree_t*) malloc(sizeof(token_btree_t));
    ret->root = root;
    ret->lhs = lhs;
    ret->rhs = rhs;
    return ret;
}

/**
 * @return empty token array
 */
static token_array_t create_token_array() {
    return (token_array_t){NULL, 0, 0, 0};
}

static void append_token_array(token_array_t* arr, token_t t) {
    mp_token_append(&arr->ptr, t, &arr->sz, &arr->allocated);
}

/**
 * @brief Split an original token array into two parts
 * @attention - Output arrays are views, NOT copies but pointers inide the original one.
 *              Operations should not be perfomed, specially on the left one.
 * @attention - Split excludes the token right at the partition point (idx) and parenthesis.
 * 
 * ------
 * @param og    Original array
 * @param idx   index into the array
 * @param left  left half subarray of original
 * @param right right half subarray of original
 * 
 * ------
 * @return token right at [idx]
 */
static token_t split_token_array(
    token_array_t og, size_t idx, 
    _out_ token_array_t* left, _out_ token_array_t* right
) 
{
    _bool t = 0;    //temp bool for sizing in excluding parenthesis
    *left  = (token_array_t){ .ptr = NULL, .sz = 0, .allocated = 0, .st = 0 };
    *right = (token_array_t){ .ptr = NULL, .sz = 0, .allocated = 0, .st = 0 };

    //Left hand side
    if (idx != 0) {
        //Exclude parenthesis
        if (og.ptr[0].type == PARENTHESIS_OPEN 
            && og.ptr[idx - 1].type == PARENTHESIS_CLOSE) t = 1;
        left->ptr = og.ptr + t;
        left->sz = left->allocated = idx - t;
    }
    t = 0;

    //Right hand side
    if (idx < og.sz) {
        //Exclude parenthesis
        if (og.ptr[idx + 1].type == PARENTHESIS_OPEN 
            && og.ptr[og.sz - 1].type == PARENTHESIS_CLOSE) t = 1;

        right->ptr = og.ptr + idx + 1 + t;
        right->sz = og.sz - idx - 1 - t;
        right->allocated = og.allocated - idx - 1 - t;
    }

    //Return either token at idx or an invalid one.
    return (idx >= og.sz)? (token_t){.type = INVALID} : og.ptr[idx];
}


/**
 * @brief create a AST (Abstract Sintax Tree) using a recursive algorithm.
 * @param array token array that its going to work with
 * @param control starting on 1, position of the unexpected token
 * @return the language being described can be represented throw a binary tree.
 */
token_btree_t* mp_createAST(const token_array_t array, errcode* control) 
{
    //Colapse recursion if errorcode > 0
    if (*control > 0 || array.sz == 0)
        return create_binary_tree((token_t){.type = INVALID}, NULL, NULL);

    size_t read_idx = 0;
    token_array_t lhs, rhs;
    _bool seenParen = 0;

    //Search from lower importance to greater.
    // ^  >  */  > +-

    // (+) and (-)
    while(read_idx < array.sz) {
        token_t curr = array.ptr[read_idx];

        //Parenthesis, First in the chain
        if      (curr.type == PARENTHESIS_OPEN)  seenParen = 1;
        else if (curr.type == PARENTHESIS_CLOSE) seenParen = 0;

        
        if ((curr.type == OPERATION_ADD || curr.type == OPERATION_SUB) && seenParen == 0) {

            split_token_array(array, read_idx, &lhs, &rhs);

            //Add implicit zero
            if ( lhs.ptr == NULL) {
                token_t zero = (token_t){.type = LITERAL_CONSTANT, .value_i = 0, .value_r = 0.0f};
                append_token_array(&lhs, zero);
            }

            //If rhs token isnt a literal -> Unexpected token, colapse recursion
            if (rhs.ptr == NULL || rhs.ptr[0].type < TC_LITERALS_IDX) {
                *control = read_idx + 1;
                ERROR("Unexpected, token: %lld", *control);
                return create_binary_tree((token_t){.type = INVALID}, NULL, NULL);
            }

            //Recursion
            return create_binary_tree(curr, mp_createAST(lhs, control), mp_createAST(rhs, control));
        }
        read_idx++;
    }

    read_idx = 0;

    // (*) and (/)
    while(read_idx < array.sz) {
        token_t curr = array.ptr[read_idx];

        //Parenthesis, First in the chain
        if      (curr.type == PARENTHESIS_OPEN)  seenParen = 1;
        else if (curr.type == PARENTHESIS_CLOSE) seenParen = 0;
        
        if ((curr.type == OPERATION_MUL || curr.type == OPERATION_DIV) && seenParen == 0){
            split_token_array(array, read_idx, &lhs, &rhs);

            //Unexpected token, colapse recursion
            if (lhs.ptr == NULL || rhs.ptr == NULL || rhs.ptr[0].type < TC_LITERALS_IDX) {
                *control = read_idx + 1;
                ERROR("Unexpected, token: %lld", *control);
                return create_binary_tree((token_t){.type = INVALID}, NULL, NULL);
            }

            //Recursion
            return create_binary_tree(curr, mp_createAST(lhs, control), mp_createAST(rhs, control));
        }
        read_idx++;
    }

    //  (^)
    while (read_idx < array.sz) {
        token_t curr = array.ptr[read_idx];

        //Parenthesis, First in the chain
        if      (curr.type == PARENTHESIS_OPEN)  seenParen = 1;
        else if (curr.type == PARENTHESIS_CLOSE) seenParen = 0;
        
        if ((curr.type == OPERATION_EXP) && seenParen == 0){
            split_token_array(array, read_idx, &lhs, &rhs);

            //Unexpected token, colapse recursion
            if (lhs.ptr == NULL || rhs.ptr == NULL || rhs.ptr[0].type < TC_LITERALS_IDX) {
                *control = read_idx + 1;
                ERROR("Unexpected, token: %lld", *control);
                return create_binary_tree((token_t){.type = INVALID}, NULL, NULL);
            }

            //Recursion
            return create_binary_tree(curr, mp_createAST(lhs, control), mp_createAST(rhs, control));
        }
        read_idx++;
    }

    read_idx = 0;
    
    //IF array is only a literal
    if (array.sz == 1) 
        return create_binary_tree(array.ptr[0], NULL, NULL);
    else {
        *control = 1;
        return create_binary_tree((token_t){.type = INVALID}, NULL, NULL);
    }

}