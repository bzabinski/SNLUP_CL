// io.c

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "snlup.h"
#include "externs.h"
#include "phrases.h"
#include "actions.h"
#include "respond.h"
#include "utilities.h"
#include "io.h"
#include "objects.h"
#include "scripts.h"
#include "sentences.h"
#include "facts.h"
#include "hypothesis.h"

void interact_with_screen(char textstring[SENTSIZE])
{
    // Reads from stdin rather than a file
    
    //long int loops = 0;
    
    printf("\n? ");                             // Display prompt
    fflush(stdout);
    
    /*
     if (maint != 29 && maint != 30)
     {
     loops = 0;
     while (!kbhit() && loops < 500000)
     {
     loops++;
     if (loops >= 500000) // Wait to do freetime processing
     {
     freetime();
     loops = 0;
     }
     }
     }
     */
    //freetime();
    //gets(textstring);
    fgets(textstring, SENTSIZE, stdin);
    printf("\n");                              // Blank line after prompt
    
} // End proc interact_with_screen

//***********************************************************************************

FILE *openfile(char filename[FILELEN], char flags[3])
{
    // Open a file with flags attributes; traps I/O errors.
    
    //int error = 0, count = 0;
    FILE *file;
    char fullpath[PATHLEN+FILELEN];
    char message[WORDSIZE];
    
    if (strcmp(datapath, "NONE")!=0)
        sprintf(fullpath, "%s/%s" ,datapath, filename);
    else
        sprintf(fullpath, "./%s" ,filename);
    
	//printf("fullpath: %s\n", fullpath);
	
	file = fopen(fullpath, flags);
    
    if (filelog)
    {
        sprintf(message, "Open %s", filename);
        add_log_entry(message);
        //sprintf(message, "Handle %d", file);
        //add_log_entry(message);
    }
    
    return (file);
    
} // End proc openfile

//***********************************************************************************

int parse(char sentence[SENTLEN][WORDSIZE], FILE *input_file)
{
    //  Simple parser breaks the input up into words. A word is a
    //  string of characters or numbers ended by a blank. A punctuation
    //  mark becomes a separate word. Returns number of words.
    //  The read_from_file flag determines whether to read input
    //  from the keyboard or from a file. Sentences are ended by
    //  a carriage return.
    
    int letter = 0, ascii_code = 0, mid_word = 0, num_words = 0, num_chars = 0, tlen = 0;
    char text[SENTSIZE], word[WORDSIZE], character, blank = ' ',  *p = word;
    
    // Clear out all words
    
    for (num_words = 0; num_words < SENTLEN; num_words++)
        for (num_chars = 0; num_chars < WORDSIZE; num_chars++)
            sentence[num_words][num_chars] = blank;
    
    num_words = 0;
    mid_word = 0;
    if (input_file != NULL)
    {
        // Read from global read_file
        
        if (feof(input_file))
            return (0);
        
        if (fgets(text, SENTSIZE, input_file) != NULL)
            tlen = (int)(strlen(text));
        else
            tlen = 0;
        
    } // End read_from_file
    else
    {
        interact_with_screen(text);
        tlen = (int)(strlen(text));
    }
    
    for (letter = 0; letter <= tlen; letter++)
    {
        character = text[letter];
        ascii_code = toascii(character);
        if ((ascii_code > 64 && ascii_code < 91) || (ascii_code > 96 && ascii_code
                                                     < 123) || (ascii_code > 47 && ascii_code < 58) || (ascii_code == 60 ||
                                                                                                        ascii_code == 62) || (ascii_code == 95))
        {
            // char is letter or number -- make a word
            *p = character;
            p++;
            mid_word = 1;
        }
        else
        {
            // see if char is puncuation
          /*  if ((ascii_code > 32 && ascii_code < 48) || (ascii_code > 57 &&
                ascii_code < 65) || (ascii_code > 90 && ascii_code < 97)
                || (ascii_code > 122 && ascii_code < 127)) */
            if (ascii_code == 61 || ascii_code == 58 || ascii_code == 43 ||
                ascii_code == 45 || ascii_code == 42 || ascii_code == 47) // Save = and : for scripts, also save + - * / for math
            {
                /* save as single char */
                if (mid_word == 1)
                {
                    *p = '\0';
                    p++;
                    strcpy(sentence[num_words], word);
                    p = word;
                    num_words++;
                    word[0] = '\0';
                    mid_word = 0;
                    if (num_words > SENTLEN)
                    {
                        printf("Input is limited to %d words\n", SENTLEN);
                        return (SENTLEN);
                    }
                } // end if
                *p = character;
                p++;
                *p = '\0';
                p++;
                strcpy(sentence[num_words], word);
                p = word;
                num_words++;
                word[0] = '\0';
                mid_word = 0;
                if (num_words > SENTLEN)
                {
                    printf("Input is limited to %d words\n", SENTLEN);
                    return (SENTLEN);
                }
            } // end if
            else
            {
                // throw the char away
                if (mid_word == 1)
                {
                    *p = '\0';
                    p++;
                    strcpy(sentence[num_words], word);
                    p = word;
                    num_words++;
                    word[0] = '\0';
                    mid_word = 0;
                    if (num_words > SENTLEN)
                    {
                        printf("Input is limited to %d words\n", SENTLEN);
                        return (SENTLEN);
                    }
                } // end if
            } // end else
        } // end else
    } // end for
    
    //  if (input_file != NULL)
    //    if (feof(input_file))
    //      return (0);
    //    else
    //      return num_words;
    //  else
    return num_words;
    
} // end parse

//***********************************************************************************

void process_input(char input[SENTLEN][WORDSIZE], int num_sent_words)
{
    int i = 0, c = 0, a = 0;
    int num_action_words = 0, num_phrase_words = 0;
    int num_purpose_words = 0, num_emotion_words = 0;
    char action[SENTLEN][WORDSIZE];
    char phrase[SENTLEN][WORDSIZE];
    char purpose[SENTLEN][WORDSIZE];
    char emotion[SENTLEN][WORDSIZE];
    FILE *input_file;
    
    recurse_num++;                                // Increase when entering procedure
    question_answered = FALSE;
    if (num_sent_words == 0)
        // If no input, go get some
    {
        //printf("\n? ");                            // Display prompt
        //while (access_file("iready.txt")!=0);      // Wait for input ready file to exist
        //removefile("iready.txt");                  // Delete iready file
        input_file = openfile("input.txt", "r");   // Open input file
        num_sent_words = parse(input, input_file); // Read input from input file
        closefile(input_file);
        removefile("input.txt");                   // Remove the input file after use
            
        for (a=0; a < num_sent_words; a++)         // Display words input to program
            printf("%s ", input[a]);
            
        printf("\n\n");                            // Blank line after prompt

        // record_raw_input(input, num_sent_words);     // Record user input in raw input file
        // Scripts do not record raw input
    }
    preprocess(input, num_sent_words);             // Preprocess input
    for (i = 0; i < num_sent_words; i++)
        for (c = 0; c < WORDSIZE; c++)
            input[i][c] = tolower(input[i][c]);
    i = lookup_phrase(input, num_sent_words, 100, PHRASE, GROUP1); // lookup a phrase
    num_phrase_words = choose_phrase(1, PHRASE, phrase, GROUP1);   // Pick first matched phrase
    match_phrase(input, num_sent_words, phrase, num_phrase_words); // set objects
    num_action_words = choose_phrase(1, ACTION, action, GROUP1);   // Set action, Pick first match
    show_sentence_pattern(action, num_action_words, phrase, num_phrase_words);
    
    // Check for intercept
    if (intercept == PHRASE)
        write_intercept(phrase, num_phrase_words);
    if (intercept == PURPOSE)
    {
        num_purpose_words = choose_phrase(1, PURPOSE, purpose, GROUP1);// Set purpose, Pick first match
        write_intercept(purpose, num_purpose_words);
    }
    if (intercept == EMOTION)
    {
        num_emotion_words = choose_phrase(1, EMOTION, emotion, GROUP1);// Set emotion, Pick first match
        write_intercept(emotion, num_emotion_words);
    }
    if (intercept == USERREPLY)
        write_intercept(input, num_sent_words);
    // Action and Response intercepts are done elsewhere
    
    if (!suppress_actions)
        // Don't do these if supressing actions
    {
        if (strcmp(action[0], "record") != 0)
            // Reduce only if not recording a fact
            if (num_action_words > 1 && strcmp(action[1], "fact") != 0)
                reduce_objects();
        if(recurse_num==1 || (running_script==1 && recurse_num==2))
            build_per_fact_file(input, num_sent_words, NEW);
        else
            build_per_fact_file(input, num_sent_words, APPEND);
        do_action(action, num_action_words, input, num_sent_words);
    }
    else
        suppress_actions = FALSE;
    
    //erase_perm_fact_file();
    recurse_num--;                // Decrease when leaving procedure
    
} // End process_input

//***********************************************************************************

int renamefile(char oldfile[FILELEN], char newfile[FILELEN])
{
    // Rename oldfile to newfile in the directory defined by the datapath global
    
    int a = 0;
    char fullpathold[PATHLEN+FILELEN];
    char fullpathnew[PATHLEN+FILELEN];
    char message[WORDSIZE];
    int error = -1;
    
    if (strcmp(datapath, "NONE")!=0)
    {
        sprintf(fullpathold, "%s/%s" ,datapath, oldfile);
        sprintf(fullpathnew, "%s/%s" ,datapath, newfile);
    }
    else
    {
        sprintf(fullpathold, "./%s" ,oldfile);
        sprintf(fullpathold, "./%s" ,oldfile);
    }
    
    for (a = 0; a < 9; a++)
    {
        error=remove(fullpathnew);
        if (error == 0)
            break;
    }
    
    error = -1;
    for (a = 0; a < 9; a++)
    {
        error=rename(fullpathold, fullpathnew);
        if (error == 0)
            break;
    }
    
    if (filelog)
    {
        sprintf(message, "Rename %s to %s", oldfile, newfile);
        add_log_entry(message);
    }
    
    if(error !=0)
    {
        if (filelog)
            add_log_entry("*** Rename Error ***");
        return (1);
    }
    
    return (0);
    
} // End proc renamefile

//***********************************************************************************

int removefile(char filename[FILELEN])
{
    // Remove the file in the directory defined by the datapath global
    
    char fullpath[PATHLEN+FILELEN];
    char message[WORDSIZE];
    
    int error;
    
    if (strcmp(datapath, "NONE")!=0)
        sprintf(fullpath, "%s/%s" ,datapath, filename);
    else
        sprintf(fullpath, "./%s" ,filename);
    
    //  if (access(fullpath, 0) ==0) // If file exists
    //{
    if ((error=remove(fullpath)!=0))
	{
        //	  error = errno;
        while (error == 13) // Permission denied
        {
            error=remove(fullpath);
            //		error = errno;
        }
        
        if (error != 0 && error != 2) // File not found
        {
            if(filelog)
            {
                sprintf(message, "I/O Error: cannot remove file: %s\n", fullpath);
                add_log_entry(message);
                sprintf(message, "Error # %d\n", error);
                add_log_entry(message);
            }
            return (1);
            //	  }
        }
    }
    
    return (0);
    
} // End proc removefile

//***********************************************************************************

int preprocess(char input[SENTLEN][WORDSIZE], int num_sent_words)
{
    // Preprocess the input sentence by replacing words found in the preprocessing list file.
    // Preprocess file is two words per line: word to find followed by its replacement
    
    char word[SENTLEN][WORDSIZE];
    char output[SENTLEN][WORDSIZE];
    int i, x, found;
    int num_pp_words;
    FILE *preprocess_file;
    
    
    
    preprocess_file = openfile("preproc.txt", "r");
    
    // Make working copy of input
    for (i = 0; i < num_sent_words; i++)
        strcpy(output[i], input[i]);
    
    while (!feof(preprocess_file))
    {
        num_pp_words = parse(word, preprocess_file); // Number of words must be even!!
        
        for (i = 0; i < num_sent_words; i++)
            if (strcmp(word[0], input[i] ) == 0)
            {
                found = 0;
                for (x = 0; x < num_pp_words/2; x++)
                    if (strcmp(word[x], input[i+x] ) == 0)
                        found++;
                if ( found == num_pp_words/2 )
                {
                    for (x = 0; x < num_pp_words/2; x++)
                        strcpy(output[i+x], word[(num_pp_words/2)+x]);
                    break;
                }
            }
    }
    
    // Copy changed text back to input
    for (i = 0; i < num_sent_words; i++)
        strcpy(input[i], output[i]);
    
    closefile(preprocess_file);
    
    return (0);
    
} // End proc preprocess

//***********************************************************************************

int access_file(char filename[FILELEN])
{
    char fullpath[PATHLEN+FILELEN];
    
    if (strcmp(datapath, "NONE")!=0)
        sprintf(fullpath, "%s/%s" ,datapath, filename);
    else
        sprintf(fullpath, "./%s" ,filename);
    
    //printf("%s\n", fullpath);
	
    return (access(fullpath, F_OK));
    
} // End access_file

//***********************************************************************************

int copyfile(char oldfile[FILELEN], char newfile[FILELEN])
{
    // Copy oldfile to newfile in the directory defined by the datapath global
    
    FILE *oldfileh;
    FILE *newfileh;
    char buf[WORDSIZE+SENTLEN];
    
    oldfileh = openfile(oldfile, "r");
    newfileh = openfile(newfile, "w");
    
    while (oldfileh != NULL && !feof(oldfileh))
    {
        if (fgets(buf, sizeof(buf),oldfileh) != NULL)
            fprintf(newfileh, "%s", buf);
    }
    
    closefile(oldfileh);
    closefile(newfileh);
    
    return (0);
    
} // End proc copyfile

//***********************************************************************************

int concatfile(char oldfile[FILELEN], char newfile[FILELEN])
{
    // Concatenate oldfile onto the end newfile in the directory defined by the datapath global
    
    FILE *oldfileh;
    FILE *newfileh;
    char buf[WORDSIZE+SENTLEN];
    
    oldfileh = openfile(oldfile, "r");
    
    if (access_file(newfile)==0)
    {
        newfileh = openfile(newfile, "r+w");
        fseek(newfileh, 0, SEEK_END);
    }
    else
        newfileh = openfile(newfile, "w");
    
    while (oldfileh != NULL && !feof(oldfileh))
    {
        if (fgets(buf, sizeof(buf),oldfileh) != NULL)
            fprintf(newfileh, "%s", buf);
    }
    
    closefile(oldfileh);
    closefile(newfileh);
    
    return (0);
    
} // End proc concatfile

//***********************************************************************************

void closefile(FILE *filehandle)
{
    // Close the file but check if it is null first
    
    int a = 0;
    int error = -1;
    char message[WORDSIZE];
    
    if (filehandle == NULL)
        return;
    
    if (filelog)
    {
        //sprintf(message, "Close %d", filehandle);
        add_log_entry(message);
    }
    
    fclose(filehandle);
    
    //  error = errno;
    
    if(error !=0 && filelog)
    {
        //sprintf(message, "I/O Error: cannot close file");
        add_log_entry(message);
        sprintf(message, "Error # %d, attempts = %d\n", error, a);
        add_log_entry(message);
    }
    
} // End proc closefile

//***********************************************************************************

void add_log_entry(char entry[WORDSIZE])
{
    // Add an entry to the file log file
    
    FILE *log;
    char fullpath[PATHLEN+FILELEN];
    
    if (!filelog)
        return;
    
    if (strcmp(datapath, "NONE")!=0)
        sprintf(fullpath, "%s/filelog.txt" ,datapath);
    else
        sprintf(fullpath, "filelog.txt"); 
    
    if (access_file("filelog.txt")==0)
        log = fopen(fullpath, "a");
    else
        log = fopen(fullpath, "w");
    
    fprintf(log, "%s\n", entry);
    
    fclose(log);
    
} // End proc add_log_entry

//***********************************************************************************

void record_objects(void)
{
	// Record <object1> and <object2> in a file for safe keeping
	
	int i;
	FILE *object_file; // ha ha thats funny right there
	
	// Save the old file
	copyfile("objects.txt", "oldobjects.txt");
	
	// Clear out the objects file
	removefile("objects.txt");
	
	// Open the objects file
	object_file = openfile("objects.txt", "w");
	
	// Write <object1>
	for (i = 0; i < num_object1_words; i++)
		fprintf(object_file, "%s ", object1[i]);
	fprintf(object_file, "\n");
	
	// Write <object2>
	for (i = 0; i < num_object2_words; i++)
		fprintf(object_file, "%s ", object2[i]);
	fprintf(object_file, "\n");
	
	// Close the objects file
	closefile(object_file);
}

//***********************************************************************************

void clear_context(void)
{
	removefile("facts.hyp");
	removefile("facts.tmp");
    
	add_response_word("The");
	add_response_word("context");
	add_response_word("has");
	add_response_word("been");
	add_response_word("cleared");
	add_response_word("\n");
	reply();	
}
