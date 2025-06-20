//
// Created by denis on 06.06.2025.
//
//
// Created by denis on 15.05.2025.
//
#include "../include/plugin_api.h"
#include "../include/inc.h"


static char *d_lib_name = "libtest.so";

static char *d_plugin_purpose = "Search for a text substring inside file";

static char *d_plugin_author = "Ilchuk Denis";

#define OPT_FORMAT_STR "test"

static struct plugin_option pl_opt_arr[] = {
        {
                {
                        OPT_FORMAT_STR,
                        required_argument,
                        0, 0,
                },
                "test"
        },
};

static int pl_opt_arr_len = sizeof(pl_opt_arr)/sizeof(pl_opt_arr[0]);


int plugin_get_info(struct plugin_info* ppi) {
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

    if(!fname || !in_opts || in_opts_len == 0){
        errno = EINVAL;
        return -1;
    }

    const char *needle = (const char*)in_opts[0].flag;
    if(!needle)
        return -1;

    FILE *f = fopen(fname, "r");
    if(!f)
        return -1;

    char *line = NULL;
    size_t n = 0;
    int res = 1;
    while(getline(&line, &n, f) != -1){
        if(strstr(line, needle)){
            res = 0;
            break;
        }
    }
    free(line);
    fclose(f);
    return res;
}