// Respond.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snlup.h"
#include "externs.h"
#include "io.h"
#include "utilities.h"
#include "phrases.h"

void add_response_word(char rword[WORDSIZE])
{
    // Adds a response word to the global variable "response" and
    // updates the global counts "num_response_words"
    
    if (strlen(rword) > (WORDSIZE - 1))
    {
        printf("\nError: Maximum response word size has been exceeded\n");
        printf("Word: %s\n", rword);
        exit_snlup(0);
    }
    if (num_response_words > SENTLEN)
    {
        printf("\nError: Maximum number of response words has been exceeded\n");
        printf("Num words: %d\n", num_response_words);
        exit_snlup(0);
    }
    strcpy(response[num_response_words], rword);
    num_response_words++;
    
} // end add_response_word

//*******************************************************************

int get_response(char action[SENTLEN][WORDSIZE], int num_words)
{
    // Looks up an appropriate purpose to match the action passed
    // in, from the file responses.rsp. Puts the responses found
    // in a group file. Returns number of responses found.
    
    char temp_response[SENTLEN][WORDSIZE];
    int num_responses_found = 0, num_temp_words = 0;
    int new_entry = TRUE, found_one = FALSE, i = 0;
    FILE *response_file;
    FILE *group_file;
    
    response_file = openfile("response.rsp", "r");
    group_file = openfile("group.grp", "w");
    
    while (!feof(response_file))
    {
        num_temp_words = parse(temp_response, response_file);
        if (new_entry)
        {
            if (exact_match(temp_response, num_temp_words, action, num_words))
                found_one = TRUE;
        }
        if (strcmp(temp_response[0], "end") == 0 || strcmp(temp_response[0], "END")
            == 0)
        {
            found_one = FALSE;
            new_entry = TRUE;
        }
        else if (found_one && !new_entry)
        {
            for (i = 0; i < num_temp_words; i++)
                fprintf(group_file, "%s ", temp_response[i]);
            fprintf(group_file, "\n");
            num_responses_found++;
        }
        
        // This must always come after the print statements, to skip the first
        // line of the new entry.
        if (found_one && new_entry)
            new_entry = FALSE;
        
    } // End while not feof
    
    closefile(response_file);
    closefile(group_file);
    
    return (num_responses_found);
    
} // End proc get_response

//*******************************************************************

void reply(void)
{
    //  Opens the output file and writes the last response to it.
    //  Global Variable intercept places the response in a file
    //  instead of printing it to the screen.
    
    int i = 0;
    FILE *intercept_file;
    FILE *capture_file;
    FILE *output_file;
    
    //printf("Enter proc reply\n");
	
    if (capture_output==TRUE)
    {
        if (access_file("capture.txt")==0)
            capture_file = openfile("capture.txt", "a"); // open capture file for append
        else
            capture_file = openfile("capture.txt", "w"); // open cature file for first time
    }
    
    if (access_file("output.txt")==0)
        output_file = openfile("output.txt", "a"); // open output file for append
    else
        output_file = openfile("output.txt", "w"); // open output file for first time
    
	if (intercept == RESPONSE || intercept == MONITOR)
        intercept_file = openfile("intercpt.tmp", "w");
    // A script will intercept the reply
    for (i = 0; i < num_response_words; i++)
    {
        if ((intercept != RESPONSE) && (!suppress_reply))
        {
            if (strcmp(response[i], "\n") == 0)
            {
                printf("%s", response[i]);
                fprintf(output_file, "%s", response[i]);
            }
            else
            {
                if (i < num_response_words-1 && ((strcmp(response[i+1], ",")==0)|| (strcmp(response[i+1], "?")==0)))
                {
                    printf("%s", response[i]);
                    fprintf(output_file, "%s", response[i]);
                }
                else
                {
                    printf("%s ", response[i]);
                    fprintf(output_file, "%s ", response[i]);
                }
            }
        }
        if (intercept == RESPONSE || intercept == MONITOR)
        {
            if (strcmp(response[i], "\n") == 0)
                fprintf(intercept_file, "%s", response[i]);
            else
                fprintf(intercept_file, "%s ", response[i]);
            intercepted = TRUE;
        }
    }
    
    if (intercept != RESPONSE)
    {
        if (capture_output==TRUE)
        {
            for (i = 0; i < num_response_words; i++)
                if (strcmp(response[i], "\n") == 0)
                    fprintf(capture_file, "%s", response[i]);
                else
                    fprintf(capture_file, "%s ", response[i]);
        }
        
    }
    num_response_words = 0; // Reset counters FIRST!! before erasing response
    for (i = 0; i < SENTLEN - 1; i++)
        // Erase response and pronunciation strings
        add_response_word("");
    num_response_words = 0; // Reset counters after call to add_response_word
    
    if (suppress_reply)
        suppress_reply = FALSE;
    
    if (intercept == RESPONSE || intercept == MONITOR)
        closefile(intercept_file);
	
    if (capture_output==TRUE)
        closefile(capture_file);
    
	closefile(output_file);
    fflush(stdout);
	
    //printf("End proc reply\n");
	
} // End proc reply