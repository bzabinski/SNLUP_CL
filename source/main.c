//
//  main.c
//  Robot Brain
//
//  Created by Karl Kittel on 6/3/14.
//  Copyright (c) 2014 Karl Kittel. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "snlup.h"
#include "globals.h"
#include "utilities.h"
#include "io.h"


int main(int argc, char *argv[])
{
    // Simplified Natural Language Understanding Program main procedure.
    
    int words = 0;
    char user_input[SENTLEN][WORDSIZE];
    
    if (argc == 2)
        // Don't forget to count the program name as an arg!
    {
        maint = atoi(argv[1]); // Turn on maintenance functions
        printf("\nMAINTENANCE OPTION # %d ACTIVATED\n", maint);
    }
    
    init();
    
    while (!done_looping)
    {
        words = 0;
        fflush(stdout);
        process_input(user_input, words);
        
    } // End while true
    
    cleanup();
	
	return(0);
    
} // end main
