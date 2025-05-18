//
// Created by denis on 15.05.2025.
//

#include "../include/inc.h"

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
    enum { BUF_SIZE = 2 };
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

void init_debug_mode(bool *debug_flag) {
    const char *env = getenv("LAB12DEBUG");

    if (env && strcmp(env, "1") == 0) {
        *debug_flag = true;
        return;
    }

    if (env && strcmp(env, "-1") == 0)
        return;


    if (yes_or_no("\nDebug mode is off. Enable debug?"))
        *debug_flag = true;
    else
        *debug_flag = false;


    if (setenv("LAB12DEBUG", "-1", 1) != 0)
        print_error_message("setenv");

}