#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

    // tableau de descripteurs du pipe. [0] pour la lecture, [1] pour l'écriture
    int pipedef[2];

    if(pipe(pipedef) == -1){         //pipe(pipedef) crée un pipe et remplit pipedef avec les descripteurs
        perror("Erreur pipe");
        exit(1);
    }
     
    // on crée un fork pour ne pas pouvoir etre bloqué juste dans le ps et pouvoir faire la suite de la commande
    int pid1 = fork();

    if(pid1 == 0){
        // Fils 1 : execution de ps eaux ------

        // on renvoie la sortie vers le pipe en ecriture AVANT l'execution de ps car ps prend le processus courant, on peut plus rien faire apres
        close(pipedef[0]);                  // on ferme la lecture du pipe car on n'en a pas besoin ici
        dup2(pipedef[1], STDOUT_FILENO);    // redirige la sortie vers le pipe en écriture
        close(pipedef[1]);                  // puis on peut fermer 


        // on fait donc la commande ps eaux

        // argument 1 : programme à exécuter
        // argument 2 : correspond à argv[0] pour le programme exécuté (le nom du programme)
        // argument 3 : parametres, correspond à argv[1] pour le programme exécuté (ici "eaux")
        // argument 4 : NULL pour terminer la liste des arguments)
        execlp("ps", "ps", "eaux", NULL);

        perror("Erreur lors de l'execution de ps");
        exit(1);
    }

    int pid2 = fork();

    if(pid2 < 0){ perror("Erreur fork"); exit(1);}
    if(pid2 == 0){
        //Fils 2 : execution de grep "^root" ------

        close(pipedef[1]);                  // on n'ecrit pas donc on ferme la partie ecriture 
        dup2(pipedef[0], STDIN_FILENO);     // on redirige vers le pipe en lecture
        close(pipedef[0]);                  // puis on peut fermer cette partie là aussi

        // redirection de la sortie standard vers /dev/null 
        int sortieNull = open("/dev/null", O_WRONLY);
        if(sortieNull == -1){ perror("Erreur ouverture /dev/null"); exit(1);}
        dup2(sortieNull, STDOUT_FILENO);    // redirige la sortie standard vers /dev/null
        close(sortieNull);                   // on peut fermer le descripteur


        execlp("grep", "grep", "^root", NULL);

        perror("Erreur lors de l'execution de grep");
        exit(1);
    }

    // Pere -------------

    //fermeture des deux cotés du pipe
    close(pipedef[0]);
    close(pipedef[1]);

    // on attend la fin des fils
    waitpid(pid1, NULL, 0);         // attente du ps 
    int status;
    waitpid(pid2, &status, 0);      // attente du grep et recupération du statut de fin

    // recuperation du code de retour de grep pour savoir si il a trouvé root
    if(WIFEXITED(status) && WEXITSTATUS(status) == 0){
        char msg[] = "root est connecté !! \n";
        write(STDOUT_FILENO, msg, sizeof(msg)-1);
    }

    return 0;
}