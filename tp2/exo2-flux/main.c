#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]){
     
    printf("Voici le premier mot en argument : %s\n", argv[1]);

    int pid = fork();

    if(pid == 0){
        // on ferme la sortie standard à l'écran pour écrire par la suite dans le fichier
        // close(1);
        close(2); 

        // creation du fichier temporaire
        char filename[] = "/temp/proc-exercise";
        int fileTemp = mkstemp(filename); //mkstemp crée un fichier temporaire et retourne le descripteur de fichier

        printf("Le descripteur de fichier temporaire est : %d\n", fileTemp);

        close(fileTemp); // on ferme le descripteur de fichier temporaire

        execlp(argv[1], argv[1], NULL); 
        

    }
    else{
        printf("Processus pere : PID : %d \n", getpid());

        wait(NULL); // on attend la fin du processus enfant

        printf("Le processus enfant est termine.\n");
        printf("That’s All Folks ! \n");
    } 
    
}