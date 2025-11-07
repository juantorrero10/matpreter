#include <mpdisplay.h>

void mpd_print_ppi_info(preprocessor_info_t ppi) {
    SEP();
    INFO("Preprocessor returned: ");
    DUMP("FUNC NAME: %s", ppi.info.name);
    DUMP("VARIABLES: amount -> %u", ppi.info.var_count);
    printf("    ");
    for (size_t i = 0; i < ppi.info.var_count; i++)
    {
        printf("%c", ppi.info.variables[i].letter);
        if (i != ppi.info.var_count-1) {printf(", ");}
    }
    NL();
    DUMP("Body: %s", ppi.body);
    SEP();
    
}


static const char* mpd_token_type_str(token_category_t t, _bool extended) {
    switch (t) {
        case INVALID:           return (extended)? "INVALID" : "#";
        case OPERATION_ADD:     return (extended)? "'+' ADD" : "+";
        case OPERATION_SUB:     return (extended)? "'-' SUB" : "-";
        case OPERATION_MUL:     return (extended)? "'*' MUL" : "*";
        case OPERATION_DIV:     return (extended)? "'/' DIV" : "/";
        case OPERATION_EXP:     return (extended)? "'^' EXP" : "^";
        case PARENTHESIS_OPEN:  return (extended)? "'(' P/OPEN" : "(";
        case PARENTHESIS_CLOSE: return (extended)? "')' P/CLOSE" : ")";
        case LITERAL_CONSTANT:  return (extended)? "LITERAL/CONST" : "C";
        case LITERAL_VARIABLE:  return (extended)? "LITERAL/VAR" : "V";
        default:                return (extended)? "UNKNOWN" : "?";
    }
}

void mpd_print_tokens(token_array_t arr, preprocessor_info_t ppi) {
        if (!arr.ptr || arr.sz == 0) {
        INFO("No tokens to display.");
        return;
    }
    SEP();
    INFO("Tokens created (%zu tokens):", arr.sz);
    for (size_t i = 0; i < arr.sz; i++) {
        token_t *t = &arr.ptr[i];

        switch (t->type) {
            case LITERAL_VARIABLE:
                DUMP("[%02zu] '%c'-> %-15s", i, ppi.info.variables[t->value_i].letter, mpd_token_type_str(t->type, 1));
                break;
            case LITERAL_CONSTANT:
                DUMP("[%02zu] %g -> %-15s", i, t->value_r, mpd_token_type_str(t->type, 1));
                break;
            default:
                DUMP("[%02zu] %-15s", i, mpd_token_type_str(t->type, 1));
                break;
        }
    }
    SEP();
}

/**
 * @brief Print singular token node content in readable form.
 */
void mpd_print_token(const token_t *t, preprocessor_info_t ppi) {
    if (!t) {
        LOG("(null)");
        return;
    }

    switch (t->type) {
        case LITERAL_CONSTANT:
            LOG("%g", t->value_r);
            break;
        case LITERAL_VARIABLE:
            LOG("%c", ppi.info.var_letters[t->value_i]);
            break;
        default:
            LOG("%s", mpd_token_type_str(t->type, 0));
            break;
    }
}

/**
 * @brief Recursively print the AST sideways (right branch up).
 * @param node pointer to the binary tree node
 * @param depth indentation depth for visualization
 */
void mpd_print_ast(const token_btree_t *node, int depth, preprocessor_info_t ppi) {
    if (!node) return;

    // Print right subtree first (so tree grows upward)
    mpd_print_ast(node->rhs, depth + 1, ppi);

    // Print current node
    for (int i = 0; i < depth; i++) LOG("    ");  // indent 4 spaces per level
    mpd_print_token(&node->root, ppi);
    LOG("\n");

    // Then left subtree
    mpd_print_ast(node->lhs, depth + 1, ppi);
}

void mpd_ast_to_dot(const token_btree_t *node, FILE *out, preprocessor_info_t ppi) {
    if (!node) return;
    fprintf(out, "  n%p [label=\"", (void*)node);
    mpd_print_token(&node->root, ppi);
    fprintf(out, "\"];\n");

    if (node->lhs) {
        fprintf(out, "  n%p -> n%p;\n", (void*)node, (void*)node->lhs);
        mpd_ast_to_dot(node->lhs, out, ppi);
    }
    if (node->rhs) {
        fprintf(out, "  n%p -> n%p;\n", (void*)node, (void*)node->rhs);
        mpd_ast_to_dot(node->rhs, out, ppi);
    }
}

void mpd_export_ast_dot(const token_btree_t *root, const char *filename, preprocessor_info_t ppi) {
    FILE *out = fopen(filename, "w");
    if (!out) return;
    fprintf(out, "digraph AST {\n  node [shape=circle];\n");
    mpd_ast_to_dot(root, out, ppi);
    fprintf(out, "}\n");
    fclose(out);
}
