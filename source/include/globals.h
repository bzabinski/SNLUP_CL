// Globals.h

//Beth 11/13/2014 Added to stop multiple declaration error
#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

// Global seed for random number generation
int global_seed=0;

// Global for preventing two responses to the same question
int question_answered=FALSE;

// Global for preventing reduction of object2
int object2_is_not_a_true_object=FALSE;

// Globals for talking to myself
int intercept=0;
int intercepted = FALSE;
int suppress_reply=FALSE;

// Global for suppressing banner at intro
int no_banner = FALSE;

// Global to determine if we are running a script
int mid_script=FALSE;

// Globals to support running a script
int command_number=0;
int skip_commands=0;

// Global for suppressing taking action during a script
int suppress_actions = FALSE;

// Global for thinking in spare time
int housekeeping=0;

// Global for using context-sensitive phrase files
char frsfiles[FILELEN][FILELEN];
int num_frs_files=0;


// Strings
char response[WORDSIZE][SENTLEN];
int num_response_words=0;
char object1[WORDSIZE][SENTLEN];
int num_object1_words=0;
char object2[WORDSIZE][SENTLEN];
int num_object2_words=0;

// Flags
int alldebug=0;
int maint=0;
int filelog=FALSE;
int done_looping=FALSE;     // for main while loop
int running_script=FALSE;   // exit from script instead of exit program
int VIEWGLOBALS=0;
int training_mode=FALSE;    // If TRUE then program will ask questions and
// store facts permanently
int phase=0;                // Number of the program's current phase
int recurse_num=0;          // Number of recursions in process_input
int capture_output=FALSE;   // Capture output from running a script
int rule_fired=FALSE;       // Mark hypotheses as generated if rule just fired
int output_is_open=FALSE;   // If TRUE then output file is open, If FALSE start new output file
int write_to_kb=FALSE;      // If TRUE then write fact to Knowledge Base
int stripped_s=FALSE;       // If TRUE keep match_phrase from infinite looping
int added_s=FALSE;

// Files
char datapath[PATHLEN]="./Data";

#endif
