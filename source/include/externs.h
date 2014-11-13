// Externs.h

// Global seed for random number generation
extern int global_seed;

// Global for preventing two responses to the same question
extern int question_answered;

// Global for preventing reduction of object2
extern int object2_is_not_a_true_object;

// Globals for talking to myself
extern int intercept;
extern int intercepted;
extern int suppress_reply;

// Global for suppressing banner at intro
extern int no_banner;

// Global for suppressing taking action during a script
extern int suppress_actions;

// Global to determine if we are running a script
extern int mid_script;

// Globals to support running a script
extern int command_number;
extern int skip_commands;

// Global for thinking in spare time
extern int housekeeping;

// Global for using context-sensitive phrase files
extern char frsfiles[FILELEN][FILELEN];
extern int num_frs_files;


// Strings
extern char response[WORDSIZE][SENTLEN];
extern int num_response_words;
extern char object1[WORDSIZE][SENTLEN];
extern int num_object1_words;
extern char object2[WORDSIZE][SENTLEN];
extern int num_object2_words;

// Flags
extern int alldebug;
extern int maint;
extern int filelog;
extern int done_looping;     // for main while loop
extern int running_script;   // exit from script instead of exit program
extern int VIEWGLOBALS;
extern int training_mode;    // If TRUE then program will ask questions and
// store facts permanently
extern int phase;            // Number of the program's current phase
extern int recurse_num;      // Number of recursions in process_input
extern int capture_output;   // Capture output from a script
extern int rule_fired;       // Mark hypotheses as generated if rule just fired
extern int output_is_open;   // If TRUE then output file is open, If FALSE start new output file
extern int write_to_kb;		 // If TRUE then write fact to Knowledge Base
extern int stripped_s;       // If TRUE prevent match_phrase from infinite looping
extern int added_s;

// Files
extern char datapath[PATHLEN];

