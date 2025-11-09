#include <matpreter.h>
#include <mpdisplay.h>

token_array_t g_fullarray = {0};

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

static void append_token_array(token_array_t* arr, token_t t) {
    mp_token_append(&arr->ptr, t, &arr->sz, &arr->allocated);
}

/**
 * @brief helper function to split_token_array() for excluding parenthesis 
 */
static _bool are_outer_parens_balanced(const token_array_t arr) {
    if (arr.sz < 2) return 0;
    if (arr.ptr[0].type != PARENTHESIS_OPEN ||
        arr.ptr[arr.sz - 1].type != PARENTHESIS_CLOSE) return 0;
    int depth = 0;
    for (size_t i = 0; i < arr.sz - 1; i++) {
        if (arr.ptr[i].type == PARENTHESIS_OPEN) depth++;
        else if (arr.ptr[i].type == PARENTHESIS_CLOSE) depth--;
        if (depth == 0 && i < arr.sz - 2)
            return 0; // closed before end → not an outer pair
    }
    return 1;
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
    *left  = (token_array_t){0};
    *right = (token_array_t){0};

    if (idx >= og.sz)
        return (token_t){.type = INVALID};

    //Left hand side
    if (idx != 0) {
        left->ptr = og.ptr;
        left->sz = left->allocated = idx;
    }

    //Right hand side
    if (idx  + 1 < og.sz) {
        right->ptr = og.ptr + idx + 1;
        right->sz = og.sz - idx - 1;
        right->allocated = og.allocated - idx - 1;
    }

    // Remove outer parentheses if entire side is wrapped
    if (are_outer_parens_balanced(*left)) {
        left->ptr++;
        left->sz -= 2;
    }
    if (are_outer_parens_balanced(*right)) {
        right->ptr++;
        right->sz -= 2;
    }

    //Return either token at idx or an invalid one.
    return (idx >= og.sz)? (token_t){.type = INVALID} : og.ptr[idx];
}

static void display_unexpected_token(const token_array_t* a, token_t* t, uint64_t pos, preprocessor_info_t ppi) {
    ERROR("AST: Unexpected token: '%c' at position: %lu/%lld", mpd_char_token(t, ppi), pos, a->sz);
    LOG("\t");
    for (size_t i = 0; i < a->sz; i++)
    {
        if (i == pos) LOG("\x1b[31m[");
        mpd_print_token(a->ptr + i, ppi);
        if (i == pos) LOG("]\x1b[0m");
    }
    NL();NL();
    
}

/* assume token_array_t, token_t, token_btree_t, TC_LITERALS_IDX etc. are defined */

token_btree_t* mp_createAST(const token_array_t array, errcode* control, preprocessor_info_t ppi, _bool b_firstIter) {
    //If first iteration, save full array for error display
    if (b_firstIter) g_fullarray = array;
    if (!control) return NULL;
    if (*control > 0 || array.sz == 0) {
        token_t inv = { .type = INVALID };
        return create_binary_tree(inv, NULL, NULL);
    }

    token_array_t lhs = {0}, rhs = {0};

    // --- 1) + and -  (left-associative)  scan right->left
    {
        int depth = 0;
        for (int i = array.sz - 1; i >= 0; --i) {
            token_t curr = array.ptr[i];
            if (curr.type == PARENTHESIS_CLOSE) { depth++; continue; }
            if (curr.type == PARENTHESIS_OPEN)  { depth--; continue; }

            if (depth == 0 && (curr.type == OPERATION_ADD || curr.type == OPERATION_SUB)) {
                split_token_array(array, i, &lhs, &rhs);

                if (lhs.ptr == NULL) { // unary minus: turn into (0 - rhs)
                    token_t zero = { .type = LITERAL_CONSTANT, .value_r = 0.0 };
                    append_token_array(&lhs, zero);
                }

                if (rhs.ptr == NULL || lhs.ptr == NULL || mp_isoperation(lhs.ptr[i-1])) {
                    token_t a = lhs.ptr[i-1];
                    *control = curr.id + 1;
                    token_t inv = { .type = INVALID };
                    display_unexpected_token(&g_fullarray, &curr, curr.id, ppi);
                    return create_binary_tree(inv, NULL, NULL);
                }

                return create_binary_tree(curr, mp_createAST(lhs, control, ppi, 0), mp_createAST(rhs, control, ppi, 0));
            }
        }
    }

    // --- 2) * and / (left-associative) scan right->left
    {
        int depth = 0;
        for (int i = array.sz - 1; i >= 0; --i) {
            token_t curr = array.ptr[i];
            if (curr.type == PARENTHESIS_CLOSE) { depth++; continue; }
            if (curr.type == PARENTHESIS_OPEN)  { depth--; continue; }

            if (depth == 0 && (curr.type == OPERATION_MUL || curr.type == OPERATION_DIV)) {
                split_token_array(array, i, &lhs, &rhs);
                if (rhs.ptr == NULL || lhs.ptr == NULL || mp_isoperation(lhs.ptr[i-1])) {
                    mpd_print_token(&lhs.ptr[i-1], ppi);
                    *control = curr.id + 1;
                    token_t inv = { .type = INVALID };
                    display_unexpected_token(&g_fullarray, &curr, curr.id, ppi);
                    return create_binary_tree(inv, NULL, NULL);
                }
                return create_binary_tree(curr, mp_createAST(lhs, control, ppi, 0), mp_createAST(rhs, control, ppi, 0));
            }
        }
    }

    // --- 3) ^ exponent (right-associative) scan left->right
    {
        int depth = 0;
        for (size_t i = 0; i < array.sz; ++i) {
            token_t curr = array.ptr[i];
            if (curr.type == PARENTHESIS_OPEN) { depth++; continue; }
            if (curr.type == PARENTHESIS_CLOSE) { depth--; continue; }

            if (depth == 0 && curr.type == OPERATION_EXP) {
                split_token_array(array, i, &lhs, &rhs);
                if (lhs.ptr == NULL || rhs.ptr == NULL || mp_isoperation(rhs.ptr[0])) {
                    mpd_print_token(&rhs.ptr[0], ppi);
                    *control = curr.id + 1;
                    token_t inv = { .type = INVALID };
                    display_unexpected_token(&g_fullarray, &curr, curr.id, ppi);
                    return create_binary_tree(inv, NULL, NULL);
                }
                return create_binary_tree(curr, mp_createAST(lhs, control, ppi, 0), mp_createAST(rhs, control, ppi, 0));
            }
        }
    }

    // Base case: single literal/variable or parenthesized expression
    if (array.sz == 1) {
        return create_binary_tree(array.ptr[0], NULL, NULL);
    }

    // If wrapped in parentheses: strip them and recurse
    if (array.sz >= 2 && array.ptr[0].type == PARENTHESIS_OPEN && array.ptr[array.sz-1].type == PARENTHESIS_CLOSE) {
        // verify they balance as outer pair (helper function recommended)
        token_array_t inner = { .ptr = array.ptr + 1, .sz = array.sz - 2 };
        return mp_createAST(inner, control, ppi, 0);
    }

    // Unexpected structure
    *control = -1;
    token_t inv = { .type = INVALID };
    display_unexpected_token(&array, &(token_t){.type = INVALID}, -1, ppi);
    return create_binary_tree(inv, NULL, NULL);
}


/**
 * Recursive freeing of an AST
 * @param tree pointer to tree
 * @param deallocator function ptr to deallocator
 */
void mp_freeAST(token_btree_t* tree, void (*deallocator) (void*)) {
    if (tree == NULL) return;

    //This language doesn't produce "one-branched" nodes
    mp_freeAST(tree->lhs, deallocator); mp_freeAST(tree->rhs, deallocator);
    deallocator(tree);
    return;
    
}