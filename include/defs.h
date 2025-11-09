#ifndef DEFS_H_
#define DEFS_H_

#include "./macros.h"


typedef uint8_t _bool;

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
    errcode st;               // return errorcode from preprocessor, unused otherwise
} preprocessor_info_t;

/*-------------TOKENIZER---------------*/

typedef enum _token_category_enum {
    //Special
    INVALID = TC_START_IDX,
    PARENTHESIS_OPEN,
    PARENTHESIS_CLOSE,
    //Oepr
    OPERATION_ADD = TC_OPERATIONS_IDX,
    OPERATION_SUB,
    OPERATION_MUL,
    OPERATION_DIV,
    OPERATION_EXP,
    LITERAL_CONSTANT = TC_LITERALS_IDX,
    LITERAL_VARIABLE,
    LITERAL_LAMBDA
}token_category_t;


typedef struct _token_type {
    token_category_t type;
    int64_t value_i;
    double value_r;
    uint64_t id;
}token_t;

typedef token_t* token_list_t;

typedef struct _token_info_return_type {
    token_list_t ptr;
    size_t sz;
    size_t allocated;
    errcode st;         //return errorcode from tokenizer, unused otherwise
}token_array_t;

typedef struct _token_binary_tree_ast {
    token_t root;
    struct _token_binary_tree_ast* lhs;
    struct _token_binary_tree_ast* rhs;
}token_btree_t;

typedef struct _function_def {
    token_btree_t *body;
    preprocessor_info_t ppi;
}function_t;

/*-------------FUNCTIONS-----------------*/

/*-------------------MAIN CHAIN----------------------------------*/
function_t mp_parse(const char* instruction);/*˥
                   _________________________˩
                  ꜒
                  ↓                                                                   */
preprocessor_info_t mp_preprocessor(const char *instruction);
/*                ꜖__
                     ˥
                     ↓                                                                */
token_array_t mp_tokenize(preprocessor_info_t ppi);
/*                   ꜖___________________________________________________
                                                                         ˥            
                                                                         ↓              */
token_btree_t* mp_createAST(token_array_t array, errcode* control, preprocessor_info_t ppi, _bool b_firstIter);

/*--------Additional------------------------*/
void mp_token_append(token_list_t *l, token_t t, size_t *occupied, size_t *allocated);
void mp_freeAST(token_btree_t* tree, void (*deallocator) (void*));
errcode mp_evaluateAST(token_btree_t** AST, token_list_t leftright_vars, size_t num_vars,
        _out_ double* value_d, _out_ int64_t* value_i) ;
const char* mpd_token_type_str(token_category_t t, _bool extended);
char mpd_char_token(const token_t *t, preprocessor_info_t ppi);
void mp_free_func(function_t* f, void (*deallocator)(void*));



#endif // DEFS_H_
