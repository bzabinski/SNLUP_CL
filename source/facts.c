// Facts.c

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
#include "hypothesis.h"
#include "logic.h"
#include "rules.h"

int find_fact(char fact_to_find[SENTLEN][WORDSIZE], int num_fact_words, int
              group_flag, int search_flag, int file_flag)
{
    // Searches for the string passed in in the fact files. Returns the
    // number of facts found. All found facts are placed in the group file.
    
    int num_facts_found = 0, num_words = 0, i = 0, num_best_facts_found = 0;
    int best = 0, rating = 0, num_tempfact_words = 0, b = 0;
    char temp_fact[SENTLEN][WORDSIZE];
    char fact[SENTLEN][WORDSIZE];
    FILE *group_file;
    FILE *temp_fact_file;
    FILE *perm_fact_file;
    FILE *hyp_fact_file;
    FILE *temp_group_file;
    
    if (search_flag != SKIP_TEMP)
        temp_fact_file = openfile("facts.tmp", "r");
    if (search_flag != SKIP_PERM)
    {
        build_per_fact_file(fact_to_find, num_fact_words, APPEND);
        perm_fact_file = openfile("facts.per", "r");
    }
    if (search_flag != SKIP_HYP)
        hyp_fact_file = openfile("facts.hyp", "r");
    
    if (group_flag == NEW)
    {
        if (file_flag == GROUP1)
            group_file = openfile("group.grp", "w");
        else if (file_flag == GROUP2)
            group_file = openfile("group2.grp", "w");
    }
    else if (group_flag == APPEND)
    {
        if (file_flag == GROUP1)
            group_file = openfile("group.grp", "a");
        else if (file_flag == GROUP2)
            group_file = openfile("group2.grp", "a");
        
        fseek(group_file, 0, SEEK_END);
    }
    temp_group_file = openfile("temp.grp", "w");
    
    if ( search_flag != SKIP_PERM)
        while (perm_fact_file != NULL && !feof(perm_fact_file))
        {
            num_words = parse(fact, perm_fact_file);
            if ( search_flag == MATCH_OBJECT )
                i = match_object(fact, num_words, fact_to_find, num_fact_words);
            else
                i = match_phrase(fact, num_words, fact_to_find, num_fact_words);
            if (i >= best)
                best = i;
            if (i > 0 && !feof(perm_fact_file))
            {
                fprintf(temp_group_file, "%d\n", i);
                for (i = 0; i < num_words; i++)
                    fprintf(temp_group_file, "%s ", fact[i]);
                fprintf(temp_group_file, "\n");
                num_facts_found++;
            }
            num_words = 0;
        }
    
    num_words = 0;
    
    if ( search_flag != SKIP_TEMP)
        while (temp_fact_file != NULL && !feof(temp_fact_file))
        {
            num_words = parse(fact, temp_fact_file)-1;
            if ( search_flag == MATCH_OBJECT )
                i = match_object(fact, num_words, fact_to_find, num_fact_words);
            else
                i = match_phrase(fact, num_words, fact_to_find, num_fact_words);
            if (i >= best)
                best = i;
            if (i > 0 && !feof(temp_fact_file))
            {
                fprintf(temp_group_file, "%d\n", i);
                for (i = 0; i < num_words; i++)
                    fprintf(temp_group_file, "%s ", fact[i]);
                fprintf(temp_group_file, "\n");
                num_facts_found++;
            }
            num_words = 0;
        }
    
    if ( search_flag != SKIP_HYP)
        while (hyp_fact_file != NULL && !feof(hyp_fact_file))
        {
            num_words = parse(fact, hyp_fact_file)-2;
            if ( search_flag == MATCH_OBJECT )
                i = match_object(fact, num_words, fact_to_find, num_fact_words);
            else
                i = match_phrase(fact, num_words, fact_to_find, num_fact_words);
            if (i >= best)
                best = i;
            if (i > 0 && !feof(hyp_fact_file))
            {
                fprintf(temp_group_file, "%d\n", i);
                for (i = 0; i < num_words; i++)
                    fprintf(temp_group_file, "%s ", fact[i]);
                fprintf(temp_group_file, "\n");
                num_facts_found++;
            }
            num_words = 0;
        }
    
    if (search_flag != SKIP_TEMP)
        closefile(temp_fact_file);
    if (search_flag != SKIP_PERM)
        closefile(perm_fact_file);
    if (search_flag != SKIP_HYP)
        closefile(hyp_fact_file);
    closefile(temp_group_file);
    
    // Find best match in temp group file, write to group file
    temp_group_file = openfile("temp.grp", "r");
    
    for (i = 0; i < num_facts_found; i++)
    {
        fscanf(temp_group_file, "%d\n", &rating);
        num_tempfact_words = parse(temp_fact, temp_group_file);
        if (rating == best && rating >= 50)
        {
            for (b = 0; b < num_tempfact_words; b++)
                fprintf(group_file, "%s ", temp_fact[b]);
            fprintf(group_file, "\n");
            num_best_facts_found++;
        }
    }
    
    closefile(temp_group_file);
    //  removefile("temp.grp");
    closefile(group_file);
    
    return (num_best_facts_found);
    
} // End proc find_fact

//*********************************************************************************

int look_for_relative_facts(int search_flag)
{
    // Look for facts relative to object1 and object2, returns number of
    // matches in the group file.
    
    int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, h = 0;
    int num_org_object1_words = 0, num_org_object2_words = 0;
    char temp_fact[SENTLEN][WORDSIZE];
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    char phrase[SENTLEN][WORDSIZE];
    FILE *group_file;
    
    // Save the original objects
    for (a = 0; a < num_object1_words; a++)
        strcpy(org_object1[a], object1[a]);
    num_org_object1_words = num_object1_words;
    for (a = 0; a < num_object2_words; a++)
        strcpy(org_object2[a], object2[a]);
    num_org_object2_words = num_object2_words;
    
    a = find_fact(object1, num_object1_words, NEW, search_flag, GROUP1);
    
    a += find_fact(object2, num_object2_words, APPEND, search_flag, GROUP1);
    
    b = prune_group_file("group.grp", a, ELIMNONDUPS, 2);
    
    c = prune_group_file("group.grp", b, ELIMDUPS, 0);
    
    copyfile("group.grp", "think.tmp");  // EXPERIMENTAL
    
    if (c >= 0) // EXPERIMENTAL - Always look for substitutions
    {
        // Make substitutions and look again
        // Substitute object 1 first
        //c = 0;
        a = find_fact(object1, num_object1_words, NEW, search_flag, GROUP1);
        group_file = openfile("group.grp", "r");
        for (b = 0; b < a; b++)
        {
            // For all substitutions
            g = parse(temp_fact, group_file); // Count words in temp_fact
            num_object2_words = 0; // Clear Object2
            lookup_phrase(temp_fact, g, 100, PHRASE, GROUP2); // lookup a phrase
            h = choose_phrase(1, PHRASE, phrase, GROUP2); // Pick first matched phrase
            match_phrase(temp_fact, g, phrase, h); // set objects
            if (match_phrase(org_object1, num_org_object1_words, object1, num_object1_words) == 100)
                d += find_fact(object2, num_object2_words, APPEND, search_flag, GROUP1);
            else
                d += find_fact(object1, num_object1_words, APPEND, search_flag, GROUP1);
        } // End for all substitutions
        closefile(group_file);
        d += a;
        d += find_fact(org_object2, num_org_object2_words, APPEND, search_flag, GROUP1);
        
        e = prune_group_file("group.grp", a, DELETEFIRSTN, 0);
        // Delete facts that were substitutions
        
        f = prune_group_file("group.grp", e, ELIMNONDUPS, 2);
        
        c = prune_group_file("group.grp", f, ELIMDUPS, 0);
    } // End substitute object 1
    
    // Check all facts found to make sure object2 is present
    // Very inefficient!!
    d = c;
    for (b = 0; b < c; b++)
    {
        // For all facts in group file
        d += find_fact(org_object2, num_org_object2_words,APPEND, search_flag, GROUP1);
    }
    
    e = prune_group_file("group.grp", 0, ELIMNONDUPS, c+1);
    
    // Restore original objects
    
    num_object1_words = num_org_object1_words;
    num_object2_words = num_org_object2_words;
    
    for (a = 0; a < num_object1_words; a++)
        strcpy(object1[a], org_object1[a]);
    
    for (a = 0; a < num_object2_words; a++)
        strcpy(object2[a], org_object2[a]);
    
    return (e);
    
} // End Look_for_relative_facts

//*********************************************************************************

void write_fact(char fact[SENTLEN][WORDSIZE], int num_words, int type_flag, int gen_flag)
{
    // Write a fact in the temp or perm fact files.
    
    int i = 0;
    char kb_file[FILELEN] = "/kb/";
    char kb_file2[FILELEN] = "/kb/";
    FILE *fact_file;
    FILE *fact_file2;
	
    if (write_to_kb)
    {
        // Build filenames
        strcat(kb_file, object1[num_object1_words-1]);
        strcat(kb_file, ".fct");
        strcat(kb_file2, object2[num_object2_words-1]);
        strcat(kb_file2, ".fct");
        
        printf("KB filename: %s\n", kb_file);
        printf("KB filename2: %s\n", kb_file2);
        
        // Open files
        fact_file = openfile(kb_file, "a");
        if (fact_file == NULL)
            fact_file = openfile(kb_file, "w");
        fact_file2 = openfile(kb_file2, "a");
        if (fact_file2 == NULL)
            fact_file2 = openfile(kb_file2, "w");
        type_flag = PERMANENT;
    }
    else if (type_flag == PERMANENT)
    {
        fact_file = openfile("facts.per", "a");
        housekeeping = CHECKPERFACTS;
    }
    else if (type_flag == TEMP)
    {
        fact_file = openfile("facts.tmp", "a");
        housekeeping = CHECKTMPFACTS;
    }
    fseek(fact_file, 0, SEEK_END);
    if (write_to_kb)
        fseek(fact_file2, 0, SEEK_END);
    
    for (i = 0; i < num_words; i++)
    {
        fprintf(fact_file, "%s ", fact[i]);
        if (write_to_kb)
            fprintf(fact_file2, "%s ", fact[i]);
    }
    
    if (type_flag == TEMP && gen_flag == GEN)
        fprintf(fact_file, "g ");
    else if (type_flag == TEMP && gen_flag == LEARNED)
        fprintf(fact_file, "l ");
    fprintf(fact_file, "\n");
    
    closefile(fact_file);
	
    if (write_to_kb)
        closefile(fact_file2);
	
} // End proc write_fact

//*********************************************************************************

void record_fact(char fact[SENTLEN][WORDSIZE], int num_words, int type_flag, int gen_flag)
{
    // Record a fact in the temp or perm fact files.
    
    int a = 0, b = 0, duplicate_found = 0, contradiction_found = 0;
    int num_fact2_words = 0;
    char fact2[SENTLEN][WORDSIZE];
    FILE *group_file;
    
    if (num_words < 1) // FAILSAFE
        return;
    
    if (type_flag == PERMANENT)
        write_fact(fact, num_words, PERMANENT, gen_flag);
    else if (type_flag == TEMP)
    {
        // See if duplicate
        look_for_relative_facts(ALL); // Must come before find_all_facts
        a = find_all_facts(fact, num_words, APPEND, ALL, GROUP1);
        
        group_file = openfile("group.grp", "r");
        
        if (a > 0) // Possible duplicates or contraditions found
        {
            for (b = 0; b < a; b++)   // For each fact found
            {
                num_fact2_words = parse(fact2, group_file);  // Get fact from group file
                if (exact_match(fact, num_words, fact2, num_fact2_words))
                {
                    increase_hyp_score(fact, num_words, FACT, POS); // Found a duplicate
                    duplicate_found += 1;
                }
                else if (check_if_contradiction(fact, num_words, fact2, num_fact2_words))
                {
                    increase_hyp_score(fact, num_words, FACT, NEG); // Found a contradition
                    contradiction_found += 1;
                }
            }
        }
        
        closefile(group_file);
        
        // See if contradiction??
        // if duplicate, increase_hyp_score(POS);
        // else
        if (duplicate_found == 0 &&
            (contradiction_found == 0 || gen_flag != GEN))
            write_fact(fact, num_words, TEMP, gen_flag);
    }
    
} // End proc record_fact

//*********************************************************************************

int find_all_facts(char facts_to_find[SENTLEN][WORDSIZE], int num_fact_words, int
                   group_flag, int search_flag, int file_flag)
{
    // Searches for each word of the string passed in in the fact files. Returns the
    // number of facts found. All found facts are placed in the group file.
    
    int a = 0, b = 0, c = 0, i = 0;
    char current_fact[SENTLEN][WORDSIZE];
    
    strcpy(current_fact[0], facts_to_find[0]);
    
    if (group_flag == APPEND)
    {
        if (file_flag == GROUP1)
            copyfile("group.grp","think.tmp");
        else if (file_flag == GROUP2)
            copyfile("group2.grp","think.tmp");
    }
    a = find_fact(current_fact, 1, NEW, search_flag, file_flag);
    
    for (i = 1; i < num_fact_words; i++)
    {
        strcpy(current_fact[0], facts_to_find[i]);
        a += find_fact(current_fact, 1, APPEND, search_flag, file_flag);
    }
    
    if(num_fact_words > 1)
    {
        if (file_flag == GROUP1)
            b = prune_group_file("group.grp", b, ELIMNONDUPS, num_fact_words);
        else if (file_flag == GROUP2)
            b = prune_group_file("group2.grp", b, ELIMNONDUPS, num_fact_words);
    }
    
    if (group_flag == APPEND)
    {
        if (file_flag == GROUP1)
        {
            concatfile("think.tmp", "group.grp");
            c = prune_group_file("group.grp", 0, ELIMDUPS, 0);
        }
        else if (file_flag == GROUP2)
        {
            concatfile("think.tmp", "group2.grp");
            c = prune_group_file("group2.grp", 0, ELIMDUPS, 0);
        }
        
    }
    else
        if (file_flag == GROUP1)
            c = prune_group_file("group.grp", b, ELIMDUPS, 0);
        else if (file_flag == GROUP2)
            c = prune_group_file("group2.grp", b, ELIMDUPS, 0);
    
    return (c);
    
} // End proc find_all_facts

//*********************************************************************************

int combine_facts(char filename[FILELEN], int num_facts)
{
    // Search the given fact file and substitute facts to form new facts.
    // If num_facts is zero, an extra pass will be made to count
    // the lines in the file.
    
    int inner = 0, outer = 0;
    int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, skip = 0;
    int num_org_object1_words = 0, num_org_object2_words = 0, num_new_fact_words = 0;
    int p1 = 0, p2 = 0, facts_found = 0;
    char fact_item[SENTLEN][WORDSIZE];
    char fact_item2[SENTLEN][WORDSIZE];
    char phrase[SENTLEN][WORDSIZE];
    char phrase2[SENTLEN][WORDSIZE];
    char purpose[SENTLEN][WORDSIZE];
    char purpose2[SENTLEN][WORDSIZE];
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    char new_fact[SENTLEN][WORDSIZE];
    FILE *fact_file;
    FILE *temp_file;
    
    fact_file = openfile(filename, "r");
    temp_file = openfile("group2.tmp", "w");
    
    if (num_facts == 0)
        // if zero then count them
    {
        while (!feof(fact_file))
        {
            parse(fact_item, fact_file);
            if (!feof(fact_file))
                num_facts++;
        }
        fseek(fact_file, 0, SEEK_SET);
    }
    
    // Prime the loop
    
    for (outer=1; outer < num_facts; outer++) // For all facts in the file
    {
        for (skip=0; skip < outer; skip++)				  // Skip to first fact
            a = parse(fact_item, fact_file);                // read first fact
        if(strcmp(fact_item[a-1], "g")==0 || strcmp(fact_item[a-1], "l")==0 )
            a--;
        lookup_phrase(fact_item, a, 100, PHRASE, GROUP2); // lookup a phrase
        b = choose_phrase(1, PHRASE, phrase, GROUP2);     // Pick first matched phrase
        p1 = choose_phrase(1, PURPOSE, purpose, GROUP2);  // Get phrase purpose
        match_phrase(fact_item, a, phrase, b);            // set objects
        
        for (c = 0; c < num_object1_words; c++)           // Save object 1
            strcpy(org_object1[c], object1[c]);
        num_org_object1_words = num_object1_words;
        
        for (c = 0; c < num_object2_words; c++)           // Save object 2
            strcpy(org_object2[c], object2[c]);
        num_org_object2_words = num_object2_words;
        
        for (inner=outer+1; inner <= num_facts; inner++)
        {
            d = parse(fact_item2, fact_file);                  // read second fact
            if(strcmp(fact_item2[d-1], "g")==0 || strcmp(fact_item2[d-1], "l")==0 )
                d--;
            lookup_phrase(fact_item2, d, 100, PHRASE, GROUP2); // lookup a phrase
            e = choose_phrase(1, PHRASE, phrase2, GROUP2);     // Pick first matched phrase
            p2 = choose_phrase(1, PURPOSE, purpose2, GROUP2);  // Get phrase purpose
            match_phrase(fact_item2, d, phrase2, e);           // set objects
            
            if (match_phrase(org_object2, num_org_object2_words, object1,
                             num_object1_words) == 100 && (exact_match(purpose, p1, purpose2, p2)) &&
                (strcmp(purpose[1], "IMPLIES")==0 || strcmp(purpose[1], "implies")==0))
            {
                
                // case #1    A implies B
                //            B implies C
                // new fact:  A implies C
                
                for (c = 0; c < e; c++)                // Load phrase2 into new_fact
                    strcpy(new_fact[c], phrase2[c]);
                num_new_fact_words = e; // = b
                num_new_fact_words = plug_in_objects(new_fact, e, org_object1, num_org_object1_words,
                                                     object2, num_object2_words);
                //record_fact(new_fact, num_new_fact_words, TEMP, GEN);
                // Write new fact to temp file
                for (f = 0; f < num_new_fact_words; f++)
                    fprintf(temp_file, "%s ", new_fact[f]);
                fprintf(temp_file, "\n");
                facts_found++;
            }
            if (match_phrase(org_object1, num_org_object1_words, object2,
                             num_object2_words) == 100 && (exact_match(purpose, p1, purpose2, p2)) &&
                (strcmp(purpose[1], "IMPLIES")==0 || strcmp(purpose[1], "implies")==0))
            {
                
                // case #2    A implies B
                //            C implies A
                // new fact:  C implies B
                
                for (c = 0; c < b; c++)                // Load phrase into new_fact
                    strcpy(new_fact[c], phrase[c]);
                num_new_fact_words = b;
                num_new_fact_words = plug_in_objects(new_fact, b, object1, num_object1_words,
                                                     org_object2, num_org_object2_words);
                //record_fact(new_fact, num_new_fact_words, TEMP, GEN);
                // Write new fact to temp file
                for (f = 0; f < num_new_fact_words; f++)
                    fprintf(temp_file, "%s ", new_fact[f]);
                fprintf(temp_file, "\n");
                facts_found++;
            }
            if (match_phrase(org_object2, num_org_object2_words, object1,
                             num_object1_words) == 100 &&
                (strcmp(purpose[1], "IMPLIES")==0 || strcmp(purpose[1], "implies")==0) &&
                (strcmp(purpose2[1], "IMPLIESNOT")==0 || strcmp(purpose2[1], "impliesnot")==0))
            {
                
                // case #3    A implies B
                //            B impliesnot C
                // new fact:  A impliesnot B
                
                for (c = 0; c < e; c++)                // Load phrase2 into new_fact
                    strcpy(new_fact[c], phrase2[c]);
                num_new_fact_words = e; // = b
                num_new_fact_words = plug_in_objects(new_fact, e, org_object1, num_org_object1_words,
                                                     object2, num_object2_words);
                //record_fact(new_fact, num_new_fact_words, TEMP, GEN);
                // Write new fact to temp file
                for (f = 0; f < num_new_fact_words; f++)
                    fprintf(temp_file, "%s ", new_fact[f]);
                fprintf(temp_file, "\n");
                facts_found++;
            }
            if (match_phrase(org_object1, num_org_object1_words, object2,
                             num_object2_words) == 100 &&
                (strcmp(purpose[1], "IMPLIES")==0 || strcmp(purpose[1], "implies")==0) &&
                (strcmp(purpose2[1], "IMPLIESNOT")==0 || strcmp(purpose2[1], "impliesnot")==0))
            {
                
                // case #4    A implies B
                //            C impliesnot A
                // new fact:  C impliesnot B
                
                for (c = 0; c < b; c++)                // Load phrase into new_fact
                    strcpy(new_fact[c], phrase[c]);
                num_new_fact_words = b;
                num_new_fact_words = plug_in_objects(new_fact, b, object1, num_object1_words,
                                                     org_object2, num_org_object2_words);
                //record_fact(new_fact, num_new_fact_words, TEMP, GEN);
                // Write new fact to temp file
                for (f = 0; f < num_new_fact_words; f++)
                    fprintf(temp_file, "%s ", new_fact[f]);
                fprintf(temp_file, "\n");
                facts_found++;
            }
        }
        fseek(fact_file, 0, SEEK_SET);
    }
    
    closefile(fact_file);
    
    closefile(temp_file);
    temp_file = openfile("group2.tmp", "r");
    
    if (facts_found > 0)
    {
        while(!feof(temp_file))
        {
            num_new_fact_words = parse(new_fact, temp_file);
            if (num_new_fact_words > 0)
                record_fact(new_fact, num_new_fact_words, TEMP, GEN);
            if (feof(temp_file))
                break;
        }
        num_facts += facts_found;
    }
    
    closefile(temp_file);
    // removefile("group2.tmp");
    
    prune_group_file(filename, 0, ELIMDUPS, 0);
    
    return (num_facts);
    
} // End combine_facts

//*********************************************************************************

void generate_more_facts(char sentence[SENTLEN][WORDSIZE], int num_sent_words)
{
    // Search the given fact file and substitute facts to form new facts.
    
    int a = 0;
    //int b = 0;
    //int c = 0;
    int count = 0;
    //int lastc = 0;
    //int finished = FALSE;
    char org_object1[SENTLEN][WORDSIZE];
    char org_object2[SENTLEN][WORDSIZE];
    //  char fact_item[SENTLEN][WORDSIZE];
    int num_org_object1_words = 0;
    int num_org_object2_words = 0;
    //  FILE *fact_file;
    
    // Save the original objects
    for (a = 0; a < num_object1_words; a++)
        strcpy(org_object1[a], object1[a]);
    num_org_object1_words = num_object1_words;
    for (a = 0; a < num_object2_words; a++)
        strcpy(org_object2[a], object2[a]);
    num_org_object2_words = num_object2_words;
    
    //  while (!finished)
    //  {
    expand_perm_fact_file();
    
    //	c = 0;
    
    //    c = find_fact(object1, num_object1_words, NEW, ALL, GROUP1);
    
    //    if (num_object2_words > 0 && !object2_is_not_a_true_object)
    //      c += find_fact(object2, num_object2_words, APPEND, ALL, GROUP1);
    
    //	if ( c > 0)
	if(access_file("group.grp")==0)
        copyfile("group.grp", "think.tmp");
    
    concatfile("facts.per", "think.tmp");          // Bring in perm facts
    
    prune_group_file("think.tmp", 0, ELIMDUPS, 0);
    
    count = combine_facts("think.tmp", 0);
	
    // Count facts found
    //    fact_file = openfile("think.tmp", "r");
    //    count = 0;
    //    while (!feof(fact_file))
    //	{
    //      parse(fact_item, fact_file);
    //      if (!feof(fact_file))
    //        count++;
    //	}
    
    //    closefile(fact_file);
    //	if(count==lastc)
    //	  break;
    //    lastc = count;
    //  }
    
    find_relative_rules();
    
    // Restore original objects
    
    num_object1_words = num_org_object1_words;
    num_object2_words = num_org_object2_words;
    
    for (a = 0; a < num_object1_words; a++)
        strcpy(object1[a], org_object1[a]);
    
    for (a = 0; a < num_object2_words; a++)
        strcpy(object2[a], org_object2[a]);
    
} // End generate_more_facts

//*********************************************************************************

FILE* open_subject_file(char subject[WORDSIZE], int file_flag)
{
    // Open the subject file passed in
    
    int  found = FALSE, a = 0;
    char filename[WORDSIZE+4];
    char list_item[SENTLEN][WORDSIZE];
    FILE *subject_list;
    
    if (file_flag == NEW)
        subject_list = openfile("subjects.per", "w+r");
    else
        subject_list = openfile("subjects.per", "a+r");
    
    while( !feof(subject_list))
    {
        a = parse(list_item, subject_list);
        if(a > 0)
            if (strcmp(subject, list_item[0])==0)
                found = TRUE;
    }
    fseek(subject_list, 0, SEEK_END);
    
    if(!found)
    {
        strcpy(filename, "kb/");
        //strcpy(filename, "");
        strcat(filename, subject);
        strcat(filename, ".fct");
        
        if (access_file(filename) == 0)
        {
            fprintf(subject_list, "%s\n", subject);
            closefile(subject_list);
            return (openfile(filename, "r"));
        }
        else
        {
            closefile(subject_list);
            return(NULL);
        }
    }
    else
    {
        closefile(subject_list);
        return(NULL);
    }
    
} // End open_subject_file

//*********************************************************************************

void build_per_fact_file(char input[SENTLEN][WORDSIZE], int num_input_words, int file_flag)
{
    // Build the permanent fact file for this run
    
    int i = 0, j = 0;
    int num_words = 0;
    char fact[SENTLEN][WORDSIZE];
    FILE *perm_fact_file;
    FILE *subject_file;
    
    if(file_flag==NEW)
        perm_fact_file = openfile("facts.per", "w");
    else
    {
        perm_fact_file = openfile("facts.per", "a");
        fseek(perm_fact_file,0,SEEK_END);
    }
    
    // Add personality facts to perm fact file
    if(file_flag==NEW)
    {
        subject_file = openfile("personality.fct", "r");
        while (!feof(subject_file))
        {
            num_words = parse(fact, subject_file);
            if(num_words>0)
            {
                for (j = 0; j < num_words; j++)
                    fprintf(perm_fact_file, "%s ", fact[j]);
                fprintf(perm_fact_file, "\n");
            }
        }
        closefile(subject_file);
    }
    
    for (i=0; i < num_input_words; i++)
    {
        subject_file = open_subject_file(input[i], file_flag);
        while (subject_file != NULL && !feof(subject_file))
        {
            num_words = parse(fact, subject_file);
            if(num_words>0)
            {
                for (j = 0; j < num_words; j++)
                    fprintf(perm_fact_file, "%s ", fact[j]);
                fprintf(perm_fact_file, "\n");
            }
        }
        closefile(subject_file);
    }
	
    closefile(perm_fact_file);
    prune_group_file("facts.per", 0, ELIMDUPS, 0);
    
} // End build_per_fact_file

//*********************************************************************************

void erase_perm_fact_file(void)
{
    // Erase the permanent fact file for this run
    
    if (access_file("facts.per")==0)
        removefile("facts.per");
    
} // End erase_perm_fact_file

//*********************************************************************************

void erase_temp_fact_file(void)
{
	// Erase the temporary fact file for this run
	
	if (access_file("facts.tmp")==0)
		removefile("facts.tmp");
	
} // End erase_temp_fact_file

//*********************************************************************************

void expand_perm_fact_file(void)
{
    // Expand the current permanent fact file for this run
    
    int expanded = FALSE, num_facts = 0, last = 0;
    int a = 0, b = 0, c = 0;
    char fact_item[SENTLEN][WORDSIZE];
    char find_item[SENTLEN][WORDSIZE];
    char ignore_list[10][WORDSIZE] = {"is", "a"};
    int num_ignore = 2;
    int ignore_item = FALSE;
    FILE *fact_file;
    
    while(!expanded)
    {
        copyfile("facts.per", "think.tmp");
        
        fact_file = openfile("think.tmp", "r");
        
        num_facts = 0;
        while (!feof(fact_file))
        {
            a = parse(fact_item, fact_file);
            if (a > 0)
            {
                num_facts++;
                for(b=0; b < a; b++)
                {
                    ignore_item = FALSE;
                    strcpy(find_item[0], fact_item[b]);
                    for(c=0; c < num_ignore; c++)
                        if(strcmp(find_item[0], ignore_list[c])==0)
                            ignore_item = TRUE;
                    if(!ignore_item)
                        build_per_fact_file(find_item, 1, APPEND);
                }
            }
        }
        closefile(fact_file);
        if(num_facts == last)
            expanded = TRUE;
        last = num_facts;
    }
    
} // End expand_perm_fact_file

int get_thoughts(void)
{
    int count;
    char fact_item[SENTLEN][WORDSIZE];
	FILE *fact_file;
    
    // Count facts found
    fact_file = openfile("think.tmp", "r");
    count = 0;
    while (!feof(fact_file))
	{
        parse(fact_item, fact_file);
        if (!feof(fact_file))
            count++;
	}
    
    closefile(fact_file);
    copyfile("think.tmp","group.grp");
    
	return (count);
    
} // End get_thoughts

