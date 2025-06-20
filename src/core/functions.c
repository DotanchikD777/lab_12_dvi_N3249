//
// Created by denis on 15.05.2025.
//

#include "../include/inc.h"
#include "../include/plugin_api.h"



int f_argc = 0;
char **f_argv = NULL;
static bool DEBUG;
char *dir_for_scan;

void get_terminal_arguments_from_main_to_functions (int argc, char *argv[], char *dir_for_scan_path){
    f_argc = argc;
    f_argv = argv;
    dir_for_scan = dir_for_scan_path;
    if (DEBUG)
        printf("\nDebug: get command args to functions file\n");

}

void get_debug_status_mode_functions (bool flag){
    if(flag){
        DEBUG = true;
    }
}


bool is_directory(const char *path) {
    struct stat sb;

    if (stat(path, &sb) != 0) {
        return false;
    }

    // Проверим, что это именно директория
    return S_ISDIR(sb.st_mode);
}



void print_standart_message(char flag){
    switch (flag) {
        case 'h':
            printf("\n%s\n",STRIPE);
            printf(
                    "Usage: lab11dviN3249 [OPTIONS] [DIRECTORY]\n"
                    "Perform a recursive file search starting from DIRECTORY (defaults to current working directory),\n"
                    "using dynamically loaded plugins. If DIRECTORY is omitted, display this help and list available plugins.\n"
                    "\n"
                    "Options:\n"
                    "  -P, --plugins-dir DIR\n"
                    "        Look for plugins in directory DIR instead of the executable’s directory.\n"
                    "  -A, --and\n"
                    "        Combine multiple search criteria with logical AND (this is the default).\n"
                    "  -O, --or\n"
                    "        Combine multiple search criteria with logical OR.\n"
                    "  -N, --not\n"
                    "        Invert the result of the combined search criteria after applying AND/OR.\n"
                    "  -v, --version\n"
                    "        Print program version, student’s full name, group number, and variant; then exit.\n"
                    "  -h, --help\n"
                    "        Display this help message and exit.\n"
                    "\n"
                    "Plugin-provided Options:\n"
                    "  The utility loads all \".so\" plugins from the plugins directory. Each plugin may define\n"
                    "  one or more long options (e.g. --crc16, --mac-addr, --exe, etc.) to specify file-matching\n"
                    "  criteria. To see the full list of plugin options and their descriptions, run with -h after\n"
                    "  ensuring your plugins directory is non-empty.\n"
                    "\n"
                    "Environment Variables:\n"
                    "  LAB1DEBUG\n"
                    "        If set (to any value), debug output is written to stdout.\n"
                    "\n"
                    "Exit Status:\n"
                    "  0    At least one matching file was found, or help/version was printed successfully.\n"
                    "  1    No files matched the given criteria.\n"
                    "  >1   An error occurred (e.g. invalid option, plugin load failure, directory traversal error).\n"
                    "\n"
                    "Version:\n"
                    "  0.5 (18 March 2025)\n"
            );
            printf("\n%s\n",STRIPE);
            break;
        case 'v':
            printf("\n%s\nCreated by: %s\nGroup: %s\nVariant: %s\n%s\n",
                             STRIPE,         NAME,      GR,          VR, STRIPE);


            break;
        default:

            break;

    }

}

int yes_or_no(const char *input) {
    enum { BUF_SIZE = 16 };
    char buf[BUF_SIZE];

    printf("%s [y/N]: ", input);
    if (!fgets(buf, BUF_SIZE, stdin))
        return 0;

    if (!strchr(buf, '\n')) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    return (buf[0] == 'y' || buf[0] == 'Y');
}

bool is_it_so_lib(const char *path) {
    // 1) Найдём имя файла (после последнего '/' )
    const char *name = strrchr(path, '/');
    if (name) name++;  // если нашли '/', переходим за него
    else      name = path;

    // 2) Проверим длину: она должна быть >= strlen("lib") + strlen(".so")
    size_t len = strlen(name);
    if (len < 3 + 3)  // 3 символа в "lib" + 3 в ".so"
        return false;

    // 3) Проверим префикс "lib"
    if (name[0] != 'l' || name[1] != 'i' || name[2] != 'b')
        return false;

    // 4) Проверим суффикс ".so"
    if (name[len-3] != '.' || name[len-2] != 's' || name[len-1] != 'o')
        return false;

    // Всё ок
    return true;
}

int scan_dir_for_dynamic_lib_options_if_user_provide_no_dir_for_scan_via_dynamic_lib(const char *fpath, const struct stat *sb, int typeflag){
    switch (typeflag) {
        case FTW_D:
            return 0;
            break;
        case FTW_F:
            if (!is_it_so_lib(fpath))
                return 0;

            char *lib_name = strdup(fpath);

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
            printf("\n%s\nYou can add plugin: %s\n", STRIPE, lib_name);
            printf("Plugin purpose:\t\t%s\n", pi.plugin_purpose);
            printf("Plugin author:\t\t%s\n", pi.plugin_author);
            printf("Supported options: ");
            if (pi.sup_opts_len > 0) {
                printf("\n");
                for (size_t i = 0; i < pi.sup_opts_len; i++) {
                    printf("\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
                }
            }
            else {
                printf("none (!?)\n");
            }
            if (pi.sup_opts_len == 0) {
                print_error_message("library supports no options! How so?");
                goto END;
            }
            printf("\n%s\n", STRIPE);


            END:
            if (lib_name) free(lib_name);
            if (dl) dlclose(dl);
            return 0;
            break;
        default:
            return 0;
            break;
    }
}

void add_string_to_global(const char *new_string)
{
    // Расширяем массив на один элемент
    char **temp = realloc(global_maches, (global_maches_len + 1) * sizeof(char *));

    if (temp == NULL)
    {
        fprintf(stderr, "\nОшибка: не удалось выделить память\n");
        exit(EXIT_FAILURE);
    }
    global_maches = temp;

    // Создаем копию строки и сохраняем её в новом элементе массива
    global_maches[global_maches_len] = strdup(new_string);
    if (global_maches[global_maches_len] == NULL)
    {
        fprintf(stderr, "\nОшибка: не удалось скопировать сторку\n");
        exit(EXIT_FAILURE);
    }

    // Увеличиваем счетчик строк
    global_maches_len++;
}

int scan_dir_via_dynamic_lib_or_libs_for_matches(const char *fpath, const struct stat *sb, int typeflag){ //TODO Need to fix parsing options
    switch (typeflag) {
        case FTW_D:
            return 0;
            break;
        case FTW_F:
            if (!is_it_so_lib(fpath))
                return 0;

            int opts_to_pass_len = 0;
            struct option *opts_to_pass = NULL;
            struct option *longopts = NULL;

            char *lib_name = strdup(fpath);

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

            if (pi.sup_opts_len == 0) {
                print_error_message("library supports no options! How so?");
                goto END;
            }

            if (DEBUG){
                printf("\nDEBUG: successfully added plugin %s\n", lib_name);
            }

            func = dlsym(dl, "plugin_process_file");
            if (!func) {
                fprintf(stderr, "ERROR: no plugin_process_file() function found\n");
                goto END;
            }

            typedef int (*ppf_func_t)(const char*, struct option*, size_t);
            ppf_func_t ppf_func = (ppf_func_t)func;

            longopts = calloc(pi.sup_opts_len + 1, sizeof(struct option));
            if (!longopts) {
                print_error_message("calloc() failed\nn");
                goto END;
            }

            for (size_t i = 0; i < pi.sup_opts_len; i++) {
                memcpy(longopts + i, &pi.sup_opts[i].opt, sizeof(struct option));
            }

            opts_to_pass = calloc(pi.sup_opts_len, sizeof(struct option));
            if (!opts_to_pass) {
                print_error_message("calloc() failed\nn");
                goto END;
            }


            if (DEBUG){
                for (size_t i = 0; i < pi.sup_opts_len; i++) {
                    fprintf(stderr, "DEBUG: to getopt(): passing option '%s'\n",
                            (longopts + i)->name);
                }
            }

            optind = 1;
            int saved_opterr = opterr;
            opterr = 0;
            while (1) {
                int opt_ind = 0;
                if(DEBUG){
                    printf("\nDebug: f_argc = %d", f_argc);
                    for (int i = 0; i < f_argc; i++){
                        printf("\t f_argv[%d] = %s\n", i, f_argv[i]);
                    }
                }
                ret = getopt_long(f_argc, f_argv, "", longopts, &opt_ind);
                if (ret == -1) break;

                if (ret == '?')
                    continue;

                if (ret != 0) {
                    fprintf(stderr, "ERROR: failed to parse options\n");
                    goto END;
                }

                #ifndef ALLOW_OPT_ABBREV
                // glibc quirk: no proper way to disable option abbreviations
                // https://stackoverflow.com/questions/5182041/turn-off-abbreviation-in-getopt-long-optarg-h
                int idx = (longopts + opt_ind)->has_arg ? 2 : 1;
                const char *actual_opt_name = f_argv[optind - idx] + 2; // +2 for -- before option
                const char *found_opt_name = (longopts + opt_ind)->name;
                if (strcmp(actual_opt_name, found_opt_name)) {
                    // It's probably abbreviated name, which we do not allow
                    fprintf(stderr, "ERROR: unknown option: %s\n", f_argv[optind - idx]);
                    goto END;
                }
                #endif

                // Check how many options we got up to this moment
                if ((size_t)opts_to_pass_len == pi.sup_opts_len) {
                    fprintf(stderr, "ERROR: too many options!\n");
                    goto END;
                }

                // Add this option to array of options actually passed to plugin_process_file()
                memcpy(opts_to_pass + opts_to_pass_len, longopts + opt_ind, sizeof(struct option));
                // Argument (if any) is passed in flag
                if ((longopts + opt_ind)->has_arg) {
                    // Mind this!
                    // flag is of type int*, but we are passing char* here (it's ok to do so).
                    (opts_to_pass + opts_to_pass_len)->flag = (int*)strdup(optarg);
                }
                opts_to_pass_len++;
            }
            opterr = saved_opterr;
            if (opts_to_pass_len == 0 )
                goto END;

            if (DEBUG) {
                fprintf(stderr, "DEBUG: opts_to_pass_len = %d\n", opts_to_pass_len);
                for (int i = 0; i < opts_to_pass_len; i++) {
                    fprintf(stderr, "DEBUG: passing option '%s' with arg '%s'\n",
                            (opts_to_pass + i)->name,
                            (char*)(opts_to_pass + i)->flag);
                }
            }



            FTS *ftsp;
            FTSENT *entry;
            char *path_t[] = {dir_for_scan, NULL};

            ftsp = fts_open(path_t, FTS_NOCHDIR | FTS_PHYSICAL, NULL);

            if (!ftsp) {
                print_error_message("fts_open");
            }

            while ((entry = fts_read(ftsp)) != NULL) {
                if (entry->fts_info == FTS_F) {
                    errno = 0;
                    ret = ppf_func(entry->fts_path, opts_to_pass, opts_to_pass_len);
                    if (ret < 0)
                        fprintf(stdout, "Error information: %s\n", strerror(errno));

                    if (ret > 0)
                        continue;

                    if (ret == 0){
                        add_string_to_global(entry->fts_path);
                    }
                }
            }



            END:
            if (opts_to_pass) {
                for (int i = 0; i < opts_to_pass_len; i++)
                    free( (opts_to_pass + i)->flag );
                free(opts_to_pass);
            }
            if (longopts) free(longopts);
            if (lib_name) free(lib_name);
            //if (file_name) free(file_name);
            if (dl) dlclose(dl);

            return 0;
            break;

        default:
            return 0;
            break;
    }
}

