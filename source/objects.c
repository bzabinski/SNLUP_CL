// Objects.c

#include <stdio.h>
#include <string.h>
#include "snlup.h"
#include "externs.h"
#include "phrases.h"
#include "actions.h"
#include "utilities.h"
#include "io.h"
#include "facts.h"

void reduce_objects(void)
{
    int num_action_words = 0, num_phrase_words = 0;
    int num_input_words = 0;
    int i = 0, reduced = FALSE, skip_reduction = FALSE;
    int save_num_object1_words = 0;
    int save_num_object2_words = 0;
    char save_object1[SENTLEN][WORDSIZE];
    char save_object2[SENTLEN][WORDSIZE];
    char action[SENTLEN][WORDSIZE];
    char phrase[SENTLEN][WORDSIZE];
    char input[SENTLEN][WORDSIZE];
    
    // Save Group File
    copyfile("group.grp","group3.grp");
    
    // Save object2 for later
    for (i = 0; i < num_object2_words; i++)
        strcpy(save_object2[i], object2[i]);
    save_num_object2_words = num_object2_words;
    
    if (num_object1_words < 2)
        reduced = TRUE;
    
    while (!reduced)
    {
        for (i = 0; i < num_object1_words; i++)
            strcpy(input[i], object1[i]);
        num_input_words = num_object1_words;
        i = lookup_phrase(input, num_input_words, 100, PHRASE, GROUP1);
        // lookup object1
        if (i < 2)
            reduced = TRUE;
        if (!reduced)
        {
            num_phrase_words = choose_phrase(1, PHRASE, phrase, GROUP1);
            // Pick first matched phrase
            match_phrase(input, num_input_words, phrase, num_phrase_words);
            // set objects
        }
        if (num_object1_words == num_input_words)
            reduced = TRUE;
        if (!reduced)
        {
            num_action_words = choose_phrase(1, ACTION, action, GROUP1);  // Pick first match
            //build_per_fact_file(input, num_input_words, NEW);
            do_action(action, num_action_words, input, num_input_words);
        }
    }
    // Save object1 for later
    for (i = 0; i < num_object1_words; i++)
        strcpy(save_object1[i], object1[i]);
    save_num_object1_words = num_object1_words;
    
    // Restore object2
    for (i = 0; i < save_num_object2_words; i++)
        strcpy(object2[i], save_object2[i]);
    num_object2_words = save_num_object2_words;
    
    reduced = FALSE;
    
    if (num_object2_words < 2)
    {
        reduced = TRUE;
        skip_reduction = TRUE;
    }
    
    while (!reduced && !object2_is_not_a_true_object)
    {
        for (i = 0; i < num_object2_words; i++)
            strcpy(input[i], object2[i]);
        num_input_words = num_object2_words;
        i = lookup_phrase(input, num_input_words, 100, PHRASE, GROUP1);
        // lookup object2
        if (i < 2)
            reduced = TRUE;
        if (!reduced)
        {
            num_phrase_words = choose_phrase(1, PHRASE, phrase, GROUP1);    // Pick first matched phrase
            match_phrase(input, num_input_words, phrase, num_phrase_words); // set objects
        }
        if (num_object1_words == num_input_words)
            reduced = TRUE;
        if (!reduced)
        {
            num_action_words = choose_phrase(1, ACTION, action, GROUP1);  // Pick first match
            //build_per_fact_file(input, num_input_words, NEW);
            do_action(action, num_action_words, input, num_input_words);
        }
        // Set object2 for next time through loop
        for (i = 0; i < num_object1_words; i++)
            strcpy(object2[i], object1[i]);
        num_object2_words = num_object1_words;
    }
    
    if (!skip_reduction)
    {
        // Set object2 for later
        for (i = 0; i < num_object1_words; i++)
            strcpy(object2[i], object1[i]);
        num_object2_words = num_object1_words;
    }
    
    // Restore object1
    for (i = 0; i < save_num_object1_words; i++)
        strcpy(object1[i], save_object1[i]);
    num_object1_words = save_num_object1_words;
    
    // Restore  Group File
    if (access_file("group3.grp")==0)
    {
        copyfile("group3.grp","group.grp");
        removefile("group3.grp");
    }
    
} // End reduce_objects

void replace_object(int flag, char sentence[SENTLEN][WORDSIZE], int num_sent_words)
{
	FILE *oldobjectfile;
	char obj1[SENTLEN][WORDSIZE];
	char obj2[SENTLEN][WORDSIZE];
	char new_sent[SENTLEN][WORDSIZE];
	int num_obj1_words, num_obj2_words, new_sent_size = 0;
	int i, j = 0;
	
	printf("Start replace object\n");
	
	// Open the old object file
	oldobjectfile = openfile("oldobjects.txt", "r");
	
	// Get obj1
	//if (fgets(text, SENTSIZE, oldobjectfile) != NULL)
	//	num_obj1_words = parse(obj1, NULL, text);
	
    num_obj1_words = parse(obj1, oldobjectfile);
    
    // Get obj2
	//if (fgets(text, SENTSIZE, oldobjectfile) != NULL)
	//	num_obj2_words = parse(obj2, NULL, text);
    
    num_obj2_words = parse(obj2, oldobjectfile);
    
	// Close old object file
	closefile(oldobjectfile);
	
	// Build new sentence
	if (flag == LAST && num_obj2_words != 0)
	{
		for (i=0; i<num_sent_words - 1; i++)
			strcpy(new_sent[i], sentence[i]);
		for (j=0; j<num_obj2_words; j++)
			strcpy(new_sent[i+j], obj2[j]);
		new_sent_size = num_sent_words - 1 + num_obj2_words;
		
		for (i=0; i<new_sent_size; i++)
			printf("%s ", new_sent[i]);
		printf("\n");
		
		// Process the new sentence
		process_input(new_sent, new_sent_size);
	}
	
}
