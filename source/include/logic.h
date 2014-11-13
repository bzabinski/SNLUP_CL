// Logic.h

int check_yesno_answer(char question[SENTLEN][WORDSIZE], int num_qwords,
				       char response[SENTLEN][WORDSIZE], int num_rwords);

int check_if_contradiction(char fact1[SENTLEN][WORDSIZE], int num_f1words,
				           char fact2[SENTLEN][WORDSIZE], int num_f2words);