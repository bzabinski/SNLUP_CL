// Sentences.c

#include <stdio.h>
#include <string.h>
#include "snlup.h"
#include "externs.h"
#include "utilities.h"
#include "io.h"

int prune_sentence(char input2[SENTLEN][WORDSIZE], int num_input2_words)
{
    // Eliminate all duplicate words within a sentence.
    
    int num_left = 0, a = 0, b = 0, found = 0;
    char temp[SENTLEN][WORDSIZE];
    
    for (a = 0; a < num_input2_words; a++)
    {
        found = 0;
        for (b = a + 1; b < num_input2_words; b++)
        {
            if (strcmp(input2[a], input2[b]) == 0)
                found++;
        }
        if (!found)
        {
            strcpy(temp[num_left], input2[a]);
            num_left++;
        }
    }
    for (a = 0; a < num_left; a++)
        strcpy(input2[a], temp[a]);
    
    return (num_left);
    
} // End proc prune_sentence

//************************************************************************

void show_sentence_pattern(char action[SENTLEN][WORDSIZE], int num_action_words,
                           char phrase[SENTLEN][WORDSIZE], int num_phrase_words)
{
    // For debugging, show sentence pattern and objects on screen and write it to a file
    // for external interfaces.
    
    int i = 0, j = 0;
    FILE *curphrase;
    
    //if(recurse_num==1)
    curphrase = openfile("curphrase.txt", "w");
    //else
    //  curphrase = openfile("curphrase.txt", "a");
    
    fprintf(curphrase, "Recursion Iteration: %d\n", recurse_num);
    fprintf(curphrase, "Action: ");
    for (i = 0; i < num_action_words; i++)
        fprintf(curphrase, "%s ", action[i]);
    fprintf(curphrase, "\n");
    fprintf(curphrase, "Object 1:");
    for (i = 0; i < num_object1_words; i++)
        fprintf(curphrase, "%s ", object1[i]);
    fprintf(curphrase, "\n");
    fprintf(curphrase, "Object 2: ");
    for (i = 0; i < num_object2_words; i++)
        fprintf(curphrase, "%s ", object2[i]);
    fprintf(curphrase, "\n");
    fprintf(curphrase, "Sentence pattern: ");
    for (i = 0; i < num_phrase_words; i++)
    {
        if (strcmp(phrase[i], "<object>") == 0 || strcmp(phrase[i], "<OBJECT>")== 0)
        {
            if (j == 0)
            {
                fprintf(curphrase, "< ");
                for (j = 0; j < num_object1_words; j++)
                    fprintf(curphrase, "%s ", object1[j]);
                fprintf(curphrase, "> ");
            }
            else
            {
                fprintf(curphrase, "< ");
                for (j = 0; j < num_object2_words; j++)
                    fprintf(curphrase, "%s ", object2[j]);
                fprintf(curphrase, "> ");
            }
        }
        else
            fprintf(curphrase, "%s ", phrase[i]);
    }
    fprintf(curphrase, "\n");
    
    closefile(curphrase);
    
} // End proc show_sentence_pattern