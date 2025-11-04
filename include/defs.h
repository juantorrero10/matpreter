#ifndef DEFS_H_
#define DEFS_H_

#include "./macros.h"

/*-------------STRUCTS/ENUMS------------------*/
typedef int64_t errcode;
enum instruction_t {
    i_definition = 0,
    i_calculation = 1
};

enum defined_over {
    d_real = 0,
    d_integer = 1
};

typedef struct {
    char letter;              // variable name, e.g. 'x'
    uint32_t id;              // index or unique ID
    enum defined_over over;   // type (real/int)
} variable_t;

typedef struct {
    char name[32];                       // function name, e.g. "f"
    variable_t variables[MAX_VARIABLE];  // support up to 4 parameters
    char var_letters[MAX_VARIABLE + 1];  // Redundant, to easen calcualtions
    uint32_t var_count;
} definition_info_t;

typedef struct {
    enum instruction_t type;
    definition_info_t info;
    char *body;               // body expression, e.g. "7x+3"
    errcode st;
} preprocessor_info_t;

//Tokenizer
typedef enum _token_cathegory_enum {
    INVALID,
    PARENTHESIS_OPEN,
    PARENTHESIS_CLOSE,
    OPERATION_ADD,
    OPERATION_SUB,
    OPERATION_MUL,
    OPERATION_DIV,
    LITERAL_CONSTANT,
    LITERAL_VARIABLE,
    LITERAL_LAMBDA
}token_cathegory_t;


typedef struct _token_type {
    //Tokenizer job
    token_cathegory_t type;
    uint64_t value_i;
    double value_r;
    //Parser's job
    struct _token_type** conections;
    uint64_t connected_to;
}token_t;

typedef token_t* token_list_t;

typedef struct _token_info_return_type {
    token_list_t ptr;
    size_t sz;
    errcode st;
}token_array_t;

/*-------------FUNCTIONS-----------------*/

errcode mp_parse(const char* instruction);
token_array_t mp_tokenize(preprocessor_info_t ppi);
preprocessor_info_t mp_preprocessor(const char *instruction);


#endif // DEFS_H_
