#include <matpreter.h>

void print_ppi_info(preprocessor_info_t ppi) {
    SEP();
    INFO("Preprocessor returned: ");
    DUMP("FUNC NAME: %s", ppi.info.name);
    DUMP("VARIABLES: amount -> %lu", ppi.info.var_count);
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


const char* token_type_str(token_cathegory_t t) {
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

print_tokens(token_array_t arr, preprocessor_info_t ppi) {
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
                DUMP("[%02zu] '%c'-> %-15s", i, (unsigned long long)ppi.info.variables[t->value_i].letter, token_type_str(t->type));
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

errcode mp_parse(const char* instruction) {
    INFO("Parse received %s", instruction);
    preprocessor_info_t ppi = mp_preprocessor(instruction);
    if (ppi.st != 0) {ERROR_INFO("mp_preprocessor", ppi.st); return 1;}
    print_ppi_info(ppi);
    token_array_t a = mp_tokenize(ppi);
    if (a.st != 0) {ERROR_INFO("mp_tokenize", a.st); return 1;}
    print_tokens(a, ppi);
    free(a.ptr);
    free(ppi.body);
    return 0;
}