#include <matpreter.h>

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

static void replace_var_to_const(token_btree_t* root, token_t _const, int idx_var) {
    if (_const.type != LITERAL_CONSTANT) return; //are you forreal?
    if (root == NULL) return;
    if (root->root.type == LITERAL_VARIABLE && root->root.value_i == idx_var) {
        root->root = _const;
    }
    replace_var_to_const(root->lhs, _const, idx_var);
    replace_var_to_const(root->rhs, _const, idx_var);
}

errcode mp_evaluateAST(token_btree_t* AST, token_list_t leftright_vars, int num_vars,
        _out_ double* value_r, _out_ int64_t* value_i) 
{
    *value_r = 0.0f; *value_i = 0;
    if (AST == NULL) return 0;
    token_btree_t* new = dup_btree(AST, malloc);

    //Replace variables
    for (size_t i = 0; i < num_vars; i++)
    {
        token_t c = *(leftright_vars + i);
        replace_var_to_const(AST, c, i);
    }
    
    //TODO: rest of the function.
    
    mp_freeAST(new, free);
}