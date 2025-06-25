#include "../include/plugin_api.h"
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>


static char *d_lib_name = "libdlsyms.so";

static char *d_plugin_purpose = "Search dynamic libraries containing specified symbols";

static char *d_plugin_author = "Pivovarov Constantin";

#define OPT_FORMAT_STR "dl-syms"

#define PNG 0
#define JPG 1
#define BMP 2
#define GIF 3

static struct plugin_option pl_opt_arr[] = {
        {
                {
                        OPT_FORMAT_STR,
                        required_argument,
                        0, 0,
                },
                "specify symbols to find (comma-separated)"
        },
};

static int pl_opt_arr_len = sizeof(pl_opt_arr) / sizeof(pl_opt_arr[0]);

/**
 * Parse a comma-separated list of symbols.
 * On success, returns a heap-allocated array of char* in *out_syms and sets *out_count;
 * On error, returns NULL.
*/
static char **parse_symbols(const char *str, int *out_count) {
    if (!str || !out_count) return NULL;

    char *buf = strdup(str);
    if (!buf) return NULL;

    int count = 0;
    char **syms = NULL;
    char *tok = strtok(buf, ",");
    while (tok) {
        // allocate or grow array
        char **tmp = realloc(syms, (count + 1) * sizeof(char *));
        if (!tmp) {
            // cleanup on error
            for (int i = 0; i < count; i++) free(syms[i]);
            free(syms);
            free(buf);
            return NULL;
        }
        syms = tmp;
        syms[count++] = strdup(tok);
        tok = strtok(NULL, ",");
    }
    free(buf);

    if (count < 1) {
        // nothing to search
        for (int i = 0; i < count; i++) free(syms[i]);
        free(syms);
        return NULL;
    }

    *out_count = count;
    return syms;
}


int plugin_get_info(struct plugin_info *ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: in plugin %s: invalid argument\n", d_lib_name);
        return -1;
    }

    ppi->plugin_purpose = d_plugin_purpose;
    ppi->plugin_author = d_plugin_author;
    ppi->sup_opts_len = pl_opt_arr_len;
    ppi->sup_opts = pl_opt_arr;

    return 0;
}

int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {


    bool DEBUG = getenv("LAB12DEBUG") != NULL;


    /* Проверка аргументов */
    if (!fname || !in_opts || in_opts_len < 1) {
        errno = EINVAL;
        return -1;
    }

    if (DEBUG) {
        fprintf(stderr, "DEBUG: %s: Processing file %s\n", d_lib_name, fname);
        fprintf(stderr, "DEBUG: %s: Got option '%s' with arg '%s'\n",
                d_lib_name, in_opts[0].name, (char *)in_opts[0].flag);
    }

    /* Разбор списка символов */
    int sym_count = 0;
    char **symbols = parse_symbols((const char *)in_opts[0].flag, &sym_count);
    if (!symbols || sym_count < 1) {
        if (DEBUG)
            fprintf(stderr, "DEBUG: %s: bad symbol list: '%s'\n",
                    d_lib_name, (char *)in_opts[0].flag);
        errno = EINVAL;
        return -1;
    }

    /* Попытка открыть файл как динамическую библиотеку */
    void *handle = dlopen(fname, RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        /* Не библиотека или не удаётся открыть — считаем, что не соответствует */
        if (DEBUG)
            fprintf(stderr, "DEBUG: %s: dlopen failed on %s: %s\n",
                    d_lib_name, fname, dlerror());
        /* Освобождаем память */
        for (int i = 0; i < sym_count; i++) free(symbols[i]);
        free(symbols);
        return 1;
    }

    /* Проверяем каждый символ */
    for (int i = 0; i < sym_count; i++) {
        if (dlsym(handle, symbols[i]) == NULL) {
            if (DEBUG)
                fprintf(stderr, "DEBUG: %s: symbol '%s' not found in %s\n",
                        d_lib_name, symbols[i], fname);
            dlclose(handle);
            for (int j = 0; j < sym_count; j++) free(symbols[j]);
            free(symbols);
            return 1;
        }
    }

    /* Всё найдено — файл соответствует */
    dlclose(handle);
    for (int i = 0; i < sym_count; i++) free(symbols[i]);
    free(symbols);
    return 0;
}