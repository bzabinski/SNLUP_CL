// Rules.h

void record_rule(char rule[SENTLEN][WORDSIZE], int num_words, int type_flag);

void test_and_fire_rule(int suppress_response);

void find_relative_rules(void);