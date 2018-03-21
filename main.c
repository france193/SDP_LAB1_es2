/**
 * Name:    Francesco
 * Surname: Longo
 * ID:      223428
 * Lab:     1
 * Ex:      2
 *
 * Write a C program that takes as arguments a number C and a directory name dir.
 * The main program, using the system call system, outputs in a file list.txt the list of files in directory dir.
 * Then it reads the content of the file list.txt, and for each read line (a filename) forks a child process,
 * which must sort the file by executing (through the execlp system call) the Unix sort program with the appropriate
 * arguments.
 * Notice that the command sort –n –o fname fname sorts in ascending order the content of fname, and by means of
 * the –o option rewrites the content of file fname with the sorted numbers. Option –n indicates numeric rather
 * than alphabetic ordering.
 * The main process can create a maximum of C children that sort different files in concurrency, to avoid overloading
 * the system. Then, it has to wait the termination of these children before reading the next filename from
 * file list.txt.
 * After all files listed in list.txt have been sorted, the main process must produce a single file all_sorted.txt,
 * where all the numbers appearing in all the sorted files are sorted in ascending order. Do this by using again
 * system call system with the appropriate command.
 * Take care of dealing with a number of files that is not a multiple of C, i.e., remember to wait for the last files
 * of the list.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    fprintf(stdout, "> Start\n");

    int C;
    char dirname[20];
    char *filename = "list.txt";

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argc != 3) {
        fprintf(stdout, "Expected 3 argument: <prog_name> <C> <dirname>\n");
        exit(-1);
    }

    // take C and dirname from command line
    C = atoi(argv[1]);
    strcpy(dirname, argv[2]);
    fprintf(stdout, "> Input args: %d, %s\n", C, dirname);

    // initialize a string for command
    char cmd[30];
    strcpy(cmd, "ls ./");
    strcat(cmd, dirname);
    strcat(cmd, " > ");
    strcat(cmd, filename);

    if (system(cmd) == -1) {
        fprintf(stdout, "Error executing system\n");
        exit(-2);
    }
    fprintf(stdout, "> File %s created!\n", filename);

    if ((fp = fopen(filename, "rt")) == NULL) {
        fprintf(stdout, "Error creating file\n");
        exit(-3);
    }

    int totalFiles = 0;
    while ((getline(&line, &len, fp)) != -1) {
        totalFiles++;
    }
    fclose(fp);
    fprintf(stdout, "> Total files: %d\n", totalFiles);

    if ((fp = fopen(filename, "rt")) == NULL) {
        fprintf(stdout, "Error creating file\n");
        exit(-4);
    }

    char temp[50];
    int status, numChild = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        strcpy(temp, "/0");
        fprintf(stdout, "> File: %s\n", line);
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);

        if (totalFiles > 0) {
            if (numChild >= C) {
                // wait for a child termination
                wait(&status);
                numChild--;

                if (status != 0) {
                    fprintf(stdout, "Error, child error\n");
                    exit(-4);
                }
            } else {
                int pid = fork();

                switch (pid) {
                    case 0:
                        // child
                        strcpy(temp, "./");
                        strcat(temp, dirname);
                        strcat(temp, "/");
                        strcat(temp, line);

                        char name[20];
                        sprintf(name, "mySort(%i)", getpid());

                        int res = execlp("sort", name, "-n", "-o", temp, temp, (char *)NULL);

                        if (res < 0) {
                            fprintf(stdout, "Error, exec error\n");
                            exit(-6);
                        }
                        break;

                    case -1:
                        // error
                        fprintf(stdout, "Error, fork error\n");
                        exit(-5);

                    default:
                        // father
                        numChild++;
                        totalFiles--;
                        break;

                }
            }
        }
    }

    fclose(fp);
    return 0;
}
