#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){

    int PID = fork();

    // si le processus est l'enfant
    if(PID == 0){
        int pidChild = getpid();
        int code = pidChild % 10;

        printf("Processus enfant: PID : %d , PID pere : %d \n", pidChild, getppid());

        exit(code);
    }
    else{
    // si c'est le processus parent
        printf("Processus pere : PID : %d , fils : %d \n", getpid(), PID);

        int status;
        wait(&status);

        int codeRetour = WEXITSTATUS(status);
        printf("Code de retour du fils : %d \n", codeRetour);

    }
}