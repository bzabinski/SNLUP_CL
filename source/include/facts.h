// Facts.h

int find_fact(char fact_to_find[SENTLEN][WORDSIZE], int num_fact_words, int
              group_flag, int search_flag, int file_flag);

int look_for_relative_facts(int search_flag);

void record_fact(char fact[SENTLEN][WORDSIZE], int num_words, int type_flag, int gen_flag);

int find_all_facts(char facts_to_find[SENTLEN][WORDSIZE], int num_fact_words, int
                   group_flag, int search_flag, int file_flag);

int combine_facts(char filename[FILELEN], int num_facts);

void write_fact(char fact[SENTLEN][WORDSIZE], int num_words, int type_flag, int gen_flag);

void generate_more_facts(char sentence[SENTLEN][WORDSIZE], int num_sent_words);

FILE* open_subject_file(char subject[WORDSIZE], int file_flag);

void build_per_fact_file(char input[SENTLEN][WORDSIZE], int num_input_words, int file_flag);

void erase_perm_fact_file(void);

void erase_temp_fact_file(void);

void expand_perm_fact_file(void);

int get_thoughts(void);
