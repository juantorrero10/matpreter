#include <matpreter.h>
#include <mpdisplay.h>

token_btree_t* mp_parse(const char* instruction) {
    INFO("Parse received %s", instruction);

    preprocessor_info_t ppi = mp_preprocessor(instruction);
    if (ppi.st != 0) {ERROR_INFO("mp_preprocessor", ppi.st); return 1;}
    mpd_print_ppi_info(ppi);

    token_array_t a = mp_tokenize(ppi);
    if (a.st != 0) {ERROR_INFO("mp_tokenize", a.st); return 1;}
    mpd_print_tokens(a, ppi);

    errcode e = 0;
    token_btree_t* b = mp_createAST(a, &e);
    mpd_print_ast(b, 0, ppi);

    OKAY("Refresh my memory: %s", instruction);

    free(a.ptr);
    free(ppi.body);
    return b;
}