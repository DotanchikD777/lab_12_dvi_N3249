#include "../include/inc.h"
#include "../include/plugin_api.h"



// var only in this file






int main(int argc, char *argv[]){
    int opt;
    int option_index = 0;
    
    bool A_flag = false;
    bool O_flag = false;
    bool N_flag = false;
    char *P_dir = NULL;

    bool DEBUG = getenv("LAB12DEBUG") != NULL;



    get_debug_status_mode_error_handlers_functions(DEBUG);

    const char *optstring = "P:AONvh";


    static struct option long_options[] = {
        {"P",   required_argument,  0,    'P'},
        {"A",   no_argument,        0,    'A'},
        {"O",   no_argument,        0,    'O'},
        {"N",   no_argument,        0,    'N'},
        {"help",   no_argument,     0,    'h'},
        {"version",   no_argument,  0,    'v'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1){

        switch (opt) {
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
// testing
    switch (argc - optind) {
        case 0:

            if(DEBUG)
                printf("\nDebug: user provide no args\n");


            printf("\n%s\nProgram has finished successful!\n%s\n",
                             STRIPE,                               STRIPE);

            return EXIT_SUCCESS;
            break;
        case 1:

            if (ftw(P_dir, scan_dir_for_dynamic_lib_options_if_user_provide_no_dir_for_scan_via_dynamic_lib, 10) == -1) {
                print_error_message("ftw");
            }


            printf("\n%s\nProgram has finished successful!\n%s\n",
                   STRIPE,                               STRIPE);



            return EXIT_SUCCESS;

            break;



    }
}