// Actions.h

void do_action(char action[WORDSIZE][SENTLEN], int num_action_words, char sentence[WORDSIZE][SENTLEN], int num_sent_words);

int find_answers(char sentence[SENTLEN][WORDSIZE], int num_sent_words);

int find_answers_one_object(char sentence[SENTLEN][WORDSIZE], int num_sent_words);

int find_facts_to_find(char sentence[SENTLEN][WORDSIZE], int num_sent_words, char facts_to_find[SENTLEN][WORDSIZE], int num_facts);

void do_math(char action[SENTLEN][WORDSIZE], int num_action_words);