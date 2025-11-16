#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]){
     
    // on créer un fork pour ne pas pouvoir etre bloqué juste dans le ps et pouvoir faire la suite de la commande
    int pid = fork();

    if(pid == 0){
        // processus fils, on peut executer le ps eaux 
        execlp("ps", "ps", "eaux", NULL);

        

    }
    else{
        // sinon c'est le processus pere, on attend la fin du fils
        wait(NULL);
        printf("Processus fils terminé.\n");
    }


}