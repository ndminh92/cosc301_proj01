/*
 *
 * Dang Minh Nguyen
 * COSC 301 Project 01
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>


#include "list.h"

// Tokenify a string. Return array of strings in the heap. 
// Code Taken from lab
char** tokenify(const char *s) {
    int count = 0;
    char *scopy = strdup(s);
    char *token = strtok(scopy," \t\n"); // get first token
    char **tok_list = malloc(strlen(s) * sizeof(char *));  
    while (token != NULL) {        
        tok_list[count] = strdup(token); 
        count += 1;
        token = strtok(NULL," \t\n"); // get next token
    }
    tok_list[count] = NULL;      
    
    //optimize length of pointer array
    char **short_tok_list = malloc((count + 1) * sizeof(char *));
    for (int i = 0; i < count; i++) { // Copy up to the NULL value
        short_tok_list[i] = tok_list[i];
    }    
    short_tok_list[count] = NULL;
    free(scopy);
    free(tok_list);
    return short_tok_list;
}



// Free memory in token array
// Code taken from lab
void free_tokens(char **tokens) {
    int i = 0;
    while (tokens[i] != NULL) {
        free(tokens[i]); // free each string
        i++;
    }
    free(tokens); // then free the array
}


// Check if string consist of only integers
bool is_all_digit(char *string) {
    int length = strlen(string);
    if (!(string[0] == '-' || isdigit(string[0]))) { // allow for negative integer
        return false;
    }

    for (int i = 1; i < length; i++) {                
        if (!isdigit(string[i])) {
            return false;
        }
    }
    return true;
}

bool is_comment(char *string) {
    int length = strlen(string);
    for (int i = 0; i < length; i++) {
        if (string[i] == '#') {
            return true;
        }
    }
    return false;
}

void replace_hash(char *string) { // Replace first # with \0
    int length = strlen(string);
    for (int i = 0; i < length; i++) { 
        if (string[i] == '#') {
            string[i] = '\0';
            return;
        }
    }        
}

void process_data(FILE *input_file) {
    // !! your code should start here.  the input_file parameter
    // is an already-open file.  you can read data from it using
    // the fgets() C library function.  close it with the fclose()
    // built-in function
    char line_buffer[4096];
    struct node **head = malloc(sizeof(struct node *));
    *head = NULL;
    struct rusage usg;
   
    while(fgets(line_buffer, 4096, input_file) != NULL) { // read until EOF
        replace_hash(line_buffer);
        //printf("%s\n",line_buffer);
        char **token_list = tokenify(line_buffer);    
        char *token = token_list[0];
        int i = 0;
        while (token != NULL) {
            if (is_all_digit(token)) {
                int value = atoi(token);
                //printf("Found an integer!\n");
                list_insert(value, head);
            }
            i++;
            token = token_list[i];
        }
        free_tokens(token_list);
    }         
    printf("** Done reading file **\n");
    print_list(head);
  
    //Freeing memory
    free_list(head);

    getrusage(RUSAGE_SELF, &usg);
    float usertime = 0;
    usertime = usg.ru_utime.tv_sec + 0.000001 * usg.ru_utime.tv_usec;
    float systime = 0;
    systime = usg.ru_stime.tv_sec + 0.000001 * usg.ru_stime.tv_usec;
    printf("User time: %f\n", usertime);
    printf("System time: %f\n", systime);
}


void usage(char *program) {
    fprintf(stderr, "usage: %s [<datafile>]\n", program);
    exit(1);
}

#ifndef AUTOTEST
int main(int argc, char **argv) {
    FILE *datafile = NULL;

    /* find out how we got invoked and deal with it */
    switch (argc) {
        case 1:
            /* only one program argument (the program name) */ 
            /* just equate stdin with our datafile */
            datafile = stdin;        
            break;

        case 2:
            /* two arguments: program name and input file */
            /* open the file, assign to datafile */
            datafile = fopen(argv[1], "r");
            if (datafile == NULL) {
                printf("Unable to open file %s: %s\n", argv[1], strerror(errno));
                exit(-1);
            }
            break;

        default:
            /* more than two arguments?  throw hands up in resignation */
            usage(argv[0]);
    }

    /* 
     * you should be able to just read from datafile regardless 
     * whether it's stdin or a "real" file.
     */
    process_data(datafile);
    fclose(datafile);
    return 0;
}
#endif
