#include <matpreter.h>
#include <math.h>

/**
 * @brief duplicate a binary tree in memory
 */
static token_btree_t* dup_btree(token_btree_t* og, void* (*allocator) (size_t)) {
    if (og == NULL) return NULL;

    token_btree_t* new = allocator(sizeof(token_btree_t));
    new->root = og->root;
    new->lhs = dup_btree(og->lhs, allocator);
    new->rhs = dup_btree(og->rhs, allocator);
    return new;
}

static void replace_token(token_btree_t* root, token_t replacedby, token_t toreplace) {
    if (root == NULL) return;
    if (root->root.type == toreplace.type && 
        root->root.value_i == toreplace.value_i && 
        root->root.value_r == toreplace.value_r) 
    {
        root->root = replacedby;
    }
    replace_token(root->lhs, replacedby, toreplace);
    replace_token(root->rhs, replacedby, toreplace);
}

/**
 * @brief Get numberic value of a btree
 * @param BT binary tree with NO VARIABLE literals.
 * @return real number resultant of the evaluation.
 */
static double get_numeric_btree(token_btree_t* BT) {
    if (BT == NULL) {return 0.0f;}
    switch (BT->root.type)
    {
    case OPERATION_ADD:
        return get_numeric_btree(BT->lhs) + get_numeric_btree(BT->rhs);
    case OPERATION_SUB:
        return get_numeric_btree(BT->lhs) - get_numeric_btree(BT->rhs);
    case OPERATION_MUL:
        return get_numeric_btree(BT->lhs) * get_numeric_btree(BT->rhs);
    case OPERATION_DIV:
        return get_numeric_btree(BT->lhs) / get_numeric_btree(BT->rhs);
    case OPERATION_EXP:
        return pow(get_numeric_btree(BT->lhs), get_numeric_btree(BT->rhs));
    case LITERAL_CONSTANT:
        return BT->root.value_r;
    default:
        ERROR("Evaluation: Unexpected token.");
        return NAN;
    }
}

errcode mp_evaluateAST(token_btree_t** _AST, token_list_t leftright_vars, int num_vars,
        _out_ double* value_r, _out_ int64_t* value_i) 
{
    token_btree_t* AST = *_AST;
    *value_r = 0.0f; *value_i = 0;
    if (AST == NULL) return 0;
    token_btree_t* new = dup_btree(AST, malloc);

    //Replace variables
    for (size_t i = 0; i < num_vars; i++)
    {
        token_t c = *(leftright_vars + i);
        token_t s = {.type = LITERAL_VARIABLE, .value_i = i, .value_r = 0.0f};
        replace_token(new, c, s);
    }
    
    *value_r = get_numeric_btree(new);
    *value_i = (int64_t)*value_r;
    
    mp_freeAST(new, free);
    if (isnan(*value_r)) return 1;
    else return 0;
}

