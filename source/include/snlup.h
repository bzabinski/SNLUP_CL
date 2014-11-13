//  Snlup.h  include file for SNLUP.C
//  Karl L. Kittel  May, 1998

//Beth 11/13/2014 Added to stop multiple declaration error
#ifndef SNLUP_H_INCLUDED
#define SNLUP_H_INCLUDED

// General Defines
#define WORDSIZE 30
#define SENTLEN  30
#define SENTSIZE WORDSIZE*SENTLEN
#define FILELEN 25
#define PATHLEN 255
#define MAXFILES 5
#define TRUE  1
#define FALSE 0
#define ON   1
#define OFF  0
#define YES 1
#define NO  0

// Phasing
#define CLEANUP 1

// Script processing
#define PHRASE    1
#define ACTION    2
#define PURPOSE   3
#define EMOTION   4
#define RESPONSE  5
#define MONITOR   6
#define USERREPLY 7

// Fact file types
#define PERMANENT 1
#define TEMP      2

// Search for Fact Flags
//#define ALL			1 --> See Housekeeping
#define   SKIP_TEMP		2
#define   SKIP_PERM		3
#define   SKIP_HYP		4
#define	  MATCH_OBJECT	5

// Generated Facts Flags
#define GEN     1
#define LEARNED 2

// Hypothesis File Types
#define FACT 1
#define RULE 2
#define POS 1
#define NEG 2

// Object Replacement
#define LAST  1
#define FIRST 2

// Logical Purpose Types
#define AIMPLIESB      1
#define AIMPLIESNOTB   2
#define SEEIFAIMPLIESB 3
#define SEEIFBIMPLIESA 4

// File processing
#define NEW    1
#define APPEND 2

// Group processing functions
#define ELIMNONDUPS  1
#define ELIMDUPS     2
#define DELETEFIRSTN 3

// Groups
#define GROUP1  1
#define GROUP2  2

// Housekeeping
#define ALL           1
#define CHECKPERFACTS 2
#define CHECKTMPFACTS 3
#define CHECKPERRULES 4
#define CHECKTMPRULES 5
#define CHECKALLTMPS  6
#define CHECKALLPERS  7
#define CHECKHYPFACTS 8
#define CHECKHYPRULES 9

#endif
