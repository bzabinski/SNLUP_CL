// Hypothesis.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "snlup.h"
#include "externs.h"
#include "phrases.h"
#include "groups.h"
#include "io.h"
#include "utilities.h"
#include "facts.h"
#include "logic.h"
#include "hypothesis.h"
#include "respond.h"

void store_hypothesis(char hyp[SENTLEN][WORDSIZE], int num_words, int type_flag, int search_flag)
{
    // Check a hypothesis to see if it already exists or if it has a contradiction, if not
    // then record the hypothesis.
    
    // Algorithm
    //
    // If duplicate found then
    //   increment pos score
    // else
    //   search for contradictions
    
    // If no contradictions and no duplicates then
    //   record_hypothesis();
    // else
    //   increment neg score on hypothesis
    
    int a = 0, b = 0, c = 0;
    int duplicate_found = 0, contradiction_found = 0;
    int num_fact_words = 0;
    char fact[SENTLEN][WORDSIZE];
    char phrase[SENTLEN][WORDSIZE];
    FILE *group_file;
    
    // Search for duplicates
    if (type_flag == FACT)
    {
        look_for_relative_facts(search_flag);                             // look for related facts
        a = find_all_facts(hyp, num_words, APPEND, search_flag, GROUP1);  // Find all facts
        lookup_phrase(hyp, num_words, 100, PHRASE, GROUP2);               // lookup a phrase
        c = choose_phrase(1, PHRASE, phrase, GROUP2);                     // Pick first matched phrase
        match_phrase(hyp, num_words, phrase, c);                          // set objects
    }
    //  else if (type_flag == RULE)
    //	a = find_all_rules(hyp, num_words, NEW);
    
    group_file = openfile("group.grp", "r");
    
    if (a > 0) // Possible duplicates or contraditions found
    {
        for (b = 0; b < a; b++)   // For each fact found
        {
            num_fact_words = parse(fact, group_file);  // Get fact from group file
            if (exact_match(fact, num_fact_words, hyp, num_words))
            {
                increase_hyp_score(hyp, num_words, type_flag, POS); // Found a duplicate
                duplicate_found += 1;
            }
            else if (check_if_contradiction(hyp, num_words, fact, num_fact_words))
            {
                increase_hyp_score(fact, num_fact_words, type_flag, NEG); // Found a contradition
                contradiction_found += 1;
            }
        }
    }
    
    // If no facts matched or no duplicates or no contradictions
    if (a == 0||
        (duplicate_found == 0 && contradiction_found == 0))
        record_hypothesis(hyp, num_words, type_flag);      // Record new hypothesis
    
    closefile(group_file);
    
} // End proc store_hypothesis

//*********************************************************************

void record_hypothesis(char hyp[SENTLEN][WORDSIZE], int num_words, int type_flag)
{
    // Record a hypothesis in the fact or rules hypothesis files.
    
    int i = 0;
    FILE *hyp_file;
    
    if (type_flag == FACT)
    {
        hyp_file = openfile("facts.hyp", "a");
        //  housekeeping = CHECKHYPFACTS;
    }
    else if (type_flag == RULE)
    {
        hyp_file = openfile("rules.hyp", "a");
        //  housekeeping = CHECKHYPRULES
    }
    fseek(hyp_file, 0, SEEK_END);
    
    strcpy(hyp[num_words], "1");
    strcpy(hyp[num_words+1],"0");
    
    for (i = 0; i < num_words+2; i++)
        fprintf(hyp_file, "%s ", hyp[i]);
    fprintf(hyp_file, "\n");
    
    closefile(hyp_file);
    
} // End proc record_hypothesis

//*********************************************************************

void increase_hyp_score(char hyp[SENTLEN][WORDSIZE], int num_words, int type_flag, int pos_or_neg)
{
    // Lookup a hypothesis and increase the positive or negative score
    // as indicated by the pos_or_neg flag.
    
    int a = 0, lines_found = 0;
    int pos_score = 0, neg_score = 0;
    int num_new_words = 0, file_exists = FALSE;
    char new_hyp[SENTLEN][WORDSIZE];
    FILE *hyp_file;
    FILE *temp_file;
    
    if (type_flag == FACT)
    {
        if( access_file("facts.hyp")==0)
        {
            hyp_file = openfile("facts.hyp", "r");
            file_exists = TRUE;
        }
    }
    else if (type_flag == RULE)
    {
        if( access_file("rules.hyp")==0)
        {
            hyp_file = openfile("rules.hyp", "r");
            file_exists = TRUE;
        }
    }
    temp_file = openfile("hyp.tmp", "w");
    
    while (file_exists && !feof(hyp_file))
    {
        num_new_words = parse(new_hyp, hyp_file);
        
        if (num_new_words > 0)
        {
            lines_found += 1;
            if (exact_match(hyp, num_words, new_hyp, num_new_words - 2))
            {
                pos_score = atoi(new_hyp[num_new_words - 2]);
                neg_score = atoi(new_hyp[num_new_words - 1]);
                if (pos_or_neg == POS)
                {
                    pos_score += 1;
                    //itoa(pos_score,new_hyp[num_new_words - 2],10);
                    sprintf(new_hyp[num_new_words - 2],"%d",pos_score);
                }
                else if (pos_or_neg == NEG)
                {
                    neg_score += 1;
                    //itoa(neg_score,new_hyp[num_new_words - 1],10);
                    sprintf(new_hyp[num_new_words - 1],"%d",neg_score);
                }
                
            }
            
            for (a = 0; a < num_new_words; a++)
                fprintf(temp_file, "%s ", new_hyp[a]);
            fprintf(temp_file, "\n");
        }
    }
    
    
    if (file_exists)
        closefile(hyp_file);
    closefile(temp_file);
    
    if (type_flag == FACT && lines_found > 0 && file_exists)
        renamefile("hyp.tmp", "facts.hyp");
    else if (type_flag == RULE && lines_found > 0 && file_exists)
        renamefile("hyp.tmp", "rules.hyp");
    
} // End proc increase_hyp_score

//*********************************************************************

void collect_hypotheses(void)
{
    // Read through the temp facts and rules files and collect new hypotheses.
    
    int num_words = 0;
    char fact[SENTLEN][WORDSIZE];
    FILE *temp_file;
    
    if (access_file("facts.tmp")==0)
    {
        temp_file = openfile("facts.tmp", "r");
        
        while (!feof(temp_file))
        {
            num_words = 0;
            num_words = parse(fact, temp_file)-1;
            if (strcmp(fact[num_words], "l") == 0 && num_words > 0)
                store_hypothesis(fact, num_words, FACT, SKIP_TEMP);
            // Erase fact from temp facts file
        }
        
        closefile(temp_file);
    }
    
    erase_temp_fact_file(); // Erase the temp facts file
    
    if (access_file("rules.tmp")==0)
    {
        temp_file = openfile("rules.tmp", "r");
        
        while (!feof(temp_file))
        {
            num_words = 0;
            num_words = parse(fact, temp_file);
            if (num_words > 0)
                record_hypothesis(fact, num_words, RULE);
        }
        
        closefile(temp_file);
    }
    
} // End proc collect_hypotheses

//*********************************************************************************

/*
void erase_hypothesis(char text[SENTSIZE])
{
	char hyp[SENTLEN][WORDSIZE];
	char new_hyp[SENTLEN][WORDSIZE];
	int hyp_words, num_new_words, a;
	int file_exists = FALSE, hyp_found = FALSE, i, c;
	FILE *hyp_file;
	FILE *temp_file;
    
	//printf("Enter proc erase_hypothesis\n");
	
	hyp_words = parse(hyp, NULL, text);
	preprocess(hyp, hyp_words);             // Preprocess input
	for (i = 0; i < hyp_words; i++)
		for (c = 0; c < WORDSIZE; c++)
			hyp[i][c] = tolower(hyp[i][c]);
	
	if( access_file("facts.hyp")==0)
	{
		hyp_file = openfile("facts.hyp", "r");
		file_exists = TRUE;
	}
	
	temp_file = openfile("hyp.tmp", "w");
    
	while (file_exists && !feof(hyp_file))
	{
		num_new_words = parse(new_hyp, hyp_file, NULL);
		if (exact_match(hyp, hyp_words, new_hyp, num_new_words - 2))
			hyp_found = TRUE;
		else
		{
			for (a = 0; a < num_new_words; a++)
				fprintf(temp_file, "%s ", new_hyp[a]);
			fprintf(temp_file, "\n");
		}
	}
	
	closefile(hyp_file);
	closefile(temp_file);
	
	if (file_exists && hyp_found)
	{
		for (a = 0; a < hyp_words; a++)
			add_response_word(hyp[a]);
		add_response_word(":");
		add_response_word("Fact");
		add_response_word("erased.");
		add_response_word("\n");
	}
	else
	{
		add_response_word("Fact");
		add_response_word("not");
		add_response_word("found");
		add_response_word("\n");
	}
	
	reply();
	
	if (file_exists && hyp_found)
		renamefile("hyp.tmp", "facts.hyp");
    
    
	//printf("Exit proc erase_hypothesis\n");
    
}
*/