#include "../include/inc.h"
#include "../include/plugin_api.h"



// var only in this file


char **global_maches = NULL;
size_t global_maches_len = 0;



int main(int argc, char *argv[]){
    int opt;
    int option_index = 0;
    
    bool A_flag = false;
    bool O_flag = false;
    bool N_flag = false;
    char *P_dir = get_p_dir(argc, argv);

    bool DEBUG = getenv("LAB12DEBUG") != NULL;


    get_debug_status_mode_error_handlers(DEBUG);
    get_debug_status_mode_functions (DEBUG);

    const char *optstring = "P:AONvh";

    size_t count_opt = 0;
    struct option *long_options = get_all_options(P_dir, &count_opt);

    while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1){

        switch (opt) {
            case 0:
                /* plugin-specific option parsed */
                break;
            case 'A':
                A_flag = true;
                opt_used_counter[A_USED] += 1;
                break;
            case 'O':
                O_flag = true;
                opt_used_counter[O_USED] += 1;
                break;
            case 'N':
                N_flag = true;
                opt_used_counter[N_USED] += 1;
                break;
            case 'P':
                P_dir = optarg;
                opt_used_counter[P_USED] += 1;
                break;
            case 'h':
                print_standart_message('h');
                opt_used_counter[H_USED] += 1;
                break;
            case 'v':
                print_standart_message('v');
                opt_used_counter[V_USED] += 1;
                break;
            case '?':
                print_error_message("corrupted options");
                break;
            default:
                print_error_message("impossible error");
                break;
        }
        
        if (DEBUG){
            printf("Debug: opt='%c', A=%d, O=%d, N=%d, P=%s\n",
                    (char)opt, A_flag, O_flag, N_flag, 
                    P_dir ? P_dir : "(none)" );
        }
    }

    if(!opt_used_counter[O_USED] && !opt_used_counter[A_USED]){
        A_flag = true;
        if(DEBUG)
            printf("\nDebug: default case --> A=%d\n", A_flag);
    }

    if (!P_dir){
        P_dir = "./lib/";
        if (DEBUG)
            printf("\nDebug: default case --> P_dir=%s\n", P_dir);
    }

    opt_errors();

    if (DEBUG) {
        printf("\nDEBUG: options count: %zu, P_dir = %s\n", count_opt, P_dir);
        for (size_t i = 0; i < count_opt; i++) {
            printf("\nDEBUG: cached option: %s\n", long_options[i].name);
        }
    }



    if(argc - optind == 0) {
        if(DEBUG)
            printf("\nDebug: user provide no args\n");

        if (ftw(P_dir, scan_dir_for_dynamic_lib_options_if_user_provide_no_dir_for_scan_via_dynamic_lib, 10) == -1)
            print_error_message("ftw");

        printf("\n%s\nProgram has finished successful!\n%s\n",
                             STRIPE,                               STRIPE);
        free(long_options);
        return EXIT_SUCCESS;
    } else {

        if (!is_directory(argv[argc-1])){

            if(DEBUG)
                printf("\nDebug: user provide no dir to scan: %s\n", argv[argc-1]);

            if (ftw(P_dir, scan_dir_for_dynamic_lib_options_if_user_provide_no_dir_for_scan_via_dynamic_lib, 10) == -1)
                print_error_message("ftw");



            printf("\n%s\nProgram has finished successful!\n%s\n",
                   STRIPE,                               STRIPE);
            free(long_options);
            return EXIT_SUCCESS;
        }

        char *dir_to_scan = argv[argc-1];

        if (DEBUG)
            printf("\nDebug: user provide %s dir to scan\n", dir_to_scan);



        get_terminal_arguments_from_main_to_functions(argc, argv,dir_to_scan);

        if (ftw(P_dir, scan_dir_via_dynamic_lib_or_libs_for_matches, 10) == -1)
            print_error_message("ftw");

        apply_logic(dir_to_scan, A_flag, O_flag, N_flag);

        print_matches_tree();

        for (size_t i = 0; i < global_maches_len; i++)
        {
            free(global_maches[i]);
        }



        free(global_maches);
        global_maches = NULL;
        global_maches_len = 0;
        free(long_options);
        return EXIT_SUCCESS;
    }
}