#include "launch.h"

int launch(char* game_command) {

    char *const  args[] = {game_command};
    pid_t pid = fork();
    switch(pid) 
    {
        case 0: 
            execvp(game_command, args);
            break;
        case -1: 
            fprintf(stderr, "Unable to fork secondary process\n");
            return 0;
    }
    return 1;
}

/*LZMA 7ZIP;*/