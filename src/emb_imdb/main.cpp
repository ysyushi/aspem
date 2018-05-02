#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <gsl/gsl_rng.h>
#include "linelib.h"

char attribute_file[MAX_STRING], center_file[MAX_STRING], hin_file[MAX_STRING], output_file[MAX_STRING], output_center_file[MAX_STRING], attribute_types[MAX_STRING];
int binary = 0, num_threads = 1, vector_size = 100, negative = 5;
long long samples = 1, edge_count_actual;
real alpha = 0.025, starting_alpha;

const gsl_rng_type * gsl_T;
gsl_rng * gsl_r;

line_node attribute, center;
line_hin star_hin;
line_trainer trainer_um, trainer_am, trainer_dm, trainer_gm;

double func_rand_num()
{
	return gsl_rng_uniform(gsl_r);
}

void *TrainModelThread(void *id) 
{
	long long edge_count = 0, last_edge_count = 0;
	unsigned long long next_random = (long long)id;
	real *error_vec = (real *)calloc(vector_size, sizeof(real));

	while (1)
	{
		if (edge_count > samples / num_threads + 2) break;

		if (edge_count - last_edge_count>10000)
		{
			edge_count_actual += edge_count - last_edge_count;
			last_edge_count = edge_count;
			printf("%cAlpha: %f Progress: %.3lf%%", 13, alpha, (real)edge_count_actual / (real)(samples + 1) * 100);
			fflush(stdout);
			alpha = starting_alpha * (1 - edge_count_actual / (real)(samples + 1));
			if (alpha < starting_alpha * 0.0001) alpha = starting_alpha * 0.0001;
		}

		if(strstr(attribute_types, "u")) trainer_um.train_sample(alpha, error_vec, func_rand_num, next_random);
		if(strstr(attribute_types, "a")) trainer_am.train_sample(alpha, error_vec, func_rand_num, next_random);
		if(strstr(attribute_types, "d")) trainer_dm.train_sample(alpha, error_vec, func_rand_num, next_random);
		if(strstr(attribute_types, "g")) trainer_gm.train_sample(alpha, error_vec, func_rand_num, next_random);

		edge_count += 3;
	}
	free(error_vec);
	pthread_exit(NULL);
}

void TrainModel() {
	long a;
	pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
	starting_alpha = alpha;

	attribute.init(attribute_file, vector_size);
	center.init(center_file, vector_size);
	star_hin.init(hin_file, &attribute, &center);

	printf("Learning embedding considering attribute node types: %s\n", attribute_types);

	if(strstr(attribute_types, "u")) trainer_um.init('u', &star_hin, negative);
	if(strstr(attribute_types, "a")) trainer_am.init('a', &star_hin, negative);
	if(strstr(attribute_types, "d")) trainer_dm.init('d', &star_hin, negative);
	if(strstr(attribute_types, "g")) trainer_gm.init('g', &star_hin, negative);

	clock_t start = clock();
	printf("Training process:\n");
	for (a = 0; a < num_threads; a++) pthread_create(&pt[a], NULL, TrainModelThread, (void *)a);
	for (a = 0; a < num_threads; a++) pthread_join(pt[a], NULL);
	printf("\n");
	clock_t finish = clock();
	printf("Total time: %lf\n", (double)(finish - start) / CLOCKS_PER_SEC);

	attribute.output(output_file, binary);
	if (0 != strcmp(output_center_file, "None"))
	{
		center.output(output_center_file, binary);
	}	
}

int ArgPos(char *str, int argc, char **argv) {
	int a;
	for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
		if (a == argc - 1) {
			printf("Argument missing for %s\n", str);
			exit(1);
		}
		return a;
	}
	return -1;
}

int main(int argc, char **argv) {
	int i;
	if (argc == 1) {
		printf("Starline: Embedding HIN with Star Schema\n\n");
		printf("Options:\n");
		printf("Parameters for training:\n");
		printf("\t-attribute <file>\n");
		printf("\t\tThe attribute node set\n");
		printf("\t-center <file>\n");
		printf("\t\tThe center node set\n");
		printf("\t-hin <file>\n");
		printf("\t\tThe file of the text HIN\n");
		printf("\t-output <file>\n");
		printf("\t\tSave attribute node embedding into <file>\n");
		printf("\t-output-center <file>\n");
		printf("\t\tSave center node embedding into <file>\n");
		printf("\t-binary <int>\n");
		printf("\t\tSave the resulting vectors in binary moded; default is 0 (off)\n");
		printf("\t-size <int>\n");
		printf("\t\tSet size of center node vectors; default is 100\n");
		printf("\t-negative <int>\n");
		printf("\t\tNumber of negative examples; default is 5, common values are 5 - 10 (0 = not used)\n");
		printf("\t-samples <int>\n");
		printf("\t\tSet the number of training samples as <int>Million\n");
		printf("\t-threads <int>\n");
		printf("\t\tUse <int> threads (default 1)\n");
		printf("\t-alpha <float>\n");
		printf("\t\tSet the starting learning rate; default is 0.025\n");
		printf("\t-types <float>\n");
		printf("\t\tTypes of attribute nodes to be considered in the embedding\n");
		printf("\nExamples:\n");
		printf("./emb_imdb -attribute attribute.txt -center center.txt -hin hin.txt -output attribute.emb -output-center center.emb -types uadg -size 20 -negative 5 -samples 100\n\n");
		return 0;
	}
	output_file[0] = 0;
	strcpy (output_center_file, "None");
	if ((i = ArgPos((char *)"-attribute", argc, argv)) > 0) strcpy(attribute_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-center", argc, argv)) > 0) strcpy(center_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-hin", argc, argv)) > 0) strcpy(hin_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-output", argc, argv)) > 0) strcpy(output_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-output-center", argc, argv)) > 0) strcpy(output_center_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-binary", argc, argv)) > 0) binary = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-size", argc, argv)) > 0) vector_size = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-negative", argc, argv)) > 0) negative = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-samples", argc, argv)) > 0) samples = atoi(argv[i + 1])*(long long)(1000000);
	if ((i = ArgPos((char *)"-alpha", argc, argv)) > 0) alpha = atof(argv[i + 1]);
	if ((i = ArgPos((char *)"-threads", argc, argv)) > 0) num_threads = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-types", argc, argv)) > 0) strcpy(attribute_types, argv[i + 1]);

	gsl_rng_env_setup();
	gsl_T = gsl_rng_rand48;
	gsl_r = gsl_rng_alloc(gsl_T);
	gsl_rng_set(gsl_r, 314159265);

	TrainModel();
	return 0;
}