/*
 * ex4b.c
 *
 *  Receives file name or directory name.
 *  if it directory, the program will go over the sum directories until
 *  1 file.
 *  When the program comes to file, it checks if executable file or not.
 *  if executable file - it will ask the user if he wants to delete or not.
 *  Return : How many bytes were deleted.
 *
 *  Compile: gcc ex4b.c Wall -o ex4b
 *  Run: ./ex4b FILE_NAME
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

//_________constants____________
#define   NUM_OF_ARGS     2
#define   DIR_NAME_PLACE  1

//_________prototypes________________
double delete_files(const char *dir_name);

int ask_for_delete(const char *d_name);

void clean_buf();

void delete_file(const char *file_name);

//____________main____________
int main(int argc, char **argv) {

    if (argc != NUM_OF_ARGS) {
        fprintf(stderr, "usage: %s <file name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%lf bytes were deleted \n", delete_files(argv[DIR_NAME_PLACE]));

    return EXIT_SUCCESS;
}

//___________________________________________________________________
/*
 * Recursive function. receives file name or directory and check
 * if file is executable. if is directory it will go over all the subdirectories
 * and file.
 * For each executable file it will ask for delete.
 * Return how many bytes were deleted.
 */
double delete_files(const char *file_name) {
    DIR *current_dir;
    struct dirent *dentry;
    struct stat the_stat;

    double sum = 0;
    if (stat(file_name, &the_stat) == -1) {
        fprintf(stderr, "%s -- %s \n", strerror(errno), file_name);
        exit(EXIT_FAILURE);
    }

    //check if executable and ask for delete it.
    if (!S_ISDIR(the_stat.st_mode)) {
        if ((the_stat.st_mode & S_IXUSR) && ask_for_delete(file_name)) {
            delete_file(file_name);
            return the_stat.st_size;
        } else
            return 0;
    }

    if (chdir(file_name) == -1) {
        fprintf(stderr, "cannot chdir into %s\n", file_name);
        exit(EXIT_FAILURE);
    }

    if ((current_dir = opendir(".")) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", file_name);
        exit(EXIT_FAILURE);
    }

    while ((dentry = readdir(current_dir)) != NULL) {
        if (stat(dentry->d_name, &the_stat) == -1) {
            fprintf(stderr, "%s -- %s \n", strerror(errno), file_name);
            exit(EXIT_FAILURE);
        }

        if (strcmp(dentry->d_name, ".") == 0 ||
            strcmp(dentry->d_name, "..") == 0)
            continue;

        if (S_ISDIR(the_stat.st_mode)) {
            sum += delete_files(dentry->d_name);
        } else if ((the_stat.st_mode & S_IXUSR) && ask_for_delete(dentry->d_name)) {
            delete_file(dentry->d_name);
            sum += the_stat.st_size;
        }
    }
    chdir("..");

    return sum;
}

//______________________________________________________________________
// * Receives executable name and ask the user if he wants to delete the file.
// * Answer must be 'y' for positive answer and whatever for no.
int ask_for_delete(const char *file_name) {
    char ans;
    printf("Do you want to delete %s exec? type y for yes, and anything for no.\n", file_name);
    ans = getchar();
    clean_buf();
    if (ans == 'y')
        return 1;
    return 0;
}

//______________
void clean_buf() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//________________________________
// Receives executable name and delete it.
void delete_file(const char *file_name) {
    if (unlink(file_name) == -1) {
        fprintf(stderr, "cannot delete file: %s\n", file_name);
        exit(EXIT_FAILURE);
    }
}

