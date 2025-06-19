//
// Created by denis on 06.06.2025.
//


#include "../include/inc.h"
#include "../include/plugin_api.h"

static struct option long_options_img[] = {
        {"P",   required_argument,  0,    'P'},
        {"A",   no_argument,        0,    'A'},
        {"O",   no_argument,        0,    'O'},
        {"N",   no_argument,        0,    'N'},
        {"help",   no_argument,     0,    'h'},
        {"version",   no_argument,  0,    'v'},
};

char * get_p_dir(int count, char **args){
    for (int i = 0; i < count; i++){
        if (strcmp(args[i], "-P"))
            continue;

        if (!is_directory(args[i+1]))
            continue;

        return args[i+1];
    }
    return "./lib";
}


static void add_long_option(struct option **opts_ptr, size_t *count_ptr, struct option newopt) {
    // новое число «реальных» элементов:
    size_t old_count = *count_ptr;
    size_t new_count = old_count + 1;

    // делаем realloc на (new_count + 1) записей:
    // +1 — для завершающего {0,0,0,0}
    struct option *tmp = realloc(*opts_ptr, (new_count + 1) * sizeof(struct option));
    if (!tmp) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    *opts_ptr = tmp;

    // копируем newopt на позицию old_count
    (*opts_ptr)[old_count] = newopt;

    // обновляем счётчик реальных элементов
    *count_ptr = new_count;

    // ставим завершающий {0,0,0,0}
    (*opts_ptr)[new_count] = (struct option){ 0, 0, 0, 0 };
}

struct option* get_all_options(char *plugin_dir_path, size_t *count){
    FTS *ftsp;
    FTSENT *entry;
    char *path_t[] = {plugin_dir_path, NULL};

    ftsp = fts_open(path_t, FTS_NOCHDIR | FTS_PHYSICAL, NULL);

    if (!ftsp) {
        print_error_message("fts_open");
    }

    size_t count_core = 0;
    struct option *core_options = NULL;

    for (int i = 0; i < 6; i++){
        add_long_option(&core_options, &count_core, long_options_img[i]);
    }


    while ((entry = fts_read(ftsp)) != NULL) {
        if (entry->fts_info == FTS_F) {
            if (!is_it_so_lib(entry->fts_path))
                continue;

            char *lib_name = strdup(entry->fts_path);

            struct plugin_info pi = {0};

            dlerror();
            void *dl = dlopen(lib_name, RTLD_LAZY);

            if (!dl) {
                fprintf(stderr, "ERROR: dlopen() in %s failed: %s\n", lib_name, dlerror());
                goto END;
            }

            void *func = dlsym(dl, "plugin_get_info");

            if (!func) {
                fprintf(stderr, "ERROR: dlsym() for %s failed: %s\n", lib_name, dlerror());
                goto END;
            }

            typedef int (*pgi_func_t)(struct plugin_info*);
            pgi_func_t pgi_func = (pgi_func_t)func;

            int ret = pgi_func(&pi);
            if (ret < 0) {
                fprintf(stderr, "ERROR: plugin_get_info()  for %s failed\n", lib_name);
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
            if (pi.sup_opts_len == 0) {
                print_error_message("library supports no options! How so?");
                goto END;
            }

            END:
            if (lib_name) free(lib_name);
            if (dl) dlclose(dl);
        }
    }

    if (ftsp)
        fts_close(ftsp);

    *count = count_core;
    return core_options;
}