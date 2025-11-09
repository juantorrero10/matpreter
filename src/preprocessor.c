#include <matpreter.h>


static void remove_spaces(char *str) {
    char *dst = str;
    for (char *src = str; *src; src++) {
        if (!isspace((unsigned char)*src))
            *dst++ = *src;
    }
    *dst = '\0';
}
preprocessor_info_t mp_preprocessor(const char *instruction) {
    preprocessor_info_t pre = {0};
    pre.type = i_definition;  // for now, assume all inputs are definitions
    pre.st = 0;
    memset(pre.info.var_letters, 0, 5);

    // --- 1. Copy and sanitize input ---
    char buffer[256];
    strncpy(buffer, instruction, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    remove_spaces(buffer);

    // --- 2. Split at '=' ---
    char *equals = strchr(buffer, '=');
    if (!equals) {
        WARN("Error: invalid definition (missing '=')");
        pre.st = 1;
        return pre;
    }

    *equals = '\0'; // separate LHS and RHS
    char *lhs = buffer;
    char *rhs = equals + 1;

    // --- 3. Extract function name and parameter list from LHS ---
    // e.g. lhs = "f(x,y)"
    char *paren_open = strchr(lhs, '(');
    char *paren_close = strchr(lhs, ')');
    if (!paren_open && !paren_close) {
        pre.info.var_count = 0;
        goto _return;
    }

    if (!paren_open || !paren_close || paren_close < paren_open) {
        ERROR("invalid function definition, correct structure: foo(x, ...)");
        pre.st = 2;
        return pre;
    }

    // Extract name
    size_t name_len = paren_open - lhs;
    strncpy(pre.info.name, lhs, name_len);
    pre.info.name[name_len] = '\0';

    // --- 4. Extract variable list ---
    char varlist[64];
    strncpy(varlist, paren_open + 1, paren_close - paren_open - 1);
    varlist[paren_close - paren_open - 1] = '\0';

    // Parse variables like "x,y,z"
    int var_count = 0;
    for (char *p = varlist; *p && var_count < MAX_VARIABLE; p++) {
        if (isalpha((unsigned char)*p)) {
            pre.info.variables[var_count].letter = *p;
            pre.info.variables[var_count].id = var_count;
            pre.info.variables[var_count].over = d_real;
            pre.info.var_letters[var_count] = *p;
            var_count++;
        }
    }
    pre.info.var_count = var_count;

    
_return:
    // --- 5. Copy body ---
    pre.body = malloc(strlen(rhs) + 1) ;
    strcpy(pre.body, rhs);

    return pre;
}