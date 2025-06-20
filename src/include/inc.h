#ifndef INC_H
#define INC_H

//Libraries

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>   // sleep()
#include <string.h>
#include <dlfcn.h>
#include <string.h>
#include <ftw.h>
#include <fts.h>
#include <sys/stat.h>
#include <errno.h>

//defines
#define A_USED 0
#define O_USED 1
#define N_USED 2
#define P_USED 3
#define H_USED 4
#define V_USED 5

#define STRIPE "<==========================================================================================>"
#define NAME "Denis Ilchuk Vitalevich"
#define VR "13"
#define GR "N3249"
#define PROGNAME "lab11dviN3249"
#define VERSION "0.5"

//Globals

extern int opt_used_counter[6];

extern char **global_maches;
extern size_t global_maches_len;
extern size_t plugins_used;
//Functions

void opt_errors(void);

void get_debug_status_mode_error_handlers(bool flag);

void print_standart_message(char flag);

void punish_dummy_user(const char *err_msg);

void print_error_message(const char *err_msg);

int yes_or_no(const char *input);

int scan_dir_for_dynamic_lib_options_if_user_provide_no_dir_for_scan_via_dynamic_lib(const char *fpath, const struct stat *sb, int typeflag);

void get_terminal_arguments_from_main_to_functions (int argc, char *argv[], char *dir_with_libs);

bool is_directory(const char *path);

void get_debug_status_mode_functions (bool flag);

int scan_dir_via_dynamic_lib_or_libs_for_matches(const char *fpath, const struct stat *sb, int typeflag);

struct option* get_all_options(char *plugin_dir_path, size_t *count);

bool is_it_so_lib(const char *path);

char * get_p_dir(int argc, char **argv);

void print_matches_tree(void);

void apply_logic(const char *dir, bool A, bool O, bool N);
#endif