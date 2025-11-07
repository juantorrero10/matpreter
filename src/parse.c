#include <matpreter.h>

static void print_ppi_info(preprocessor_info_t ppi) {
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


static const char* token_type_str(token_cathegory_t t) {
    switch (t) {
        case INVALID:           return "INVALID";
        case OPERATION_ADD:     return "'+' ADD";
        case OPERATION_SUB:     return "'-' SUB";
        case OPERATION_MUL:     return "'*' MUL";
        case OPERATION_DIV:     return "'/' DIV";
        case OPERATION_EXP:     return "'^' EXP";
        case PARENTHESIS_OPEN:  return "'(' P/OPEN";
        case PARENTHESIS_CLOSE: return "')' P/CLOSE";
        case LITERAL_CONSTANT:  return "LITERAL/CONST";
        case LITERAL_VARIABLE:  return "LITERAL/VAR";
        default:                return "UNKNOWN";
    }
}

static void print_tokens(token_array_t arr, preprocessor_info_t ppi) {
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
                DUMP("[%02zu] '%c'-> %-15s", i, ppi.info.variables[t->value_i].letter, token_type_str(t->type));
                break;
            case LITERAL_CONSTANT:
                DUMP("[%02zu] %g -> %-15s", i, t->value_r, token_type_str(t->type));
                break;
            default:
                DUMP("[%02zu] %-15s", i, token_type_str(t->type));
                break;
        }
    }
    SEP();
}

static const char* token_symbol(token_cathegory_t type) {
    switch (type) {
        case OPERATION_ADD: return "+";
        case OPERATION_SUB: return "-";
        case OPERATION_MUL: return "*";
        case OPERATION_DIV: return "/";
        case OPERATION_EXP: return "^";
        case PARENTHESIS_OPEN: return "(";
        case PARENTHESIS_CLOSE: return ")";
        case LITERAL_CONSTANT: return "CONST";
        case LITERAL_VARIABLE: return "VAR";
        case LITERAL_LAMBDA: return "FUNC";
        case INVALID: return "INVALID";
        default: return "?";
    }
}

/**
 * @brief Print a token node content in readable form.
 */
static void print_token(const token_t *t) {
    if (!t) {
        printf("(null)");
        return;
    }

    switch (t->type) {
        case LITERAL_CONSTANT:
            printf("%g", t->value_r);
            break;
        case LITERAL_VARIABLE:
            printf("%c", (char)t->value_i);
            break;
        default:
            printf("%s", token_symbol(t->type));
            break;
    }
}

/**
 * @brief Recursively print the AST sideways (right branch up).
 * @param node pointer to the binary tree node
 * @param depth indentation depth for visualization
 */
void print_ast(const token_btree_t *node, int depth) {
    if (!node) return;

    // Print right subtree first (so tree grows upward)
    print_ast(node->rhs, depth + 1);

    // Print current node
    for (int i = 0; i < depth; i++) printf("    ");  // indent 4 spaces per level
    print_token(&node->root);
    printf("\n");

    // Then left subtree
    print_ast(node->lhs, depth + 1);
}

void ast_to_dot(const token_btree_t *node, FILE *out) {
    if (!node) return;
    fprintf(out, "  n%p [label=\"", (void*)node);
    print_token(&node->root);
    fprintf(out, "\"];\n");

    if (node->lhs) {
        fprintf(out, "  n%p -> n%p;\n", (void*)node, (void*)node->lhs);
        ast_to_dot(node->lhs, out);
    }
    if (node->rhs) {
        fprintf(out, "  n%p -> n%p;\n", (void*)node, (void*)node->rhs);
        ast_to_dot(node->rhs, out);
    }
}

void export_ast_dot(const token_btree_t *root, const char *filename) {
    FILE *out = fopen(filename, "w");
    if (!out) return;
    fprintf(out, "digraph AST {\n  node [shape=circle];\n");
    ast_to_dot(root, out);
    fprintf(out, "}\n");
    fclose(out);
}
errcode mp_parse(const char* instruction) {
    INFO("Parse received %s", instruction);
    preprocessor_info_t ppi = mp_preprocessor(instruction);
    if (ppi.st != 0) {ERROR_INFO("mp_preprocessor", ppi.st); return 1;}
    print_ppi_info(ppi);
    token_array_t a = mp_tokenize(ppi);
    if (a.st != 0) {ERROR_INFO("mp_tokenize", a.st); return 1;}
    print_tokens(a, ppi);

    errcode e = 0;
    token_btree_t* b = mp_createAST(a, &e);
    print_ast(b, 0);
    free(a.ptr);
    free(ppi.body);
    return 0;
}