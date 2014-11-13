// Utilities.h

//Beth 11/13/2014 Added to stop multiple declaration error
#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

void viewglobals(char procname[WORDSIZE]);

void exit_snlup(int ecode);

int freetime(void);

void init(void);

//void cspause(void);

int random_num(int low, int high);

void set_global_seed(void);

void cleanup(void);

#endif
