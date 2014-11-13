// Logic.c

#include <stdio.h>
#include <string.h>
#include "snlup.h"
#include "externs.h"
#include "utilities.h"
#include "phrases.h"

int check_yesno_answer(char question[SENTLEN][WORDSIZE], int num_qwords,
                       char response[SENTLEN][WORDSIZE], int num_rwords)
{
    // Compare a yesno question to its response and see if the answer is yes or no
    
    int result = TRUE, found = 0;
    int b = 0, c = 0, d = 0, p1 = 0, p2 = 0;
    int num_org_object1_words = 0, num_org_object2_words = 0;
    int num_org_qobject1_words = 0, num_org_qobject2_words = 0;
    int num_org_robject1_words = 0, num_org_robject2_words = 0;
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    char org_qobject1[SENTLEN][WORDSIZE];
    char org_qobject2[SENTLEN][WORDSIZE];
    char qphrase[SENTLEN][WORDSIZE];
    char qpurpose[SENTLEN][WORDSIZE];
    char org_robject1[SENTLEN][WORDSIZE];
    char org_robject2[SENTLEN][WORDSIZE];
    char rphrase[SENTLEN][WORDSIZE];
    char rpurpose[SENTLEN][WORDSIZE];
    
    //printf("In Proc check_yesno_answer");
	
    // Save org objects
    
    for (c = 0; c < num_object1_words; c++)           // Save object 1
        strcpy(org_object1[c], object1[c]);
    num_org_object1_words = num_object1_words;
    
    for (c = 0; c < num_object2_words; c++)           // Save object 2
        strcpy(org_object2[c], object2[c]);
    num_org_object2_words = num_object2_words;
    
    
    // Lookup Question
    
    lookup_phrase(question, num_qwords, 100, PHRASE, GROUP2); // lookup a phrase
    b = choose_phrase(1, PHRASE, qphrase, GROUP2);            // Pick first matched phrase
    p1 = choose_phrase(1, PURPOSE, qpurpose, GROUP2);         // Get phrase purpose
    match_phrase(question, num_qwords, qphrase, b);           // set objects
    
    for (c = 0; c < num_object1_words; c++)           // Save question's object 1
        strcpy(org_qobject1[c], object1[c]);
    num_org_qobject1_words = num_object1_words;
    
    for (c = 0; c < num_object2_words; c++)           // Save question's object 2
        strcpy(org_qobject2[c], object2[c]);
    num_org_qobject2_words = num_object2_words;
    
    // Lookup Response
    
    lookup_phrase(response, num_rwords, 100, PHRASE, GROUP2); // lookup a phrase
    d = choose_phrase(1, PHRASE, rphrase, GROUP2);            // Pick first matched phrase
    p2 = choose_phrase(1, PURPOSE, rpurpose, GROUP2);         // Get phrase purpose
    match_phrase(response, num_rwords, rphrase, b);           // set objects
    
	//printf("Responses object 1\n");
    for (c = 0; c < num_object1_words; c++)           // Save response's object 1
    {
        strcpy(org_robject1[c], object1[c]);
        //printf("%s\n", object1[c]);
    }
    num_org_robject1_words = num_object1_words;
    
    for (c = 0; c < num_object2_words; c++)           // Save response's object 2
        strcpy(org_robject2[c], object2[c]);
    num_org_robject2_words = num_object2_words;
    
    found = 0;
	
	//printf("rpurpose:\n");
    
    for (c = 0; c < p2; c++)
    {
        //printf("%s\n", rpurpose[c]);
        if (strcmp(rpurpose[c], qpurpose[c+2])==0) // Skip "see if" on qpurpose
            found += 1;
    }
	
    if (found == p2)
        result = TRUE;
    else
        result = FALSE;
    
    // Restore org objects
    
    for (c = 0; c < num_org_object1_words; c++)           // Restore object 1
        strcpy(object1[c], org_object1[c]);
    num_object1_words = num_org_object1_words;
    
    for (c = 0; c < num_org_object2_words; c++)           // Restore object 2
        strcpy(object2[c], org_object2[c]);
    num_object2_words = num_org_object2_words;
    
    return result;
    
} // End proc check_yesno_answer

//*************************************************************************

int check_if_contradiction(char fact1[SENTLEN][WORDSIZE], int num_f1words,
				           char fact2[SENTLEN][WORDSIZE], int num_f2words)
{
    // Compare two facts to see if they contradict.
    
    int result = TRUE;
    int b = 0, c = 0, d = 0, p1 = 0, p2 = 0;
    int num_org_object1_words = 0, num_org_object2_words = 0;
    int num_org_f1object1_words = 0, num_org_f1object2_words = 0;
    int num_org_f2object1_words = 0, num_org_f2object2_words = 0;
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    char org_f1object1[SENTLEN][WORDSIZE];
    char org_f1object2[SENTLEN][WORDSIZE];
    char f1phrase[SENTLEN][WORDSIZE];
    char f1purpose[SENTLEN][WORDSIZE];
    char org_f2object1[SENTLEN][WORDSIZE];
    char org_f2object2[SENTLEN][WORDSIZE];
    char f2phrase[SENTLEN][WORDSIZE];
    char f2purpose[SENTLEN][WORDSIZE];
    
    // Failsafe
    
    if (num_f1words == 0 || num_f2words == 0)
        return(FALSE);
    
    // Save org objects
    
    for (c = 0; c < num_object1_words; c++)           // Save object 1
        strcpy(org_object1[c], object1[c]);
    num_org_object1_words = num_object1_words;
    
    for (c = 0; c < num_object2_words; c++)           // Save object 2
        strcpy(org_object2[c], object2[c]);
    num_org_object2_words = num_object2_words;
    
    
    // Lookup Fact1
    
    lookup_phrase(fact1, num_f1words, 100, PHRASE, GROUP2); // lookup a phrase
    b = choose_phrase(1, PHRASE, f1phrase, GROUP2);         // Pick first matched phrase
    p1 = choose_phrase(1, PURPOSE, f1purpose, GROUP2);      // Get phrase purpose
    match_phrase(fact1, num_f1words, f1phrase, b);          // set objects
    
    for (c = 0; c < num_object1_words; c++)           // Save fact1's  object 1
        strcpy(org_f1object1[c], object1[c]);
    num_org_f1object1_words = num_object1_words;
    
    for (c = 0; c < num_object2_words; c++)           // Save fact1's object 2
        strcpy(org_f1object2[c], object2[c]);
    num_org_f1object2_words = num_object2_words;
    
    // Lookup Fact2
    
    lookup_phrase(fact2, num_f2words, 100, PHRASE, GROUP2); // lookup a phrase
    d = choose_phrase(1, PHRASE, f2phrase, GROUP2);         // Pick first matched phrase
    p2 = choose_phrase(1, PURPOSE, f2purpose, GROUP2);      // Get phrase purpose
    match_phrase(fact2, num_f2words, f2phrase, d);          // set objects
    
    for (c = 0; c < num_object1_words; c++)           // Save fact2's object 1
        strcpy(org_f2object1[c], object1[c]);
    num_org_f2object1_words = num_object1_words;
    
    for (c = 0; c < num_object2_words; c++)           // Save fact2's object 2
        strcpy(org_f2object2[c], object2[c]);
    num_org_f2object2_words = num_object2_words;
    
    if ((exact_match(f1purpose, p1, f2purpose, p2)==TRUE) &&
        (exact_match(org_f1object1, num_org_f1object1_words,
                     org_f2object1, num_org_f2object1_words)==TRUE) &&
        (exact_match(org_f1object2, num_org_f1object2_words,
                     org_f2object2, num_org_f2object2_words)==TRUE))
        result = FALSE; // If purposes match and objects match then not a contradiction
    else if((exact_match(f1purpose, p1, f2purpose, p2)==FALSE) &&
            (exact_match(org_f1object1, num_org_f1object1_words,
                         org_f2object1, num_org_f2object1_words)==TRUE) &&
            (exact_match(org_f1object2, num_org_f1object2_words,
                         org_f2object2, num_org_f2object2_words)==TRUE))
        result = TRUE; // If purposes don't match and objects match then it is a contradiction
    else if ((exact_match(org_f1object1, num_org_f1object1_words,
                          org_f2object1, num_org_f2object1_words)==FALSE) ||
             (exact_match(org_f1object2, num_org_f1object2_words,
                          org_f2object2, num_org_f2object2_words)==FALSE))
        result = FALSE; // If one or the other objects don't match then not a contradiction
    
    // Restore org objects
    
    for (c = 0; c < num_org_object1_words; c++)           // Restore object 1
        strcpy(object1[c], org_object1[c]);
    num_object1_words = num_org_object1_words;
    
    for (c = 0; c < num_org_object2_words; c++)           // Restore object 2
        strcpy(object2[c], org_object2[c]);
    num_object2_words = num_org_object2_words;
    
    if (result == TRUE)
    {
        printf("Contradiction Found\n");
        
        add_response_word("Contradiction");
        add_response_word("Found!\n");
        
        reply();
    }
	
    return result;
    
} // End proc check_if_contradiction