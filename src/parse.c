#include <matpreter.h>
#include <mpdisplay.h>

void mp_free_func(function_t* f, void (*deallocator)(void*)) {
    if (f->body != NULL)mp_freeAST(f->body, deallocator);
    if (f->ppi.body != NULL) deallocator(f->ppi.body);
    f->body = NULL;
    f->ppi.body = NULL;
}

function_t mp_parse(const char* instruction) {
    INFO("Parse received %s", instruction);
    function_t f = {0};
    preprocessor_info_t ppi = mp_preprocessor(instruction);
    if (ppi.st != 0) {ERROR_INFO("mp_preprocessor", ppi.st); return f;}
    mpd_print_ppi_info(ppi);

    token_array_t a = mp_tokenize(ppi);
    if (a.st != 0) {ERROR_INFO("mp_tokenize", a.st); return f;}
    //mpd_print_tokens(a, ppi);

    errcode e = 0;
    token_btree_t* b = mp_createAST(a, &e, ppi, 1);
    if (e > 0) {
        ERROR_INFO("mp_createAST", e-1);
        return f;
    }
    f = (function_t){.body = b, .ppi = ppi};

    free(a.ptr);
    return f;
}