// Rules.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snlup.h"
#include "externs.h"
#include "phrases.h"
#include "groups.h"
#include "io.h"
#include "utilities.h"
#include "facts.h"

void record_rule(char rule[SENTLEN][WORDSIZE], int num_words, int type_flag)
{
    // Record a rule in the temp or perm rule files.
    
    int i = 0;
    FILE *rule_file;
    
    if (type_flag == PERMANENT)
    {
        rule_file = openfile("rules.per", "a");
        housekeeping = CHECKPERRULES;
    }
    else if (type_flag == TEMP)
    {
        rule_file = openfile("rules.tmp", "a");
        housekeeping = CHECKTMPRULES;
    }
    fseek(rule_file, 0, SEEK_END);
    for (i = 0; i < num_words; i++)
        fprintf(rule_file, "%s ", rule[i]);
    fprintf(rule_file, "\n");
    
    closefile(rule_file);
    
} // End proc record_rule

//*******************************************************************************

void test_and_fire_rule(int suppress_response)
{
    // Test the rule against known facts, and if TRUE, fire the rule
    // It is assumed that the test is in <object1> and the rule to fire is in <object2>
    // If the flag suppress_response is TRUE then the response will be suppressed as the
    // rule is fired
    
    int a = 0, i = 0;
    int old_reply = 0;
    int num_rfwords = 0;
    int found = 0;
    char rule[SENTLEN][WORDSIZE];
    char found_rule[SENTLEN][WORDSIZE];
    FILE *temp_group_file;
    
    // Save reply flag
    old_reply = suppress_reply;
    
    if (suppress_response == TRUE)
        suppress_reply = TRUE;
    
    a = find_fact(object1, num_object1_words, NEW, ALL, GROUP1);
    
    // See if any of the rules found is an exact match
    temp_group_file = openfile("group.grp", "r");
    for(i = 0; i < a; i++)
    {
        num_rfwords = parse(found_rule, temp_group_file);
        if(exact_match(object1, num_object1_words, found_rule, num_rfwords)==TRUE)
            found++;
    }
    closefile(temp_group_file);
    
    if (found > 0)
    {
        for (i = 0; i < num_object2_words; i++)
            strcpy(rule[i], object2[i]);
        rule_fired = TRUE;
        
        process_input(rule, num_object2_words);
        rule_fired = FALSE;
    }
    
    // Restore reply flag
    suppress_reply = old_reply;
    
} // End proc test_and_fire_rule

//*******************************************************************************

void find_relative_rules(void)
{
    // Find rules relative to object1 and object2 in the temp or perm rule files.
    // New facts generated will be added to the temp fact file as generated facts.
    
    int i = 0;
    int a = 0, b = 0, t = 0, s = 0;
    int num_rules_found = 0;
    int num_rwords = 0;
    int num_org_object1_words = 0;
    int num_org_object2_words = 0;
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    char rule[SENTLEN][WORDSIZE];
    char phrase[SENTLEN][WORDSIZE];
    FILE *rule_file;
    FILE *temp_group_file;
    
    // Save the original objects
    for (a = 0; a < num_object1_words; a++)
        strcpy(org_object1[a], object1[a]);
    num_org_object1_words = num_object1_words;
    for (a = 0; a < num_object2_words; a++)
        strcpy(org_object2[a], object2[a]);
    num_org_object2_words = num_object2_words;
    
    temp_group_file = openfile("group5.grp", "w");
    
    // Search hypothesis rules
    if (access_file("rules.hyp")==0)
    {
        rule_file = openfile("rules.hyp", "r");
        while (!feof(rule_file))
        {
            num_rwords = parse(rule, rule_file)-2;
            i = match_phrase(object1, num_object1_words, rule, num_rwords);
            s = match_phrase(object2, num_object2_words, rule, num_rwords);
            if (num_rwords > 0)
                t = (num_object1_words*100)/num_rwords;
            else
                t = 100;
            if ((i >= t || s >= t) && !feof(rule_file))
            {
                for (i = 0; i < num_rwords; i++)
                    fprintf(temp_group_file, "%s ", rule[i]);
                fprintf(temp_group_file, "\n");
                num_rules_found++;
            }
            num_rwords = 0;
        }
        closefile(temp_group_file);
        if(num_rules_found > 1)
        {
            a = prune_group_file("group5.grp", num_rules_found, ELIMDUPS, 0);
            num_rules_found = a;
        }
        closefile(rule_file);
    }
    
    // Search temp rules
    if (access_file("rules.tmp")==0)
    {
        rule_file = openfile("rules.tmp", "r");
        temp_group_file = openfile("group5.grp", "a");
        while (!feof(rule_file))
        {
            num_rwords = parse(rule, rule_file);
            i = match_phrase(object1, num_object1_words, rule, num_rwords);
            s = match_phrase(object2, num_object2_words, rule, num_rwords);
            if (num_rwords > 0)
                t = (num_object1_words*100)/num_rwords;
            else
                t = 100;
            if ((i >= t || s >= t) && !feof(rule_file))
            {
                for (i = 0; i < num_rwords; i++)
                    fprintf(temp_group_file, "%s ", rule[i]);
                fprintf(temp_group_file, "\n");
                num_rules_found++;
            }
            num_rwords = 0;
        }
        closefile(temp_group_file);
        if(num_rules_found > 1)
        {
            a = prune_group_file("group5.grp", num_rules_found, ELIMDUPS, 0);
            num_rules_found = a;
        }
        closefile(rule_file);
    }
    
    // Search permanent rules
    if (access_file("rules.per")==0)
    {
        rule_file = openfile("rules.per", "r");
        temp_group_file = openfile("group5.grp", "a");
        while (!feof(rule_file))
        {
            num_rwords = parse(rule, rule_file);
            i = match_phrase(object1, num_object1_words, rule, num_rwords);
            s = match_phrase(object2, num_object2_words, rule, num_rwords);
            if (num_rwords > 0)
                t = (num_object1_words*100)/num_rwords;
            else
                t = 100;
            if ((i >= t || s >= t) && !feof(rule_file))
            {
                for (i = 0; i < num_rwords; i++)
                    fprintf(temp_group_file, "%s ", rule[i]);
                fprintf(temp_group_file, "\n");
                num_rules_found++;
            }
            num_rwords = 0;
        }
        closefile(temp_group_file);
        if(num_rules_found > 1)
        {
            a = prune_group_file("group5.grp", num_rules_found, ELIMDUPS, 0);
            num_rules_found = a;
        }
        closefile(rule_file);
    }
    
    // Fire off all rules found
    if (num_rules_found > 0)
    {
        temp_group_file = openfile("group5.grp", "r");
        while (!feof(temp_group_file))
        {
            num_rwords = parse(rule, temp_group_file);            // Get a rule from group file
            lookup_phrase(rule, num_rwords, 100, PHRASE, GROUP2); // lookup the rule
            b = choose_phrase(1, PHRASE, phrase, GROUP2);         // Pick first matched phrase
            match_phrase(rule, num_rwords, phrase, b);            // set objects 
            test_and_fire_rule(TRUE);
        }
        
        closefile(temp_group_file);
        //removefile("group5.grp");
    }
    
    // Restore original objects
    
    num_object1_words = num_org_object1_words;
    num_object2_words = num_org_object2_words;
    
    for (a = 0; a < num_object1_words; a++)
        strcpy(object1[a], org_object1[a]);
    
    for (a = 0; a < num_object2_words; a++)
        strcpy(object2[a], org_object2[a]);
    
} // End proc find_relative_rules

