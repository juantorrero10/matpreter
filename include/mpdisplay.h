/**
 * Optional header with utilities for displaying tokens and token structures.
 */
#ifndef MPDISPLAY_H_
#define MPDISPLAY_H_

#include <matpreter.h>

void mpd_print_ppi_info(preprocessor_info_t ppi);
void mpd_print_tokens(token_array_t arr, preprocessor_info_t ppi);
void mpd_print_token(const token_t *t, preprocessor_info_t ppi);
void mpd_print_ast(const token_btree_t *node, int depth, preprocessor_info_t ppi);
void mpd_ast_to_dot(const token_btree_t *node, FILE *out, preprocessor_info_t ppi);
void mpd_export_ast_dot(const token_btree_t *root, const char *filename, preprocessor_info_t ppi);

#endif // MPDISPLAY_H_