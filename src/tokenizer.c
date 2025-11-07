#include <matpreter.h>

void mp_token_append(token_list_t *l, token_t t, size_t *occupied, size_t *allocated) {
    if (*allocated == 0) {
        *allocated = 8; // initial capacity
        *l = malloc((*allocated) * sizeof(token_t));
        if (!*l) { ERROR_INFO("malloc", 1); exit(1); }
    } else if (*occupied >= *allocated) {
        *allocated *= 2;
        *l = realloc(*l, (*allocated) * sizeof(token_t));
        if (!*l) { ERROR_INFO("realloc", 1); exit(1); }
    } else if (*occupied > *allocated) {
        ERROR("Memory wrong");
    }

    memcpy((*l) + (*occupied)++, &t, sizeof(token_t));
}

/**
 * A tokenizer through a state machine
 * @param ppi A valid preprocessor info struct
 * @return the list of tokens
 */
token_array_t mp_tokenize(preprocessor_info_t ppi) {
    INFO("tokenize received %s", ppi.body);

    token_array_t a = {0};
    token_list_t list = NULL;

    // Init variables
    size_t allocated = 0;
    size_t occupied = 0;
    char *ptr = ppi.body;
    char number[32];
    int8_t parenthesis_weight = 0;
    _bool left_add_mul = 0;

    //Implicit multiplication token e.g 7x -> 7, '*', 'x'
    token_t mul_token = {OPERATION_MUL, 0, 0, NULL, 0};
    mul_token.type = OPERATION_MUL;

Init:
    memset(number, 0, sizeof(number));
    token_t curr = {INVALID, 0, 0, NULL, 0};

    switch (*ptr) {
        case 0: goto exit;
        case '+': left_add_mul=0;curr.type = OPERATION_ADD; ptr++; mp_token_append(&list, curr, &occupied, &allocated); goto Init;
        case '-': left_add_mul=0;curr.type = OPERATION_SUB; ptr++; mp_token_append(&list, curr, &occupied, &allocated); goto Init;
        case '/': left_add_mul=0;curr.type = OPERATION_DIV; ptr++; mp_token_append(&list, curr, &occupied, &allocated); goto Init;
        case '*': left_add_mul=0;curr.type = OPERATION_MUL; ptr++; mp_token_append(&list, curr, &occupied, &allocated); goto Init;
        case '^': left_add_mul=0;curr.type = OPERATION_EXP; ptr++; mp_token_append(&list, curr, &occupied, &allocated); goto Init;

        case '(':
            curr.type = PARENTHESIS_OPEN;
            if (left_add_mul) mp_token_append(&list, mul_token, &occupied, &allocated);
            left_add_mul = 0;
            parenthesis_weight++;
            ptr++; mp_token_append(&list, curr, &occupied, &allocated); goto Init;

        case ')':
            curr.type = PARENTHESIS_CLOSE;
            
            parenthesis_weight--;
            ptr++; mp_token_append(&list, curr, &occupied, &allocated); goto Init;

        default:
            if (isdigit(*ptr) || *ptr == '.') {
                if (left_add_mul) {
                    mp_token_append(&list, mul_token, &occupied, &allocated);
                }
                left_add_mul = 0;
                goto number;
            } else if (isalpha(*ptr)) {
                if (left_add_mul) mp_token_append(&list, mul_token, &occupied, &allocated);
                left_add_mul = 0;
                goto variable;
            } else {
                goto unexpected_token;
            }
    }

variable:
    curr.type = LITERAL_VARIABLE;
    {
        char *t = strchr(ppi.info.var_letters, *ptr);
        if (!t) goto unexpected_token;
        uint64_t idx = (uint64_t)(t - ppi.info.var_letters);
        curr.value_i = ppi.info.variables[idx].id;
    }
    left_add_mul = 1;
    mp_token_append(&list, curr, &occupied, &allocated);
    ptr++; goto Init;

number: {
    curr.type = LITERAL_CONSTANT;
    size_t len = 0;
    while (isdigit(*ptr) || *ptr == '.') {
        if (len < sizeof(number) - 1) number[len++] = *ptr;
        ptr++;
    }
    char *endptr;
    curr.value_r = strtod(number, &endptr);
    if (isnan(curr.value_r) || isinf(curr.value_r)) goto number_error;
    left_add_mul = 1;
    mp_token_append(&list, curr, &occupied, &allocated);
    goto Init;
}

number_error:
    ERROR("%s -> \"%s\": not a valid number", ppi.body, number);
    a.st = 1; goto exit;

unexpected_token:
    ERROR("%s -> \"%c\": unexpected token", ppi.body, *ptr);
    a.st = 2; goto exit;

exit:
    if (parenthesis_weight > 0) {
        ERROR("%s -> ')' expected", ppi.body);
        a.st = 3;
    } else if (parenthesis_weight < 0) {
        ERROR("%s -> '(' expected", ppi.body);
        a.st = 4;
    }

    a.ptr = list;
    a.sz = occupied;
    return a;
}
