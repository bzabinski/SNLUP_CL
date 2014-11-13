// Scripts.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "snlup.h"
#include "externs.h"
#include "respond.h"
#include "io.h"
#include "utilities.h"
#include "actions.h"
#include "phrases.h"
#include "facts.h"

void run_script(char script_name[FILELEN], int read_file)
{
    // Turns control of the program over to a user-defined script.
    
    int i = 0, c = 0, pos = 0, found = 0, t = 0;
    int num_command_words = 0;
    int num_action_words = 0, script_id = 0;
    int num_cept_words = 0, num_if_arg_words = 0;
    int do_else = FALSE, swap_obj1 = FALSE;
    int num_temp_words = 0, found_not = FALSE, num_var_words = 0;
    char command[SENTLEN][WORDSIZE];
    char action[SENTLEN][WORDSIZE];
    char cept[SENTLEN][WORDSIZE];
    char label[SENTLEN][WORDSIZE];
    char if_arg[SENTLEN][WORDSIZE];
    char temp[SENTLEN][WORDSIZE];
    char labelfile_name[FILELEN];
    char varfile_name[FILELEN];
    char tmpvars_name[FILELEN];
    FILE *script_file;
    FILE *label_file;
    FILE *intercept_file;
    FILE *var_file;
    FILE *tmpvars;
    FILE *tmpfacts;
    FILE *currentscript;
	
	// Prepare file names
    
	sprintf(labelfile_name, "%s", script_name);
	sprintf(varfile_name, "%s", script_name);
	sprintf(tmpvars_name, "%s", script_name);
    
	if (read_file)
        strcat(script_name, ".txt");
	else
	{
        strcat(script_name, ".scr");
        // Record current running script (for export)
        removefile("curscript.txt");
        currentscript = openfile("curscript.txt", "w");
        fprintf(currentscript, "%s\n", script_name);
        closefile(currentscript);
    }
    strcat(labelfile_name, ".lbl");
    strcat(varfile_name, ".var");
    strcat(tmpvars_name, ".tmp");
    
	script_id = running_script;
	script_file = openfile(script_name, "r");
	
	if (script_file == NULL)
	{
		add_response_word("I");
		add_response_word("can");
		add_response_word("not");
		add_response_word("find");
		add_response_word("that");
		add_response_word("file");
		add_response_word("\n");
		reply();
		
		return;
	}
	
    label_file = openfile(labelfile_name, "w");
    var_file = openfile(varfile_name, "w");
    
    num_command_words = parse(command, script_file); // Prime the loop
    while (!feof(script_file))
        // Find and record all labels and vars
    {
        for (i = 0; i < num_command_words; i++)
        {
            for (c = 0; c < WORDSIZE; c++)
                command[i][c] = tolower(command[i][c]);
        }
        if (strcmp(command[0], ":") == 0)
        {
            fprintf(label_file, "%s\n", command[1]);
            fprintf(label_file, "%lu\n", ftell(script_file));
        }
        else if (strcmp(command[1], "=") == 0)
            fprintf(var_file, "%s %s\n", command[0], command[2]);
        // user-defined variable, then what to sub for
		num_command_words = parse(command, script_file);
        // Keep at end of loop so we don't read last line twice
    }
    closefile(label_file);
    closefile(var_file);
    fseek(script_file, 0, SEEK_SET);
	
	if (mid_script) // Skip commands already done
	{
        printf("Skip Commands:%d\n", skip_commands);
        for (i=0; i<skip_commands; i++)
            num_command_words = parse(command, script_file);
	}
	
	
	while (!feof(script_file))
    {
        //printf("Top of While Loop\n");
        skip_commands++;
        if (read_file)
            suppress_reply = TRUE;
        swap_obj1 = FALSE;
        num_command_words = parse(command, script_file);
        for (i = 0; i < num_command_words; i++)
        {
            // Find all vars and replace -- LIMITED TO REPLACING LAST VAR ONLY!!
            //printf("Command: %s\n", command[i]);
            for (c = 0; c < WORDSIZE; c++)
                command[i][c] = tolower(command[i][c]);
            if (num_command_words >= 2 && strcmp(command[1], "=") != 0)
                // Don't replace vars in assignments
            {
                var_file = openfile(varfile_name, "r");
                tmpvars = openfile(tmpvars_name, "w");
                num_var_words = parse(label, var_file);
                while (!feof(var_file))
                {
                    if (strcmp(command[i], label[0]) == 0)
                    {
                        // Swapping an object
                        //                  if(strcmp(label[1],"<object1>")==0)
                        //                  {
                        // Splice in object1
                        //                     if(debug) printf("Splicing in object1\n");
                        //                     for(c=0;c<i;c++)
                        //                     {
                        //                        if(debug) printf("C=%d\n", c);
                        //                        strcpy(temp[c],command[c]);
                        //                     }
                        //                     if(debug) printf("Splicing in object1 #2\n");
                        //                     for(c=0;c<num_object1_words;c++)
                        //                     {
                        //                        if(debug) printf("C=%d\n", c);
                        //                        strcpy(temp[i+c],object1[c]);
                        //                     }
                        //                     if(debug) printf("Splicing in object1 #3\n");
                        //                     for(c=i+1;c<=num_command_words;c++)
                        //                     {
                        //                        if(debug) printf("C=%d\n", c);
                        //                        strcpy(temp[c+num_object1_words-1],command[c]);
                        //                     }
                        //                     num_temp_words=num_command_words+num_object1_words-1;
                        //                     swap_obj1=TRUE;
                        //                  }
                        //                  else
                        //                  {
                        // Just swapping from var file
                        // Simple replacement ?
                        if (num_var_words == 2)
                            strcpy(command[i], label[1]);
                        // Subsitute the var
                        else
                        {
                            // Splice in more than one word
                            for (c = 0; c < i; c++)
                                strcpy(temp[c], command[c]);
                            for (c = 1; c < num_var_words; c++)
                                strcpy(temp[i + c - 1], label[c]);
                            for (c = i + 1; c <= num_command_words; c++)
                                strcpy(temp[c + num_var_words - 2], command[c]);
                            num_temp_words = num_command_words + num_var_words - 2;
                            swap_obj1 = TRUE;
                        } // End else splice in more than one word
                        //                  } // End else just swapping from a file
                    } // End if swapping an object
                    
                    // Save substitutions in temp var file
                    // so that a   'var = <object1>'  has the
                    // value of <object1> saved from the first assignment.
                    
                    //               if(swap_obj1)
                    //               {
                    //                  if(maint==31)
                    //                     printf("Swapping old object1\n");
                    //                  fprintf(tmpvars, "%s ", label[0]);
                    //                  for(t=0;t<num_object1_words-1;t++)
                    //                     fprintf(tmpvars, "%s ", object1[t]);
                    //                  fprintf(tmpvars, "%s\n", temp[num_object1_words-1]);
                    //               }
                    //               else
                    //               {
                    //                  if(maint==31)
                    //                     printf("Not swapping old object1\n");
                    //                  for(t=0;t<num_var_words-1;t++)
                    //                     fprintf(tmpvars, "%s ", label[t]);
                    //                  fprintf(tmpvars, "%s\n", label[num_var_words-1]);
                    //               }
                    
                    num_var_words = parse(label, var_file);
                } // End while not feof var_file
                closefile(var_file);
                closefile(tmpvars);
                
                // move file here
                //            sprintf(syscall, "move %s %s >junk.tmp", tmpvars_name, varfile_name);
                //            system(syscall);
                //            if(maint==31)
                //            {
                //               printf("Press esc then look at %s\n", varfile_name);
                //               CSpause();
                //            }
                
            } // End if not an assignment
            else
            {
                
                // This IS an assignment. Loop though var file and replace
                // all <object1>'s for this assignment.
                
                if (strcmp(label[2], "<object1>") != 0)
                    // Only <object1> assignments
                {
                    var_file = openfile(varfile_name, "r");
                    tmpvars = openfile(tmpvars_name, "w");
                    num_var_words = parse(label, var_file);
                    while (!feof(var_file))
                    {
                        if (strcmp(command[i], label[0]) == 0)
                        {
                            // Its a match, swap it
                            fprintf(tmpvars, "%s ", label[0]);
                            for (t = 0; t < num_object1_words - 1; t++)
                                fprintf(tmpvars, "%s ", object1[t]);
                            fprintf(tmpvars, "%s\n", object1[num_object1_words - 1]);
                        } // End Its a match, swap it
                        else
                        {
                            // Just copy it as is to temp var file
                            if (maint == 31)
                                printf("Not swapping old object1\n");
                            for (t = 0; t < num_var_words - 1; t++)
                                fprintf(tmpvars, "%s ", label[t]);
                            fprintf(tmpvars, "%s\n", label[num_var_words - 1]);
                        } // End just copy it as is to temp var file
                        num_var_words = parse(label, var_file);
                    } // End while
                    closefile(var_file);
                    closefile(tmpvars);
                    renamefile(tmpvars_name, varfile_name);
                } // End if <object1>
            } // End this IS an assignment
            
        } // End find all vars and replace
        
        if (swap_obj1)
        {
            for (c = 0; c < num_temp_words; c++)
                strcpy(command[c], temp[c]);
            num_command_words = num_temp_words;
        }
        
        if (strcmp(command[0], "respond") == 0)
        {
            for (i = 1; i < num_command_words; i++)
                strcpy(action[i - 1], command[i]);
            num_action_words = num_command_words - 1;
            //build_per_fact_file(action, num_action_words, NEW);
            do_action(action, num_action_words, action, num_action_words);
        } // End if command = respond
        else if (strcmp(command[0], "intercept") == 0)
        {
            if (strcmp(command[1], "responses") == 0)
                intercept = RESPONSE;
            else if (strcmp(command[1], "phrases") == 0)
                intercept = PHRASE;
            else if (strcmp(command[1], "purposes") == 0)
                intercept = PURPOSE;
            else if (strcmp(command[1], "actions") == 0)
                intercept = ACTION;
            else if (strcmp(command[1], "emotions") == 0)
                intercept = EMOTION;
            else if (strcmp(command[1], "user") == 0)
            {
                if (strcmp(command[2], "reply") == 0)
                    intercept = USERREPLY;
            }
            if (strcmp(command[2], "suppress") == 0)
                if (strcmp(command[3], "reply") == 0)
                    suppress_reply = TRUE;
        }
        else if (strcmp(command[0], "monitor") == 0)
        {
            if (strcmp(command[1], "responses") == 0)
                intercept = MONITOR;
        }
        else if (strcmp(command[0], "end") == 0)
        {
            if (strcmp(command[1], "intercept") == 0)
                intercept = FALSE;
        }
        else if (strcmp(command[0], "if") == 0)
        {
            if ((strcmp(command[1], "!") == 0) || (strcmp(command[1], "not") == 0))
                found_not = TRUE;
            if (found_not)
                for (i = 2; i < num_command_words; i++)
                    strcpy(if_arg[i - 2], command[i]);
            else
                for (i = 1; i < num_command_words; i++)
                    strcpy(if_arg[i - 1], command[i]);
            num_if_arg_words = num_command_words - 1;
            intercept_file = openfile("intercpt.tmp", "r");
            num_cept_words = parse(cept, intercept_file);
            closefile(intercept_file);
            intercepted = FALSE;
            
            if (found_not)
            {
                if (exact_match(cept, num_cept_words, if_arg, num_if_arg_words))
                {
                    do_else = TRUE;
                    num_command_words = parse(command, script_file); // Skip next command
                }
                else
                    ;
                // Do nothing; process next command in script normally
            }
            else
            {
                if (exact_match(cept, num_cept_words, if_arg, num_if_arg_words))
                    ;
                // Do nothing; process next command in script normally
                else
                {
                    do_else = TRUE;
                    num_command_words = parse(command, script_file); // Skip next command
                }
            }
            found_not = FALSE;
        }
        else if (strcmp(command[0], "else") == 0)
        {
            if (do_else)
                do_else = FALSE;
            // Reset flag and process next command normally
            else
                num_command_words = parse(command, script_file);
            // Skip next command
        }
        else if (strcmp(command[0], "goto") == 0)
        {
            openfile(labelfile_name, "r");
            found = 0;
            while (!feof(label_file))
            {
                parse(label, label_file);
                fscanf(label_file, "%d\n", &pos);
                if (strcmp(label[0], command[1]) == 0)
                {
                    fseek(script_file, pos, SEEK_SET);
                    found++;
                }
            } // End while not feof label file
            if (found != 1)
            {
                printf("\nERROR: Label not found or duplicate label\n");
                exit_snlup(0);
            }
            closefile(label_file);
        } // End goto
        else if (strcmp(command[0], "echo") == 0)
        {
            num_response_words = 0; // Clear word counts first
            for (i = 1; i < num_command_words; i++)
                add_response_word(command[i]);
            add_response_word("\n");
            reply();
            //suppress_actions = TRUE;
        }
        else if (strcmp(command[0], "rem") == 0)
            ;
        // Comment -- Skip this line
        else if (strcmp(command[1], "=") == 0)
            ;
        // Var assignment -- Skip this line
        else if (strcmp(command[0], ":") == 0)
            ;
        // Label -- Skip this line
        else if (strcmp(command[0], "wait") == 0)
        {
            if (strcmp(command[1], "for") == 0)
                if (strcmp(command[2], "input") == 0)
                {
                    mid_script = TRUE;
                    if (num_command_words > 3)
                        if (strcmp(command[3], "suppress") == 0)
                            if (strcmp(command[4], "actions") == 0)
                                suppress_actions = TRUE;
                    break;
                }
        }
        else if (strcmp(command[0], "import") == 0)
        {
            if (strcmp(command[1], "phrase") == 0)
                if (strcmp(command[2], "file") == 0)
                {
                    num_frs_files++;
                    strcat(command[3], ".frs");
                    sprintf(frsfiles[num_frs_files - 1], "%s", command[3]);
                }
        }
        else if (strcmp(command[0], "close") == 0)
        {
            if (strcmp(command[1], "phrase") == 0)
                if (strcmp(command[2], "file") == 0)
                    num_frs_files--;
        }
        else if (strcmp(command[0], "start") == 0)
        {
            if (strcmp(command[1], "new") == 0)
                if (strcmp(command[2], "temp") == 0)
                    if (strcmp(command[3], "facts") == 0)
                        if (strcmp(command[4], "file") == 0)
                        {
                            removefile("facts.tmp");
                            tmpfacts = openfile("facts.tmp", "w");
                            fprintf(tmpfacts, "today is wednesday g\n"); // Should be an init file or saved tmp fact file
                            // Put in code to add real day
                            closefile(tmpfacts);
                        }
        }
        else if (strcmp(command[0], "capture") == 0)
        {
            if (strcmp(command[1], "output") == 0)
                capture_output = TRUE;
            if (strcmp(command[1], "off") == 0)
                capture_output = FALSE;
        } 
        else
            // Always last choice
        {
            if (!feof(script_file))
                process_input(command, num_command_words);
        }
        
    } // End while !feof
    
    if (feof(script_file))
        mid_script = FALSE;
	
    closefile(script_file);
    
	if (!mid_script)
	{
        removefile(labelfile_name);
        removefile(varfile_name);
        removefile(tmpvars_name);
        
        intercept = FALSE; // Clear intercept flag
        if (read_file)
            suppress_reply = FALSE;
        // Clear read file flag
        running_script=0;
        skip_commands=0;
    }
    
	
} // End run_script

//*******************************************************************************

void write_intercept(char inp[SENTLEN][WORDSIZE], int num_inp_words)
{
    // Write the intercepted item to the intercept file
    
    int i = 0;
    FILE *intercept_file;
    
    intercept_file = openfile("intercpt.tmp", "w");
    for (i = 0; i < num_inp_words; i++)
        fprintf(intercept_file, "%s ", inp[i]);
    fprintf(intercept_file, "\n");
    intercepted = TRUE;
    closefile(intercept_file);
    
} // End write intercept