//
// Created by denis on 15.05.2025.
//

#include "../include/inc.h"
#include "../include/plugin_api.h"

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

bool match_lib_so(const char *path) {
    // 1) Найдём имя файла (после последнего '/')
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

int scan_dir_for_dynamic_lib_options_if_user_provide_no_dir_for_scan_via_dynamic_lib(const char *fpath, const struct stat *sb, int typeflag){ // пока не тестил надо доделать
    switch (typeflag) {
        case FTW_D:
            return 0;
            break;
        case FTW_F:
            if (match_lib_so(fpath)){

                char *lib_name = strdup(fpath);

                struct plugin_info pi = {0};

                dlerror();
                void *dl = dlopen(lib_name, RTLD_LAZY);

                if (!dl) {
                    fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
                    return 0;
                }

                void *func = dlsym(dl, "plugin_get_info");

                if (!func) {
                    fprintf(stderr, "ERROR: dlsym() for %s failed: %s\n", lib_name, dlerror());
                    return 0;
                }

                typedef int (*pgi_func_t)(struct plugin_info*);
                pgi_func_t pgi_func = (pgi_func_t)func;

                int ret = pgi_func(&pi);
                if (ret < 0) {
                    fprintf(stderr, "ERROR: plugin_get_info()  for %s failed\n", lib_name);
                    return 0;
                }

                // Plugin info
                fprintf(stdout, "Plugin purpose:\t\t%s\n", pi.plugin_purpose);
                fprintf(stdout, "Plugin author:\t\t%s\n", pi.plugin_author);
                fprintf(stdout, "Supported options: ");
                if (pi.sup_opts_len > 0) {
                    fprintf(stdout, "\n");
                    for (size_t i = 0; i < pi.sup_opts_len; i++) {
                        fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
                    }
                }
                else {
                    fprintf(stdout, "none (!?)\n");
                }
                fprintf(stdout, "\n");

                return 0;

            } else
                return 0;
            break;
        default:
            return 0;
            break;
    }
}