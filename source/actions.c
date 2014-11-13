// Actions.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "snlup.h"
#include "externs.h"
#include "phrases.h"
#include "respond.h"
#include "facts.h"
#include "utilities.h"
#include "scripts.h"
#include "objects.h"
#include "groups.h"
#include "rules.h"
#include "logic.h"
#include "io.h"
#include "actions.h"


void do_action(char action[SENTLEN][WORDSIZE], int num_action_words, char
               sentence[SENTLEN][WORDSIZE], int num_sent_words)
{
    // Execute the action passed in.
    // Dependency: Phrase sentence must reside in Group1 in order to find input purpose.
    
    FILE *openfile(char filename[FILELEN], char flags[3]);
    int i = 0, b = 0, c = 0, d = 0, e = 0;
    int skip = FALSE, answer_yesno = FALSE;
    int num_answer_words = 0, num_purpose_words = 0;
    int num_phrase_words = 0, num_emotion_words = 0;
    int num_facts = 0, num_ipurpose_words = 0, ipurpose_type = 0;
    int yesno = NO, answered = FALSE;
    int save_num_object1_words = 0, object1_saved = FALSE;
    // int finished = FALSE, lastc = 0;
    int ask = FALSE;
    unsigned long lmaint;
    char *endptr;
    char answer[SENTLEN][WORDSIZE];
    char phrase[SENTLEN][WORDSIZE];
    char purpose[SENTLEN][WORDSIZE];
    char emotion[SENTLEN][WORDSIZE];
    char script_file[FILELEN];
    char save_object1[SENTLEN][WORDSIZE];
    char facts_to_find[SENTLEN][WORDSIZE];
    char ipurpose[SENTLEN][WORDSIZE];
    // FILE *unknown_file;
    int thinking = FALSE;
	
    // Normalize Case
    for (i = 0; i < num_action_words; i++)
        for (c = 0; c < WORDSIZE; c++)
            action[i][c] = tolower(action[i][c]);
    
    // Check for intercepted action here
    if (intercept)
        if (intercept == ACTION)
            write_intercept(action, num_action_words);
    
    // Find input purpose
    num_ipurpose_words = choose_phrase(1, PURPOSE, ipurpose, GROUP1);  // Set purpose
    ipurpose_type = decode_purpose_type(ipurpose, num_ipurpose_words); // Set purpose type
	
    // Pre-defined Actions
    
    // Check for yesno question
    if (strcmp(action[num_action_words-1], "yesno") == 0)
    {
        answer_yesno = TRUE;
        
        // Check for A implies A special Case
        if ((ipurpose_type == SEEIFAIMPLIESB || ipurpose_type == SEEIFBIMPLIESA) &&
            (exact_match(object1, num_object1_words, object2, num_object2_words) == TRUE))
        {
            answered = TRUE;
            question_answered = TRUE;
            yesno = YES;
            add_response_word("Yes"); // Hard-Coded response for now
            add_response_word("\n");
            reply();
            skip = TRUE;
        }
        else
            answered = FALSE;
        num_action_words = num_action_words - 1;
    }
    
    
    /*  if (strcmp(action[0], "undefined") == 0)
     {
     unknown_file = openfile("unknown.frs", "a");
     for (i = 0; i < num_sent_words; i++)
     fprintf(unknown_file, "%s ", sentence[i]);
     fprintf(unknown_file, "\n");
     closefile(unknown_file);
     }
     */
    
    // Lets do some math
    if (strcmp(action[0], "add") == 0 || strcmp(action[0], "subtract") == 0
        || strcmp(action[0], "multiply") == 0 || strcmp(action[0], "divide") == 0)
        do_math(action, num_action_words);
    
    if (strcmp(action[0], "replace") == 0)
    {
        if (strcmp(action[1], "last") == 0)
            replace_object(LAST, sentence, num_sent_words);
    }
	
    if (strcmp(action[0], "none") == 0)
        skip = TRUE;                      // Or else it will try to respond with a purpose of "none"
	
    if (strcmp(action[0], "exit") == 0)
        if (running_script > 0)
            running_script=0;
        else
            done_looping = TRUE;
        else if (strcmp(action[0], "set") == 0)
        {
            if (strcmp(action[1], "maint") == 0)
            {
                if (strcmp(object1[0], "none") == 0)
                {
                    maint = 0;
                    alldebug = 0;
                    printf("\nALL MAINTENANCE OPTIONS DEACTIVATED\n");
                }
                else
                {
                    lmaint = strtoul(object1[0], &endptr, 10);
                    maint = (int)lmaint;
                    if (maint == 0)
                        alldebug = 1;
                    printf("\nMAINTENANCE OPTION # %d ACTIVATED\n", maint);
                }
                //cspause();
            }
        }
        else if (strcmp(action[0], "record") == 0)
        {
            if (strcmp(action[1], "fact") == 0)
            {
                if (strcmp(action[2], "permanent") == 0)
                    record_fact(sentence, num_sent_words, PERMANENT, LEARNED);
                else if (strcmp(action[2], "temporary") == 0)
                {
                    if (rule_fired)
                        record_fact(sentence, num_sent_words, TEMP, GEN);
                    else
                        record_fact(sentence, num_sent_words, TEMP, LEARNED);
                }
                reduce_objects();
                if (!rule_fired)
                {
                    strcpy(action[0], "acknowledge");
                    num_action_words = 1;
                }
            }
            else if (strcmp(action[1], "rule") == 0)
            {
                if (strcmp(action[2], "permanent") == 0)
                    record_rule(sentence, num_sent_words, PERMANENT);
                else if (strcmp(action[2], "temporary") == 0)
                    record_rule(sentence, num_sent_words, TEMP);
                test_and_fire_rule(FALSE);
                strcpy(action[0], "acknowledge");
                num_action_words = 1;
            }
            else if (strcmp(action[1], "adjective") == 0)
            {
                // Record adjective as a fact
                for (i = 0; i < num_object1_words; i++)
                    strcpy(answer[i], object1[i]);
                num_answer_words = num_object1_words;
                strcpy(answer[num_answer_words], "is");
                num_answer_words++;
                strcpy(answer[num_answer_words], action[2]);
                num_answer_words++;
                record_fact(answer, num_answer_words, TEMP, GEN);
                skip = TRUE;
            }
        }
        else if (strcmp(action[0], "find") == 0 && !question_answered)
        {
            if (strcmp(action[1], "one") == 0 && num_action_words == 3)
                c = find_answers_one_object(sentence, num_sent_words);
            else if (strcmp(action[1], "one") == 0 && strcmp(action[3], "plus") == 0)
            {
                // Add object1 to facts_to_find
                num_facts = 0;
                for ( i = 0; i < num_object1_words; i++)
                {
                    strcpy(facts_to_find[i], object1[i]);
                    num_facts +=1;
                }
                // Add plus words to facts_to_find
                for (i = 4; i < num_action_words; i++)
                {
                    strcpy(facts_to_find[num_facts], action[i]);
                    num_facts +=1;
                }
                c = find_facts_to_find(sentence, num_sent_words, facts_to_find, num_facts);
            }
            if (strcmp(action[1], "two") == 0)
            {
                c = find_answers(sentence, num_sent_words);
                
                if (answer_yesno)
                {
                    if( c > 0)
                        yesno = YES;
                    else
                        yesno = NO;
                }
            }
            if (strcmp(action[1], "all") == 0 /* || strcmp(action[1], "two") == 0 */ )
            {
                //*
                if( strcmp(action[3], "plus") == 0)  // lookup additional plus words
                {
                    for (i = 0; i < num_object1_words; i++)  // Save object1
                        strcpy(save_object1[i], object1[i]);
                    save_num_object1_words = num_object1_words;
                    object1_saved = TRUE;
                    
                    for (i = 4; i < num_action_words; i++) // Add plus words to object1
                    {
                        strcpy(object1[num_object1_words], action[i]);
                        num_object1_words = num_object1_words + 1;
                    }
                }
                //*/
                // Add object1 to facts_to_find
                for (i = 0; i < num_object1_words; i++)
                {
                    strcpy(facts_to_find[i], object1[i]);
                    num_facts +=1;
                }
                // Add object2 to facts_to_find
                for (e = 0; e < num_object2_words; e++)
                {
                    strcpy(facts_to_find[i+e], object2[e]);
                    num_facts +=1;
                }
                c = find_answers_one_object(sentence, num_sent_words);
                if ( c == 0)
                    yesno = NO;
                else
                    yesno = YES;
                
                if (object1_saved) // Restore object1 if plus words looked up
                {
                    for (i = 0; i < save_num_object1_words; i++)
                        strcpy(object1[i], save_object1[i]);
                    num_object1_words = save_num_object1_words;
                    object1_saved = FALSE;
                }
                
            }
            
            if (c > 1 && answer_yesno)
            {
                d = choose_best_response(sentence, num_sent_words, c, GROUP1);
                if( d > 0)  // If there is a best response then use it else use c
                    c = d;
            }
            /*
             if (c == 0)
             {
             c = get_thoughts();
             thinking = TRUE;
             }
             */
            b = c;
            c = prune_group_file("group.grp", b, ELIMDUPS, 0);
            
            if (c > 0 )
            {
                for (d = 1; d <= c; d++) // Experiment - Multiple answers
                {
                    num_answer_words = get_phrase(d, RESPONSE, answer, GROUP1);
                    
                    if(intercept == PHRASE ||
                       intercept == PURPOSE ||
                       intercept == EMOTION)
                    {
                        lookup_phrase(answer, num_answer_words, 100, PHRASE, GROUP2);
                        // lookup the phrase
                        num_phrase_words = choose_phrase(1, PHRASE, phrase, GROUP2);
                        // Pick first matched phrase
                        num_action_words = choose_phrase(1, ACTION, action, GROUP2);
                        // Set action, Pick first match
                        num_purpose_words = choose_phrase(1, PURPOSE, purpose, GROUP2);
                        // Set purpose, Pick first match
                        num_emotion_words = choose_phrase(1, EMOTION, emotion, GROUP2);
                        // Set emotion, Pick first match
                    }
                    // Check for intercept
                    if (intercept == PHRASE)
                        write_intercept(phrase, num_phrase_words);
                    if (intercept == PURPOSE)
                        write_intercept(purpose, num_purpose_words);
                    if (intercept == EMOTION)
                        write_intercept(emotion, num_emotion_words);
                    
                    // Action and Response intercepts are done elsewhere
                    
                    num_response_words = 0; // Clear word counts first
                    
                    if (answer_yesno)
                    {
                        if (check_yesno_answer(sentence, num_sent_words,
                                               answer, num_answer_words) == FALSE)
                            yesno = NO;
                        else
                            yesno = YES;
                        
                        if (yesno == YES)
                            add_response_word("Yes,");
                        else if (yesno == NO)
                            add_response_word("No,");
                        answered = TRUE;
                    }
                    
                    if ( thinking )
                    {
                        add_response_word("I");
                        add_response_word("think");
                        add_response_word("that");
                    }
                    
                    for (i = 0; i < num_answer_words; i++)
                        add_response_word(answer[i]);
                    add_response_word("\n");
                    reply();
                    skip = TRUE;
                    question_answered = TRUE;
                } // End for loop (multiple answers)
                
                if (answered)
                    answer_yesno = FALSE;
                
            } // End if c>0
            else if (answer_yesno)
            {
                if (yesno == YES)
                    add_response_word("Yes\n");
                else if (yesno == NO)
                    add_response_word("No\n");
                answered = TRUE;
                reply();
                skip = TRUE;
                question_answered = TRUE;
                //strcpy(action[0], "say");
                //strcpy(action[1], "not");
                //strcpy(action[2], "sure");
                //num_action_words = 3;
            }
            else
            {
                strcpy(action[0], "say");
                strcpy(action[1], "dont");
                strcpy(action[2], "know");
                ask = TRUE;
                num_action_words = 3;
            }
        } // End else if find
        else if (strcmp(action[0], "reduce") == 0)
            skip = TRUE;
        else if (strcmp(action[0], "run") == 0)
        {
            if (num_action_words == 3)
                strcpy(script_file, action[2]);
            else
                strcpy(script_file, object1[0]);
            run_script(script_file, FALSE);
        }
        else if (strcmp(action[0], "read") == 0)
        {
            if (num_action_words == 3)
                strcpy(script_file, action[2]);
            else
                strcpy(script_file, object1[0]);
            run_script(script_file, TRUE);
        }
    
    // Dynamically-defined Actions
	
    if (!skip)
        i = get_response(action, num_action_words);
    
    if (i > 0 && !skip)
    {
        num_purpose_words = choose_phrase(i, RESPONSE, purpose, GROUP1);
        i = lookup_phrase(purpose, num_purpose_words, 100, PURPOSE, GROUP1);
        // lookup a purpose
        num_answer_words = choose_phrase(i, PHRASE, answer, GROUP1);
        
        num_phrase_words = choose_phrase(1, PHRASE, phrase, GROUP1);
        // Pick first matched phrase
        num_action_words = choose_phrase(1, ACTION, action, GROUP1);
        // Set action, Pick first match
        num_purpose_words = choose_phrase(1, PURPOSE, purpose, GROUP1);
        // Set purpose, Pick first match
        num_emotion_words = choose_phrase(1, EMOTION, emotion, GROUP1);
        // Set emotion, Pick first match
        
        // Check for intercept
        if (intercept == PHRASE)
            write_intercept(phrase, num_phrase_words);
        if (intercept == PURPOSE)
            write_intercept(purpose, num_purpose_words);
        if (intercept == EMOTION)
            write_intercept(emotion, num_emotion_words);
        // Action and Response intercepts are done elsewhere
        
        num_response_words = 0; // Clear word counts first
        for (i = 0; i < num_answer_words; i++)
            add_response_word(answer[i]);
        if (ask)
        {
            add_response_word(",");
            for (i = 0; i < num_sent_words; i++)
                add_response_word(sentence[i]);
            add_response_word("?");
            ask = FALSE;
        }
        add_response_word("\n");
        reply();
    }
	object2_is_not_a_true_object = FALSE;
	
} // End proc do_action

//*******************************************************************************

int find_answers(char sentence[SENTLEN][WORDSIZE], int num_sent_words)
{
	int c = 0, lastc = 0;
    int finished = FALSE;
    
	c = look_for_relative_facts(ALL);
    
	if(c == 0)
	{
        while(!finished)
        {        
            generate_more_facts(sentence, num_sent_words);
            c = look_for_relative_facts(ALL);
            if(lastc == c)
                break;
            lastc = c;
        }
	}
    
	return(c);
    
} // End proc find_answers

//*********************************************************************************

int find_answers_one_object(char sentence[SENTLEN][WORDSIZE], int num_sent_words)
{
	int finished = FALSE;
	int b = 0, c = 0, lastc = 0;
    
	//b = find_all_facts(object1, num_object1_words, NEW, ALL, GROUP1);
	//b = find_fact(object1, num_object1_words, NEW, MATCH_OBJECT, GROUP1);
	b = find_fact(object1, num_object1_words, NEW, ALL, GROUP1);
	
    if(b == 0)
	{
        while(!finished)
        {        
            generate_more_facts(sentence, num_sent_words);
            //c = find_all_facts(object1, num_object1_words, NEW, ALL, GROUP1);
            c = find_fact(object1, num_object1_words, NEW, MATCH_OBJECT, GROUP1);        
            //c = prune_group_file("group.grp", b, ELIMDUPS, 0);
            if(lastc == c)
                finished = TRUE;
            lastc = c;
        }
	} 
    else
        c = b;
    
	return(c);
    
} // End proc find_answers_one_object

//*********************************************************************************

int find_facts_to_find(char sentence[SENTLEN][WORDSIZE], int num_sent_words, char facts_to_find[SENTLEN][WORDSIZE], int num_facts)
{
	int finished = FALSE;
	int b = 0, c = 0, lastc = 0;
	
	b = find_all_facts(facts_to_find, num_facts, NEW, ALL, GROUP1);
	
	if(b == 0)
	{
		while(!finished)
		{        
			generate_more_facts(sentence, num_sent_words);
			c = find_all_facts(facts_to_find, num_facts, NEW, ALL, GROUP1);
			//c = prune_group_file("group.grp", b, ELIMDUPS, 0);
			if(lastc == c)
				finished = TRUE;
			lastc = c;
		}
	} 
    else
		c = b;
	
	return(c);
	
} // End proc find_facts_to_find

//*********************************************************************************

void do_math(char action[SENTLEN][WORDSIZE], int num_action_words)
{
    float answer=0.0;
    float result=0.0;
    char operand[WORDSIZE];
    char x[WORDSIZE];
    
    if (strcmp(action[0], "add") == 0)
    {
        answer = atof(object1[0]) + atof(object2[0]);
        sprintf(operand, "+");
    }
    if (strcmp(action[0], "subtract") == 0)
    {
        answer = atof(object1[0]) - atof(object2[0]);
        sprintf(operand, "-");
    }
    if (strcmp(action[0], "multiply") == 0)
    {
        answer = atof(object1[0]) * atof(object2[0]);
        sprintf(operand, "*");
    }
    if (strcmp(action[0], "divide") == 0)
    {
        answer = atof(object1[0]) / atof(object2[0]);
        sprintf(operand, "/");
    }
    
    result = answer - (int)answer; // see if there is a decimal other than zero
    
    if (result != 0)
        sprintf(x, "%f", answer);
    else
        sprintf(x, "%d", (int)answer);
    
    add_response_word(object1[0]);
    add_response_word(operand);
    add_response_word(object2[0]);
    add_response_word("=");
    add_response_word(x);
    add_response_word("\n");
    reply();
    
}