/*=============================ex1b===============================
 * name: Roee ben ezra
 * id: 206123994
 * 
 program B: this program use 3 other programs and running the
            program the user give throw child process
            programs : 1. history(+file name)
                       2. ps (gives process id)
                       3. echo (print strings)

==============================================================*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

FILE *open_file(char file_name[]);

int do_fork();

void do_child(char *args[]);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("wrong inp files");
        exit(1);
    }
    char *line = NULL;      //for read line from stdin
    size_t size = 1;
    line = (char *) malloc(size);   //allocate 1 char for start
    char prog[10];         //to save first string as program name to run
    FILE *fp = open_file(argv[1]);
    while (1) {
        getline(&line, &size, stdin);
        fprintf(fp, "%s", line);
        fseek(fp, -strlen(line), SEEK_CUR);
        fscanf(fp, "%s", prog);
        fseek(fp, 0, SEEK_END);

        if (!strcmp(prog, "exit")) {
            unlink(argv[1]);    //delete file
            exit(0);
        }

        //compare for existing program
        if (!strcmp(prog, "history") || !strcmp(prog, "echo") || !strcmp(prog, "ps")) {
            char *args[] = {prog, line, NULL};  //for passing exec()
            int status = do_fork();
            if (status == 0) //child
                do_child(args);

            if (wait(&status) == -1)
                perror("wait");
        }
        else
            puts("bad command");
    }
    free(line);
    fclose(fp);

    return EXIT_SUCCESS;
}

//---------------------------------------------------
void do_child(char *args[]) {
    if (execvp(args[0], args) == -1) {
        perror("exec");
        exit(1);
    }
    exit(0);
}

//--------------------------------------------------------
FILE *open_file(char file_name[]) {
    FILE *fp = fopen(file_name, "w+");
    if (!fp) {
        perror("cannot open file\n");
        exit(0);
    }
    return fp;
}

//--------------------------------------------------------
int do_fork() {
    int status = fork();
    if (status < 0) {
        perror("cannot fork");
        exit(0);
    }
    return status;
}
