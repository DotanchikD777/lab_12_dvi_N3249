#include "inc.h"



// var only in this file






int main(int argc, char *argv[]){
    int opt;
    int option_index = 0;
    
    bool A_flag = false;
    bool O_flag = false;
    bool N_flag = false;
    bool v_flag = false;
    bool h_flag = false;
    char *P_dir = NULL;
    

    bool DEBUG = (getenv("LAB1DEBUG") != NULL);

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
                // print --help message
                printf("\nhelp\n");
                opt_used_counter[H_USED] += 1;
                break;
            case 'v':
                // print --version message
                printf("\nhelp\n");
                opt_used_counter[V_USED] += 1;
                break;
            case '?':
                printf("\nError: corrupted options\n");
                exit(EXIT_FAILURE);
                break;
            default:
                printf("\nError: Impossible error\n");
                exit(EXIT_FAILURE);
        }
        
        if (DEBUG){
            printf("Debug: opt='%c', A=%d, O=%d, N=%d, P=%s\n",
                    (char)opt, A_flag, O_flag, N_flag, 
                    P_dir ? P_dir : "(none)" );
        }
    }

    if(!opt_used_counter[O_USED] && !opt_used_counter[A_USED]){
        A_flag = true;
        if(DEBUG){
            printf("\nDebug: default case --> A=%d", A_flag);
        }
    }

    opt_errors();




    




}