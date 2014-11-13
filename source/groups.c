// Groups.c

#include <stdio.h>
#include <stdlib.h>
#include "snlup.h"
#include "externs.h"
#include "io.h"
#include "phrases.h"
#include "utilities.h"

int prune_group_file(char filename[FILELEN], int num_group_items, int
                     pruning_flag, int num_dups)
{
    // Prune the group file according to the pruning flag's instructions.
    // If num_group_items is zero, an extra pass will be made to count
    // the lines in the file.
    
    int num_left = 0, i = 0, a = 0, b = 0;
    int loop = 0, loop2 = 0, skip = 0, dups = 0, matched = 0;
    char group_item[SENTLEN][WORDSIZE];
    char second_item[SENTLEN][WORDSIZE];
    FILE *group_file;
    FILE *temp_file;
    
    if (access_file(filename)!=0)        // Failsafe
        return(num_group_items);
    
    group_file = openfile(filename, "r");
    temp_file = openfile("group.tmp", "w");
    
    if (num_group_items == 0)
        // if zero then count them
    {
        while (!feof(group_file))
        {
            parse(group_item, group_file);
            if (!feof(group_file))
                num_group_items++;
        }
        fseek(group_file, 0, SEEK_SET);
    }
    if (pruning_flag == ELIMNONDUPS)
    {
        for (skip = 1; skip < num_group_items + 1; skip++)
        {
            for (i = 0; i < skip; i++)
                a = parse(group_item, group_file);
            matched = 0;
            for (loop = 0; loop < num_group_items - skip; loop++)
            {
                b = parse(second_item, group_file);
                if (a > 0 && b > 0) // If not blanks
                    if (exact_match(group_item, a, second_item, b))
                    {
                        matched++;
                        if (matched == num_dups-1)
                        {
                            for (loop2 = 0; loop2 < a; loop2++)
                                fprintf(temp_file, "%s ", group_item[loop2]);
                            fprintf(temp_file, "\n");
                            num_left++;
                        }
                        
                    } // End found a match
            } // End inner loop
            fseek(group_file, 0, SEEK_SET);
        } // End outer loop
    } // End ELIMNONDUPS
    
    else if (pruning_flag == ELIMDUPS)
    {
        for (skip = 1; skip < num_group_items + 1; skip++)
        {
            dups = 0;
            for (i = 0; i < skip; i++)
                a = parse(group_item, group_file);
            for (loop = 0; loop < num_group_items - skip; loop++)
            {
                b = parse(second_item, group_file);
                if (a > 0 && b > 0) // If not blanks
                    if (exact_match(group_item, a, second_item, b))
                        dups++;
            }
            if (dups == 0)
            {
                for (loop2 = 0; loop2 < a; loop2++)
                    fprintf(temp_file, "%s ", group_item[loop2]);
                fprintf(temp_file, "\n");
                if (a > 0) // If blank file, keep num_left at zero
                    num_left++;
            } // Write it to the temp file
            fseek(group_file, 0, SEEK_SET);
        } // End outer loop
    } // End ELIMDUPS
    else if (pruning_flag == DELETEFIRSTN)
    {
        for (i = 0; i < num_group_items; i++)
            a = parse(group_item, group_file);
        
        while (a != 0)
        {
            a = parse(group_item, group_file);
            for (loop2 = 0; loop2 < a; loop2++)
                fprintf(temp_file, "%s ", group_item[loop2]);
            fprintf(temp_file, "\n");
            num_left++;
        }
    }
    closefile(group_file);
    closefile(temp_file);
    if (access_file("group.tmp")==0)
        renamefile("group.tmp", filename);
    
    return (num_left);
    
} // End prune_group_file