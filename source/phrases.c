// phrases.c

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <ctype.h>
#include "snlup.h"
#include "externs.h"
#include "io.h"
#include "utilities.h"
#include "phrases.h"
#include "sentences.h"
#include "objects.h"
#include "actions.h"
#include "facts.h"
#include "groups.h"

int choose_phrase(int num_phrases, int phrase_flag, char
                  output[SENTLEN][WORDSIZE], int group_num)
{
    // Randomly chooses a phrase from the group file. Num phrases is
    // the number of phrases in the group file to choose from, and
    // loads the phrase type of phrase_flag into output. 1=Phrase,
    // 2=Action, 3=Purpose, 4=Emotion, 5=Response. If there is more than
    // one phrase to choose from, the default phrase (always the last one)
    // will not be importd in the random pick. (except for responses)
    // Returns the number of words in the chosen phrase.
    
    int num_phrase_words = 0, num_action_words = 0;
    int num_purpose_words = 0, num_emotion_words = 0;
    int loop = 0, num_to_read = 0;
    char phrase[SENTLEN][WORDSIZE];
    char action[SENTLEN][WORDSIZE];
    char purpose[SENTLEN][WORDSIZE];
    char emotion[SENTLEN][WORDSIZE];
    FILE *group_file;
    
    if (num_phrases <= 0)
        return (0);  // Failsafe
    
    if (num_phrases == 1)
        num_to_read = 1;
    else if (phrase_flag != RESPONSE)
        num_to_read = random_num(1, num_phrases - 1);
    else
        num_to_read = random_num(1, num_phrases);
    
    if (group_num == GROUP1)
        group_file = openfile("group.grp", "r");
    else if (group_num == GROUP2)
        group_file = openfile("group2.grp", "r");
    
    if (phrase_flag == RESPONSE)
    {
        for (loop = 0; loop < num_to_read; loop++)
            num_phrase_words = parse(phrase, group_file);
    }
    else
    {
        
        for (loop = 0; loop < num_to_read; loop++)
        {
            num_phrase_words = parse(phrase, group_file);
            num_action_words = parse(action, group_file);
            num_purpose_words = parse(purpose, group_file);
            num_emotion_words = parse(emotion, group_file);
        }
    }
    closefile(group_file);
    
    switch (phrase_flag)
    {
        case PHRASE:
        {
            for (loop = 0; loop < num_phrase_words; loop++)
                strcpy(output[loop], phrase[loop]);
            return (num_phrase_words);
        }
        case ACTION:
        {
            for (loop = 0; loop < num_action_words; loop++)
                strcpy(output[loop], action[loop]);
            return (num_action_words);
        }
        case PURPOSE:
        {
            for (loop = 0; loop < num_purpose_words; loop++)
                strcpy(output[loop], purpose[loop]);
            return (num_purpose_words);
        }
        case EMOTION:
        {
            for (loop = 0; loop < num_emotion_words; loop++)
                strcpy(output[loop], emotion[loop]);
            return (num_emotion_words);
        }
        case RESPONSE:
        {
            for (loop = 0; loop < num_phrase_words; loop++)
                strcpy(output[loop], phrase[loop]);
            return (num_phrase_words);
        }
            
    } // End switch phrase_flag
    
    return (0);
    
} // End proc choose_phrase

//******************************************************************************

int get_phrase(int phrase_num, int phrase_flag, char
               output[SENTLEN][WORDSIZE], int group_num)
{
    // Get a phrase by number from the group file. Phrases num is
    // the number of the phrase in the group file to get.
    // Returns the number of words in the chosen phrase.
    
    int num_phrase_words = 0, num_action_words = 0;
    int num_purpose_words = 0, num_emotion_words = 0;
    int loop = 0, num_to_read = 0;
    char phrase[SENTLEN][WORDSIZE];
    char action[SENTLEN][WORDSIZE];
    char purpose[SENTLEN][WORDSIZE];
    char emotion[SENTLEN][WORDSIZE];
    FILE *group_file;
    
    if (phrase_num <= 0)
        return (0);
    
    num_to_read = phrase_num;
    
    if (group_num == GROUP1)
        group_file = openfile("group.grp", "r");
    else if (group_num == GROUP2)
        group_file = openfile("group2.grp", "r");
    
    if (phrase_flag == RESPONSE)
    {
        for (loop = 0; loop < num_to_read; loop++)
            num_phrase_words = parse(phrase, group_file);
    }
    else
    {
        
        for (loop = 0; loop < num_to_read; loop++)
        {
            num_phrase_words = parse(phrase, group_file);
            num_action_words = parse(action, group_file);
            num_purpose_words = parse(purpose, group_file);
            num_emotion_words = parse(emotion, group_file);
        }
    }
    closefile(group_file);
    
    switch (phrase_flag)
    {
        case PHRASE:
        {
            for (loop = 0; loop < num_phrase_words; loop++)
                strcpy(output[loop], phrase[loop]);
            return (num_phrase_words);
        }
        case ACTION:
        {
            for (loop = 0; loop < num_action_words; loop++)
                strcpy(output[loop], action[loop]);
            return (num_action_words);
        }
        case PURPOSE:
        {
            for (loop = 0; loop < num_purpose_words; loop++)
                strcpy(output[loop], purpose[loop]);
            return (num_purpose_words);
        }
        case EMOTION:
        {
            for (loop = 0; loop < num_emotion_words; loop++)
                strcpy(output[loop], emotion[loop]);
            return (num_emotion_words);
        }
        case RESPONSE:
        {
            for (loop = 0; loop < num_phrase_words; loop++)
                strcpy(output[loop], phrase[loop]);
            return (num_phrase_words);
        }
            
    } // End switch phrase_flag
    
    return (0);
    
} // End proc get_phrase

//******************************************************************************

int expand_new_phrases(void)
{
    // Reads new phrases from the unknown phrase file. Expands each phrase
    // to all its <object> phrase <object> possibilities. Output goes to a
    // group file. Since this procedure is to be called during freetime
    // processing, any keyboard input will interrupt it. Returns 0 if
    // processing completed, else returns 1.
    
    FILE *phrase_file,  *expanded_file;
    
    char phrase[SENTLEN][WORDSIZE];
    char object[WORDSIZE] = "<object>";
    
    int done = FALSE, num_phrase_words = 0;
    int loop1 = 0, loop2 = 0, loop3 = 0;
    
    //  if (kbhit())
    //  return (1);
    
    phrase_file = openfile("unknown.frs", "r");
    expanded_file = openfile("expanded.frs", "w");
    
    while (!done)
        // Loop through all phrases in unknown phrase file
    {
        num_phrase_words = parse(phrase, phrase_file);
        
        // object in front
        for (loop1 = 1; loop1 < num_phrase_words; loop1++)
        {
            fprintf(expanded_file, "%s ", object);
            for (loop2 = loop1; loop2 < num_phrase_words; loop2++)
                fprintf(expanded_file, "%s ", phrase[loop2]);
            fprintf(expanded_file, "\n");
        }
        
        // object in rear
        for (loop1 = 1; loop1 < num_phrase_words; loop1++)
        {
            for (loop2 = 0; loop2 < num_phrase_words - loop1; loop2++)
                fprintf(expanded_file, "%s ", phrase[loop2]);
            fprintf(expanded_file, "%s ", object);
            fprintf(expanded_file, "\n");
        }
        
        // object in middle front
        for (loop1 = 1; loop1 < num_phrase_words - 1; loop1++)
        {
            fprintf(expanded_file, "%s ", phrase[0]);
            for (loop2 = 1; loop2 < num_phrase_words - loop1 - 1; loop2++)
                fprintf(expanded_file, "%s ", phrase[loop2]);
            fprintf(expanded_file, "%s ", object);
            fprintf(expanded_file, "%s ", phrase[num_phrase_words - 1]);
            fprintf(expanded_file, "\n");
        }
        
        // object in middle rear
        for (loop1 = 1; loop1 < num_phrase_words - 2; loop1++)
        {
            fprintf(expanded_file, "%s ", phrase[0]);
            fprintf(expanded_file, "%s ", object);
            for (loop2 = 1+loop1; loop2 < num_phrase_words; loop2++)
                fprintf(expanded_file, "%s ", phrase[loop2]);
            fprintf(expanded_file, "\n");
        }
        
        // objects on ends
        for (loop3 = 0; loop3 < num_phrase_words / 2; loop3++)
        {
            // Front
            for (loop1 = 1; loop1 < num_phrase_words - 1-loop3; loop1++)
            {
                if ((loop1 + loop3) < (num_phrase_words - 1-loop3))
                {
                    fprintf(expanded_file, "%s ", object);
                    for (loop2 = loop1 + loop3; loop2 < num_phrase_words - 1-loop3; loop2
                         ++)
                        fprintf(expanded_file, "%s ", phrase[loop2]);
                    fprintf(expanded_file, "%s ", object);
                    fprintf(expanded_file, "\n");
                }
            }
            
            // objects on ends
            // Rear
            for (loop1 = 1; loop1 < num_phrase_words - 2-loop3; loop1++)
            {
                if ((1+loop3) < (num_phrase_words - loop1 - 1-loop3))
                {
                    fprintf(expanded_file, "%s ", object);
                    for (loop2 = 1+loop3; loop2 < num_phrase_words - loop1 - 1-loop3;
                         loop2++)
                        fprintf(expanded_file, "%s ", phrase[loop2]);
                    fprintf(expanded_file, "%s ", object);
                    fprintf(expanded_file, "\n");
                }
            }
        }
        
        if (feof(phrase_file))
            done = TRUE;
        /*    if (kbhit())
         {
         closefile(phrase_file);
         closefile(expanded_file);
         return (1);
         }
         */
    } // End while - looping through all phrases in unknown file
    
    closefile(phrase_file);
    closefile(expanded_file);
    
    return (0);
} // End Proc expand_new_phrases

//******************************************************************************

int lookup_phrase(char phrasein[SENTLEN][WORDSIZE], int num_phrasein_words, int
                  threshold, int phrase_type, int group_num)
{
    // Lookup a phrase in the phrase file that matches phrasein with the same
    // phrase_type, 1=phrase, 2=action, 3=purpose, 4=emotion. Must match a
    // phrase by threshold percentage. Places the matches in the group file,
    // and returns the number of matches.
    
    // Globals: char frsfiles[FILELEN][MAXFILES], int num_frs_files
    
    int num_phrase_words = 0, num_action_words = 0, i = 0, done = 0;
    int num_purpose_words = 0, num_emotion_words = 0, percent_matched = 0;
    int num_matched = 0, filenum = 0;
    char current_file[FILELEN];
    char phrase[SENTLEN][WORDSIZE];
    char action[SENTLEN][WORDSIZE];
    char purpose[SENTLEN][WORDSIZE];
    char emotion[SENTLEN][WORDSIZE];
    FILE *phrase_file;
    FILE *group_file;
    
    if (group_num == GROUP1)
        group_file = openfile("group.grp", "w");
    else if (group_num == GROUP2)
        group_file = openfile("group2.grp", "w");
    
    for (filenum = num_frs_files - 1; filenum >= 0; filenum--)
    {
        // For all phrase files
        //printf("frsfile: %s\n", frsfiles[0]);
        sprintf(current_file, "%s", frsfiles[filenum]);
        //printf("frsfile: %s\n", current_file);
        phrase_file = openfile(current_file, "r");
        done = FALSE; // Done with this file?
        while (!done)
        {
            // Loop thru file
            num_phrase_words = parse(phrase, phrase_file);
            num_action_words = parse(action, phrase_file);
            num_purpose_words = parse(purpose, phrase_file);
            num_emotion_words = parse(emotion, phrase_file);
            
            switch (phrase_type)
            {
                case PHRASE:
                {
                    percent_matched = match_phrase(phrasein, num_phrasein_words, phrase,
                                                   num_phrase_words);
                    break;
                }
                case ACTION:
                {
                    percent_matched = match_phrase(phrasein, num_phrasein_words, action,
                                                   num_action_words);
                    break;
                }
                case PURPOSE:
                {
                    percent_matched = match_phrase(phrasein, num_phrasein_words,
                                                   purpose, num_purpose_words);
                    break;
                }
                case EMOTION:
                {
                    percent_matched = match_phrase(phrasein, num_phrasein_words,
                                                   emotion, num_emotion_words);
                    break;
                }
            } // End switch on phrase_type
            
            
            if (percent_matched >= threshold  && percent_matched <= 100 )
                // Don't take anything over 100%
            {
                for (i = 0; i < num_phrase_words; i++)
                    fprintf(group_file, "%s ", phrase[i]);
                fprintf(group_file, "\n");
                for (i = 0; i < num_action_words; i++)
                    fprintf(group_file, "%s ", action[i]);
                fprintf(group_file, "\n");
                for (i = 0; i < num_purpose_words; i++)
                    fprintf(group_file, "%s ", purpose[i]);
                fprintf(group_file, "\n");
                for (i = 0; i < num_emotion_words; i++)
                    fprintf(group_file, "%s ", emotion[i]);
                fprintf(group_file, "\n");
                num_matched++;
            }
            
            if (feof(phrase_file))
                done = TRUE;
        } // End loop thru file
        
        closefile(phrase_file);
    } // End loop thru all files
    
    closefile(group_file);
    
    return (num_matched);
    
} // End proc lookup_phrase

//******************************************************************************

int check_for_plural_object(char sentence[SENTLEN][WORDSIZE], int num_sentence_words, char
                            phrase[SENTLEN][WORDSIZE], int num_phrase_words)
{
    int c=0, foundit=0;
    FILE *swords_file;
    int num_words=0;
    char word[SENTLEN][WORDSIZE];
    int sword=0;
    int found_sword=FALSE;
    
    // See if object ends in 's'
    if (!stripped_s && !added_s)
    {
        
        for (c = 0; c < WORDSIZE-1; c++)
        {
            // Find end of word
            if (phrase[num_phrase_words-1][c] == '\0')
                break;
        }
        
        // If word ends in s
        if (phrase[num_phrase_words-1][c-1] == 's')
        {
            //printf("Strip an s from the object\n");
            //printf("Object = %s\n", phrase[num_phrase_words-1]);
            
            // Try without s
            phrase[num_phrase_words-1][c-1] = '\0';
            //printf("New Object = %s\n", phrase[num_phrase_words-1]);
            stripped_s = TRUE;
        }
        else
        {
            //printf("Add an s to the object\n");
            //printf("Object = %s\n", phrase[num_phrase_words-1]);
            phrase[num_phrase_words-1][c] = 's';
            phrase[num_phrase_words-1][c+1] = '\0';
            //printf("New Object = %s\n", phrase[num_phrase_words-1]);
            added_s = TRUE;
        }
        
        // Make sure phrase is not a common word that ends in s
        
        swords_file = openfile("swords.txt", "r");
        
        while (!feof(swords_file))
        {
            num_words = parse(word, swords_file);
            sword = exact_match(word, num_words, phrase, num_phrase_words);
            if (sword)
            {
                found_sword = TRUE;
                break;
            }
        }
        
        closefile(swords_file);
        
        foundit = 0;
        if (!found_sword)
            foundit = match_phrase(sentence, num_sentence_words, phrase, num_phrase_words);
    }
    
    if (stripped_s)
        stripped_s = FALSE;
    if (added_s)
        added_s = FALSE;
    
    if (foundit == 100)
        return(foundit);
    else
        return(0);
}

//******************************************************************************

int match_phrase(char sentence[SENTLEN][WORDSIZE], int num_sentence_words, char
                 phrase[SENTLEN][WORDSIZE], int num_phrase_words)
{
    // Trys to match a phrase to a sentence. Returns percentage of
    // phrase words matched in the sentence. The basic matching algorithm
    // is based on the <object> phrase <object> system.
    
    int matched = 0, found = 0;
    int processing_object1 = 0, processing_object2 = 0;
    int object1_pos = 0, object2_pos = 0;
    int sentence_pos = 0, phrase_pos = 0;
    int object1_found = 0, object2_found = 0;
    int loop = 0, i = 0, c = 0, simple_search = TRUE;
    char save_object1[SENTLEN][WORDSIZE], save_object2[SENTLEN][WORDSIZE];
    char temp_sent[SENTLEN][WORDSIZE], temp_phrase[SENTLEN][WORDSIZE];
    int num_temp_sent_words = 0, num_temp_phrase_words = 0;
    int save_num_object1_words = 0, save_num_object2_words = 0;
    int found_object = 0, objects_found_in_sent = 0;
    int extra_words = 0;
    int return_value = 0;
    int made_it_to_end_of_phrase = FALSE;
    int made_it_to_end_of_sentence = FALSE;
    int failsafe = 0;
    int last_pass_for_phrase = 0, last_pass_for_sentence = 0;
    int obj1_pos_found = 0;
    int obj2_pos_found = 0;
    
    for (i = 0; i < num_sentence_words; i++)
        for (c = 0; c < WORDSIZE; c++)
            sentence[i][c] = tolower(sentence[i][c]);
    
    for (i = 0; i < num_phrase_words; i++)
        for (c = 0; c < WORDSIZE; c++)
            phrase[i][c] = tolower(phrase[i][c]);
    
    if (num_sentence_words < 1 || num_sentence_words > SENTLEN)
        return (0);
    
    if (num_phrase_words < 1 || num_phrase_words > SENTLEN)
        return (0);
    
    save_num_object1_words = num_object1_words;
    save_num_object2_words = num_object2_words;
    
    for (loop = 0; loop < num_object1_words; loop++)
        strcpy(save_object1[loop], object1[loop]);
    
    for (loop = 0; loop < num_object2_words; loop++)
        strcpy(save_object2[loop], object2[loop]);
    
    if (((strcmp(phrase[0], "<OBJECT>") == 0) || (strcmp(phrase[0], "<object>") ==
                                                  0)) && num_phrase_words == 1)
    {
        // A single <object> matches anything
        
        for (loop = 0; loop < num_sentence_words; loop++)
            strcpy(object1[loop], sentence[loop]);
        num_object1_words = num_sentence_words;
        return (100);
    } // End a single <object> matches everything
    
    simple_search = TRUE;
    for (loop = 0; loop < num_phrase_words; loop++)
    {
        if ((strcmp(phrase[loop], "<OBJECT>") == 0) || (strcmp(phrase[loop],
                                                               "<object>") == 0))
            simple_search = FALSE;
    }
    for (loop = 0; loop < num_sentence_words; loop++)
    {
        if ((strcmp(sentence[loop], "<OBJECT>") == 0) || (strcmp(sentence[loop],
                                                                 "<object>") == 0))
            simple_search = FALSE;
    }
    
    if (simple_search)
    {
        // Make working copies of sentence and phrase
        for (i = 0; i < num_sentence_words; i++)
            strcpy(temp_sent[i], sentence[i]);
        num_temp_sent_words = num_sentence_words;
        for (i = 0; i < num_phrase_words; i++)
            strcpy(temp_phrase[i], phrase[i]);
        num_temp_phrase_words = num_phrase_words;
        
        // Prune all duplicate words from each working copy
        num_temp_sent_words = prune_sentence(temp_sent, num_temp_sent_words);
        num_temp_phrase_words = prune_sentence(temp_phrase, num_temp_phrase_words);
    }
    found = 0;
    if (simple_search && num_temp_phrase_words <= num_temp_sent_words)
    {
        // Do a simple search #1
        for (sentence_pos = 0; sentence_pos < num_temp_sent_words; sentence_pos++)
        {
			for (phrase_pos = 0; phrase_pos < num_temp_phrase_words; phrase_pos++)
			{
				if (strcmp(temp_sent[sentence_pos], temp_phrase[phrase_pos]) == 0)
                    found++;
			}
        }
        
        if (found)
        {
            return_value = (found *100 / num_temp_phrase_words);
            return (return_value);
        }
        else
            return (check_for_plural_object(temp_sent, num_temp_sent_words, temp_phrase, num_temp_phrase_words));
    } // End Simple search #1
    
    found = 0;
    if (simple_search && num_temp_sent_words < num_temp_phrase_words)
    {
        // Simple search #2
        for (phrase_pos = 0; phrase_pos < num_temp_phrase_words; phrase_pos++)
        {
            for (loop = 0; loop < num_temp_sent_words; loop++)
                if (strcmp(temp_phrase[phrase_pos], temp_sent[loop]) == 0)
                    found++;
        }
        if (found)
        {
            return_value = (found *100 / num_temp_phrase_words);
            return (return_value);
        }
        else
            return (check_for_plural_object(temp_sent, num_temp_sent_words, temp_phrase, num_temp_phrase_words));
    } // End Simple Search #2
    
    sentence_pos = 0;
    phrase_pos = 0;
    failsafe = 0;
    
    while (!matched)
    {
        failsafe++;
        if (failsafe > 100)
            break;
        // Match phrase is hopelessly lost!!
        
        if (strcmp(phrase[phrase_pos], sentence[sentence_pos]) == 0)
        {
            found++;
            
            if (processing_object1)
                processing_object1 = FALSE;
            if (processing_object2)
                processing_object2 = FALSE;
            if (found == num_phrase_words)
            {
                matched = TRUE;
                break; /* break out of main while loop */
            }
            if (sentence_pos <= num_sentence_words - 2)
                sentence_pos++;
            if (phrase_pos <= num_phrase_words - 2)
                phrase_pos++;
            
        } // End if words match
        
        else
            // words don't match
        {
            if (((strcmp(phrase[phrase_pos], "<OBJECT>") == 0) || (strcmp
                                                                   (phrase[phrase_pos], "<object>") == 0)) && (strcmp
                                                                                                               (sentence[sentence_pos], phrase[phrase_pos]) != 0))
            {
                if (!processing_object1 && found_object == 0)
                {
                    processing_object1 = TRUE;
                    found_object++;
                    obj1_pos_found = phrase_pos;
                    // Experiment
                    //if (phrase_pos == 0 && sentence_pos == 0);
                    //else
                    if (phrase_pos <= num_phrase_words - 2)
                        phrase_pos++;
                }
                else
                {
                    if (!processing_object1 && !processing_object2)
                    {
                        processing_object1 = FALSE;
                        processing_object2 = TRUE;
                        obj2_pos_found = phrase_pos;
                        
                        if (phrase_pos <= num_phrase_words - 2)
                            phrase_pos++;
                        found_object++;
                    } // End if
                    
                    else if (processing_object1 && !processing_object2 && obj1_pos_found != phrase_pos)
                    {
                        processing_object1 = FALSE;
                        processing_object2 = TRUE;
                        if (phrase_pos <= num_phrase_words - 2)
                            phrase_pos++;
                        found_object++;
                    }
                    
                } // End else
                
            } // End if <object>
            
            if (processing_object1)
            {
                if (object1_pos >= num_sentence_words - 1)
                    processing_object1 = FALSE;
                
                if (strcmp(sentence[sentence_pos], "") != 0)
                {
                    if (!object1_found)
                    {
                        object1_found = TRUE;
                        objects_found_in_sent++;
                    }
                    sprintf(object1[object1_pos], "%s", sentence[sentence_pos]);
                    if (object1_pos <= num_sentence_words - 2)
                        object1_pos++;
                    if (sentence_pos <= num_sentence_words - 2)
                        sentence_pos++;
                    
                    if (num_phrase_words >= num_sentence_words)
                    {
                        if (object1_pos >= num_phrase_words)
                            // Removed -1
                            matched = TRUE;
                    }
                    else
                    {
                        if (object1_pos >= num_sentence_words)
                            // Removed -1
                            matched = TRUE;
                    }
                    
                } // end if sent word is not blank
                
            } // End if processing object 1
            
            else
            {
                // not processing object 1
                if (processing_object2)
                {
                    if (strcmp(sentence[sentence_pos], "") != 0)
                    {
                        if (!object2_found)
                        {
                            object2_found = TRUE;
                            objects_found_in_sent++;
                        }
                        strcpy(object2[object2_pos], sentence[sentence_pos]);
                        if (object2_pos <= num_sentence_words - 2)
                            object2_pos++;
                        if (sentence_pos <= num_sentence_words - 2)
                            sentence_pos++;
                        
                        if (sentence_pos == num_sentence_words)
                            matched = TRUE;
                        
                    } // End if sent word is not blank
                    
                } // End if  processing object2
                
                else
                {
                    // not processing object 2, either
                    
                    if (phrase_pos <= num_phrase_words - 2)
                        phrase_pos++;
                    if (sentence_pos <= num_sentence_words - 2)
                        sentence_pos++;
                    
                } // End else not processing object 2 either
                
            } // End else not processing object1
            
        } // End else words don't match
        
        if (last_pass_for_phrase)
            made_it_to_end_of_phrase = TRUE;
        if (phrase_pos >= num_phrase_words - 1)
            last_pass_for_phrase = TRUE;
        // Needs one more pass through loop
        
        if (last_pass_for_sentence)
            made_it_to_end_of_sentence = TRUE;
        if (sentence_pos >= num_sentence_words - 1)
            last_pass_for_sentence = TRUE;
        // Needs one more pass through loop
        
        /*
         if ((((found >= num_phrase_words - found_object) ||
         (made_it_to_end_of_phrase)) && (made_it_to_end_of_sentence) &&
         (objects_found_in_sent >= found_object)) || ((made_it_to_end_of_phrase)
         && (sentence_pos >= num_phrase_words - 1) && (found == 0)) || (
         (made_it_to_end_of_sentence) && (found == 0)) || (
         (made_it_to_end_of_phrase) && (found == 0)) || (made_it_to_end_of_phrase
         && made_it_to_end_of_sentence))
         */
        if ((((found >= num_phrase_words - found_object) ||
              (made_it_to_end_of_phrase)) && (made_it_to_end_of_sentence) &&
             (objects_found_in_sent >= found_object)) || ((made_it_to_end_of_phrase)
                                                          && (sentence_pos >= num_phrase_words - 1) && (found == 0) && (!processing_object1)) || (
                                                                                                                                                  (made_it_to_end_of_phrase) && (made_it_to_end_of_sentence) && (found == 0)) ||
            (made_it_to_end_of_phrase && made_it_to_end_of_sentence))
            
            
            matched = TRUE;
        
    } // End while matched
    
    if (num_phrase_words - found_object > 0)
    {
        if ((num_sentence_words > (found + object1_pos + object2_pos)) && (found +
                                                                           object1_pos + object2_pos > 0) && (found *100 / (num_phrase_words -
                                                                                                                            found_object)) == 100)
            extra_words = (num_sentence_words / (found + object1_pos + object2_pos));
        else
            if ((num_sentence_words > (found + object1_pos + object2_pos)) && (found
                                                                               + object1_pos + object2_pos > 0) && (found *100) == 100)
                extra_words = (num_sentence_words / (found + object1_pos + object2_pos));
    }
    
    if (num_phrase_words - found_object > 0)
    {
        if (extra_words != 0 && (found + extra_words == num_phrase_words -
                                 found_object))
            return_value = 0;
        else
            return_value = (found + extra_words) *100 / (num_phrase_words -
                                                         found_object);
    }
    else
    {
        if (num_phrase_words == found_object)
            return_value = (found + extra_words) *100;
        else
        {
            if ((found_object - found == num_phrase_words) && (num_sentence_words -
                                                               found == found_object))
                return_value = 100;
            else
                return_value = 0;
        } // End else
        
    } // End else
    
    if (return_value == 100)
    {
        if (found_object > 0 && object1_pos == 0)
        {
            if (num_phrase_words > num_sentence_words)
                return_value = (num_sentence_words *100) / num_phrase_words;
            else
                return_value = (num_phrase_words *100) / num_sentence_words;
        }
    }
    
    if (return_value == 100)
    {
        num_object1_words = object1_pos;
        num_object2_words = object2_pos;
        
    } // End if matched = 100%
    
    else
    {
        // Restore original objects
        
        num_object1_words = save_num_object1_words;
        num_object2_words = save_num_object2_words;
        
        for (loop = 0; loop < num_object1_words; loop++)
            strcpy(object1[loop], save_object1[loop]);
        
        for (loop = 0; loop < num_object2_words; loop++)
            strcpy(object2[loop], save_object2[loop]);
        
    } // End else restore original objects
    
    return (return_value);
    
} // End proc match_phrase

//******************************************************************************

int match_object(char sentence[SENTLEN][WORDSIZE], int num_sentence_words, char
				 phrase[SENTLEN][WORDSIZE], int num_phrase_words)
{
	int i = 0, c = 0,return_value=0, best=0, found=0;
	int loop = 0, sentence_pos=0, phrase_pos=0;;
	int foundit=0;
	
	for (i = 0; i < num_sentence_words; i++)
		for (c = 0; c < WORDSIZE; c++)
			sentence[i][c] = tolower(sentence[i][c]);
	
	for (i = 0; i < num_phrase_words; i++)
		for (c = 0; c < WORDSIZE; c++)
			phrase[i][c] = tolower(phrase[i][c]);
	
	if (num_sentence_words < 1 || num_sentence_words > SENTLEN)
		return (0);
	
	if (num_phrase_words < 1 || num_phrase_words > SENTLEN)
		return (0);
	
    
    // New Simple Search - see if the words are all in a row
    for (sentence_pos = 0; sentence_pos < num_sentence_words; sentence_pos++)
    {
        for (phrase_pos = 0; phrase_pos < num_phrase_words; phrase_pos++)
        {
            if (strcmp(sentence[sentence_pos], phrase[phrase_pos]) == 0)
            {
                foundit++;
                // Found first word, now see if all in a row
                for (loop = sentence_pos; loop < num_phrase_words-foundit+1+sentence_pos; loop++)
                    if (strcmp(sentence[loop], phrase[loop-sentence_pos+foundit-1]) == 0)
                        found++;
                if (found == num_phrase_words)
                    return (100);
                else
                {
                    if (found > best)
                        best = found;
                    found = 0;
                }
            }
            
        }
    }
    
    found = best;
	
	/*(
     // Do a simple search #1
     for (sentence_pos = 0; sentence_pos < num_sentence_words; sentence_pos++)
     {
     for (phrase_pos = 0; phrase_pos < num_phrase_words; phrase_pos++)
     {
     if (strcmp(sentence[sentence_pos], phrase[phrase_pos]) == 0)
     found++;
     }
     }
     
     if (found < best)
     found = best;
     */
    
	if (found)
    {
		return_value = (found *100 / num_phrase_words);
		return (return_value);
    }
    else
		return (0);
	
    
} // End proc match_object

//******************************************************************************

int exact_match(char sentence[SENTLEN][WORDSIZE], int num_sentence_words, char
                phrase[SENTLEN][WORDSIZE], int num_phrase_words)
{
    // Returns 1 if the two phrases match exactly; else returns 0
    // Not case sensitive
    
    int i = 0, exact = 0;
    int a = 0, b = 0;
    
    // Convert to lower case
    for (a = 0; a < SENTLEN; a++)
        for (b = 0; b < WORDSIZE; b++)
        {
            phrase[b][a] = tolower(phrase[b][a]);
            sentence[b][a] = tolower(sentence[b][a]);
        }
    
    if (num_sentence_words == num_phrase_words)
    {
        for (i = 0; i < num_sentence_words; i++)
            if (strcmp(sentence[i], phrase[i]) == 0)
                exact++;
    }
    if (exact == num_sentence_words && num_sentence_words == num_phrase_words)
        exact = 1;
    else
        exact = 0;
    
    return (exact);
    
} // End exact_match

//******************************************************************************

int plug_in_objects(char phrase[SENTLEN][WORDSIZE], int num_phrase_words,
					char obj1[SENTLEN][WORDSIZE], int num_obj1_words,
					char obj2[SENTLEN][WORDSIZE], int num_obj2_words)
{
    // Takes the input phrase and replaces <object> with obj1 and obj2
    // Returns the number of words in the new phrase.
    
    int  i = 0, a = 0, b = 0;
    int obj1_found = FALSE, obj2_found = FALSE;
    char org_phrase[SENTLEN][WORDSIZE];
    int num_org_phrase_words = 0;
    
    for (i = 0; i < num_phrase_words; i++)	 // Save phrase coming in
        strcpy(org_phrase[i], phrase[i]);
    num_org_phrase_words = num_phrase_words;
    
    for (i = 0; i < num_org_phrase_words; i++) // Loop through org phrase
    {
        if (strcmp(org_phrase[i], "<object>")==0)
        {
            if (obj1_found == FALSE)
            {
                // plug in object 1
                for (a = 0; a < num_obj1_words; a++)
                    strcpy(phrase[i+a], obj1[a]);
                num_phrase_words += num_obj1_words - 1;
                obj1_found = TRUE;
            }
            else
            {
                // plug in object 2
                for (b = 0; b < num_obj2_words; b++)
                    strcpy(phrase[i+a-1+b], obj2[b]);
                num_phrase_words += num_obj2_words - 1;
                obj2_found = TRUE;
            }
        }
        else
        {
            if (obj1_found == TRUE && obj2_found == FALSE)
                strcpy(phrase[i+a-1], org_phrase[i]);
            
            if (obj1_found == TRUE && obj2_found == TRUE)
                strcpy(phrase[i+a+b], org_phrase[i]);
        }
    }
    
    return (num_phrase_words);
    
} // End plug_in_objects

//******************************************************************************

int choose_best_response(char org_input[SENTLEN][WORDSIZE], int num_words,
						 int num_responses, int group_num)
{
    // Chooses the best response from the group file. Num phrases is
    // the number of phrases in the group file to choose from, and
    // loads the phrase type of phrase_flag into output.
    // Returns the number of words in the chosen phrase.
    
    int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, i = 0;
    int purpose_type = 0;
    int num_org_object1_words = 0;
    int num_org_object2_words = 0;
    int num_robject1_words = 0;
    int num_robject2_words = 0;
    int num_oiobject1_words = 0;
    int num_oiobject2_words = 0;
    int method = 0, keeper = FALSE, found = 0;
    char response_item[SENTLEN][WORDSIZE];
    char rphrase[SENTLEN][WORDSIZE];
    char rpurpose[SENTLEN][WORDSIZE];
    char oiphrase[SENTLEN][WORDSIZE];
    char oipurpose[SENTLEN][WORDSIZE];
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    char oiobject1[SENTLEN][WORDSIZE];
    char oiobject2[SENTLEN][WORDSIZE];
    char robject1[SENTLEN][WORDSIZE];
    char robject2[SENTLEN][WORDSIZE];
    FILE *group_file;
    FILE *temp_file;
    
    // Save the original objects
    for (a = 0; a < num_object1_words; a++)
        strcpy(org_object1[a], object1[a]);
    num_org_object1_words = num_object1_words;
    for (a = 0; a < num_object2_words; a++)
        strcpy(org_object2[a], object2[a]);
    num_org_object2_words = num_object2_words;
    
    if (group_num == GROUP1)
    {
        group_file = openfile("group.grp", "r");
        copyfile("group.grp","group6.grp");
    }
    else if (group_num == GROUP2)
    {
        group_file = openfile("group2.grp", "r");
        copyfile("group2.grp","group6.grp");
    }
    temp_file = openfile("group4.grp", "w");
    
    if (num_responses == 0)
        // if zero then count them
    {
        while (!feof(group_file))
        {
            parse(response_item, group_file);
            if (!feof(group_file))
                num_responses++;
        }
        fseek(group_file, 0, SEEK_SET);
    }
    
    lookup_phrase(org_input, num_words, 100, PHRASE, GROUP2); // lookup org input
    a = choose_phrase(1, PHRASE, oiphrase, GROUP2);           // Pick first matched phrase
    b = choose_phrase(1, PURPOSE, oipurpose, GROUP2);         // Get phrase purpose
    match_phrase(org_input, num_words, oiphrase, a);          // set objects
    reduce_objects();                                         // reduce the objects
    
    for (c = 0; c < num_object1_words; c++)           // Save object 1
        strcpy(oiobject1[c], object1[c]);
    num_oiobject1_words = num_object1_words;
    
    for (c = 0; c < num_object2_words; c++)           // Save object 2
        strcpy(oiobject2[c], object2[c]);
    num_oiobject2_words = num_object2_words;
    
    method = decode_purpose_type(oipurpose, b);
    
    for (i = 0; i < num_responses; i++)
    {
        
        keeper = FALSE;
        
        c = parse(response_item, group_file);                  // read first phrase
        lookup_phrase(response_item, c, 100, PHRASE, GROUP2);  // lookup a phrase
        d = choose_phrase(1, PHRASE, rphrase, GROUP2);         // Pick first matched phrase
        e = choose_phrase(1, PURPOSE, rpurpose, GROUP2);       // Get phrase purpose
        match_phrase(response_item, c, rphrase, d);            // set objects 
        reduce_objects();                                      // reduce the objects
        
        for (f = 0; f < num_object1_words; f++)           // Save object 1
            strcpy(robject1[f], object1[f]);
        num_robject1_words = num_object1_words;
        
        for (f = 0; f < num_object2_words; f++)           // Save object 2
            strcpy(robject2[f], object2[f]);
        num_robject2_words = num_object2_words;
        
        purpose_type = decode_purpose_type(rpurpose, e);
        
        switch (method)
        {
            case SEEIFBIMPLIESA:
            {
                if(
                   ( purpose_type == AIMPLIESB || purpose_type == AIMPLIESNOTB) &&
                   (exact_match(oiobject2, num_oiobject2_words, robject1, num_robject1_words) == TRUE) &&
                   (same_type(robject2, num_robject2_words, oiobject1, num_oiobject1_words ) == TRUE))
                    keeper = TRUE;
                break;
            }
            case SEEIFAIMPLIESB:
            {
                if(
                   ( purpose_type == AIMPLIESB || purpose_type == AIMPLIESNOTB) &&
                   ((exact_match(oiobject1, num_oiobject1_words, robject1, num_robject1_words) == TRUE) &&
                    (((num_oiobject2_words == 0 ) ||
                      (num_oiobject2_words > 0 && exact_match(oiobject2, num_oiobject2_words, robject2, num_robject2_words) == TRUE)) ||
                     (num_oiobject1_words == 2 && strcmp(oiobject1[0], robject1[0])==0 && strcmp(oiobject1[1], robject2[0])==0))))
                    keeper = TRUE;
                break;
            }
        }
        
        if (keeper)
        {
            for (g = 0; g < c; g++)
                fprintf(temp_file, "%s ", response_item[g]);
            fprintf(temp_file, "\n");
            found++;
        }
        
    }  
    
    fclose(temp_file);
    fclose(group_file);
    if (group_num == GROUP1)
        copyfile("group4.grp","group.grp");
    else if (group_num == GROUP2)
        copyfile("group4.grp","group2.grp");
    removefile("group4.grp");
    
    if (group_num == GROUP1)
        found = prune_group_file("group.grp", 0, ELIMDUPS, 0);
    else if (group_num == GROUP2)
        found = prune_group_file("group2.grp", 0, ELIMDUPS, 0);
    
    if (found == 0)		// If found == 0 the restore original group file
    {
        if (group_num == GROUP1)
            copyfile("group6.grp","group.grp");
        else if (group_num == GROUP2)
            copyfile("group6.grp","group2.grp");
    }
    removefile("group6.grp");
	
    // Restore original objects
    
    num_object1_words = num_org_object1_words;
    num_object2_words = num_org_object2_words;
    
    for (a = 0; a < num_object1_words; a++)
        strcpy(object1[a], org_object1[a]);
    
    for (a = 0; a < num_object2_words; a++)
        strcpy(object2[a], org_object2[a]);
    
    return (found);
    
} // End proc choose_best_response

//******************************************************************************
int decode_purpose_type(char purpose[SENTLEN][WORDSIZE], int num_words)
{
    // Decode a purpose and return the answer method return code;
    
    int method = 0;
    
    if (strcmp(purpose[0], "see") == 0 && strcmp(purpose[1], "if") == 0)
    {
        if (strcmp(purpose[2], "a") == 0)
            method = SEEIFAIMPLIESB;
        else if (strcmp(purpose[2], "b") == 0)
            method = SEEIFBIMPLIESA;
    }
    else if (strcmp(purpose[0], "a") == 0)
    {
        if (strcmp(purpose[1], "implies") == 0)
            method = AIMPLIESB;
        else if (strcmp(purpose[1], "impliesnot") == 0)
            method = AIMPLIESNOTB;
    }
    return (method);
    
} // End proc decode_purpose_type

//******************************************************************************

int same_type(char obj1[SENTLEN][WORDSIZE], int num_obj1_words,
			  char obj2[SENTLEN][WORDSIZE], int num_obj2_words)
{
    // See if obj2 is of type obj1
    
    int result = FALSE;
    int a = 0;
    char q1[SENTLEN][WORDSIZE];
    int num_q1_words = 0;
    //  char action[SENTLEN][WORDSIZE];
    //  int num_action_words = 0;
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    int num_org_object1_words = 0;
    int num_org_object2_words = 0;
    
    // If the objects are the same, then obj2 is not of type obj1
    if(exact_match(obj1, num_obj1_words, obj2, num_obj2_words == TRUE))
        return (FALSE);
    
    // Save the original objects
    for (a = 0; a < num_object1_words; a++)
        strcpy(org_object1[a], object1[a]);
    num_org_object1_words = num_object1_words;
    for (a = 0; a < num_object2_words; a++)
        strcpy(org_object2[a], object2[a]);
    num_org_object2_words = num_object2_words;
    
    // Save the original group file
    copyfile("group.grp","group3.grp");
    
    // Begin to Construct Query
    strcpy(q1[0], "is");
    num_q1_words++;
    // Add obj1
    for (a = 0; a < num_obj1_words; a++)
    {
        strcpy(q1[num_q1_words], obj1[a]); 
        num_q1_words++;
    }
    strcpy(q1[num_q1_words], "a");
    num_q1_words++;
    // Add obj1
    for (a = 0; a < num_obj2_words; a++)
    {
        strcpy(q1[num_q1_words], obj2[a]); 
        num_q1_words++;
    }
    
    //  a = lookup_phrase(q1, num_q1_words, 100, PHRASE, GROUP1);
    //  num_action_words = choose_phrase(1, ACTION, action, GROUP1);
    //  do_action(action, num_action_words, q1, num_q1_words);
    
    a = find_all_facts(q1, num_q1_words, NEW, ALL, GROUP2);
    
    // Might need to confirm that A IMPLIES B here...
    // but for now use the code below
    
    if (a > 0)
        result = TRUE;
    else
        result = FALSE;
    
    // Restore original objects
    
    num_object1_words = num_org_object1_words;
    num_object2_words = num_org_object2_words;
    
    for (a = 0; a < num_object1_words; a++)
        strcpy(object1[a], org_object1[a]);
    
    for (a = 0; a < num_object2_words; a++)
        strcpy(object2[a], org_object2[a]);
    
    // Restore original group file
    copyfile("group3.grp", "group.grp");
    removefile("group3.grp");
    
    return (result);
    
} // End proc same_type