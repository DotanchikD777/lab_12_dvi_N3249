//
// Created by denis on 15.05.2025.
//
#include "../include/plugin_api.h"
#include "../include/inc.h"


static char *d_lib_name = "libpic.so";

static char *d_plugin_purpose = "Check if the file contains an image";

static char *d_plugin_author = "Ilchuk Denis";

#define OPT_FORMAT_STR "pic"

static struct plugin_option pl_opt_arr[] = {
        {
                {
                    OPT_FORMAT_STR,
                    required_argument,
                    0, 0,
                },
                "specify formats to find"
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

int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) { //TODO Need to finish

    return 0;
}