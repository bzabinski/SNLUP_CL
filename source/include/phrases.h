// Phrases.h

int choose_phrase(int num_phrases, int phrase_flag, char
                  output[WORDSIZE][SENTLEN], int group_num);

int expand_new_phrases(void);

int lookup_phrase(char phrasein[WORDSIZE][SENTLEN], int num_phrasein_words, int
                  threshold, int phrase_type, int group_num);

int match_phrase(char sentence[WORDSIZE][SENTLEN], int num_sentence_words, char
                 phrase[WORDSIZE][SENTLEN], int num_phrase_words);

int match_object(char sentence[SENTLEN][WORDSIZE], int num_sentence_words, char
				 phrase[SENTLEN][WORDSIZE], int num_phrase_words);

int exact_match(char sentence[WORDSIZE][SENTLEN], int num_sentence_words, char
                phrase[WORDSIZE][SENTLEN], int num_phrase_words);

int plug_in_objects(char phrase[SENTLEN][WORDSIZE], int num_phrase_words,
					char obj1[SENTLEN][WORDSIZE], int num_obj1_words,
					char obj2[SENTLEN][WORDSIZE], int num_obj2_words);

int get_phrase(int phrase_num, int phrase_flag, char
               output[SENTLEN][WORDSIZE], int group_num);

int choose_best_response(char org_input[SENTLEN][WORDSIZE], int num_words,
						 int num_responses, int group_num);

int decode_purpose_type(char purpose[SENTLEN][WORDSIZE], int num_words);

int same_type(char obj1[SENTLEN][WORDSIZE], int num_obj1_words,
			  char obj2[SENTLEN][WORDSIZE], int num_obj2_words);