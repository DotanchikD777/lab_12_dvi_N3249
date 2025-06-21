//
// Created by denis on 06.06.2025.
//


#include "../include/inc.h"
#include "../include/plugin_api.h"

static struct option long_options_img[] = {
        {"plugins-dir", required_argument, 0, 'P'},
        {"and",         no_argument,       0, 'A'},
        {"or",          no_argument,       0, 'O'},
        {"not",         no_argument,       0, 'N'},
        {"help",        no_argument,       0, 'h'},
        {"version",     no_argument,       0, 'v'},
};

char *get_p_dir(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-P") == 0) {
            if (i + 1 >= argc)
                print_error_message("-P requires directory argument");
            if (!is_directory(argv[i + 1]))
                print_error_message("-P argument is not a directory");
            return argv[i + 1];
        }
    }
    return "./lib";
}


static void
add_long_option(struct option **opts_ptr, size_t *count_ptr, struct option newopt) { // add element to options massive
    // check for duplicate names
    for (size_t i = 0; i < *count_ptr; i++) {
        if ((*opts_ptr)[i].name && newopt.name &&
            strcmp((*opts_ptr)[i].name, newopt.name) == 0)
            print_error_message("duplicate option detected");
    }
    size_t old_count = *count_ptr;
    size_t new_count = old_count + 1;


    struct option *tmp = realloc(*opts_ptr, (new_count + 1) * sizeof(struct option));
    if (!tmp)
        print_error_message("can`t realloc memory");

    *opts_ptr = tmp;

    (*opts_ptr)[old_count] = newopt;

    *count_ptr = new_count;

    (*opts_ptr)[new_count] = (struct option) {0, 0, 0, 0}; // ending element
}

struct option *get_all_options(char *plugin_dir_path, size_t *count) {
    FTS *ftsp;
    FTSENT *entry;
    char *path_t[] = {plugin_dir_path, NULL};

    ftsp = fts_open(path_t, FTS_NOCHDIR | FTS_PHYSICAL, NULL);

    if (!ftsp)
        print_error_message("fts can`t open plugin directory");

    size_t count_core = 0;
    struct option *core_options = NULL;

    for (int i = 0; i < 6; i++) // add core options
        add_long_option(&core_options, &count_core, long_options_img[i]);


    while ((entry = fts_read(ftsp)) != NULL) { // scan dir and add options
        if (entry->fts_info != FTS_F)
            continue;

        if (!is_it_so_lib(entry->fts_path))
            continue;

        char *lib_name = strdup(entry->fts_path);

        struct plugin_info pi = {0};

        dlerror();
        void *dl = dlopen(lib_name, RTLD_LAZY);

        if (!dl) {
            fprintf(stderr, "Error: dlopen() in %s failed: %s\n", lib_name, dlerror());
            goto END;
        }

        void *func = dlsym(dl, "plugin_get_info");

        if (!func) {
            fprintf(stderr, "Error: dlsym() for %s failed: %s\n", lib_name, dlerror());
            goto END;
        }

        typedef int (*pgi_func_t)(struct plugin_info *);
        pgi_func_t pgi_func = (pgi_func_t) func;

        int ret = pgi_func(&pi);
        if (ret < 0) {
            fprintf(stderr, "Error: plugin_get_info()  for %s failed\n", lib_name);
            goto END;
        }

        // Plugin info
        if (pi.sup_opts_len > 0)
            for (size_t i = 0; i < pi.sup_opts_len; i++) {
                struct option tmp = pi.sup_opts[i].opt;
                if (tmp.name)
                    tmp.name = strdup(tmp.name);
                add_long_option(&core_options, &count_core, tmp);
            }
        else
            printf("none (!?)\n");

        if (pi.sup_opts_len == 0)
            print_error_message("library supports no options! How so?");

        END:
        if (lib_name) free(lib_name);
        if (dl) dlclose(dl);
    }

    if (ftsp)
        fts_close(ftsp);

    *count = count_core;
    return core_options;
}