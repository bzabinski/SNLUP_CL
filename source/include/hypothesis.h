// Hypothesis.h

void store_hypothesis(char hyp[SENTLEN][WORDSIZE], int num_words, int type_flag, int search_flag);

void record_hypothesis(char hyp[SENTLEN][WORDSIZE], int num_words, int type_flag);

void increase_hyp_score(char hyp[SENTLEN][WORDSIZE], int num_words, int type_flag, int pos_or_neg);

void collect_hypotheses(void);

void erase_hypothesis(char text[SENTSIZE]);