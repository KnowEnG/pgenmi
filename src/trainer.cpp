#include <assert.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <vector>


#include "helper.h"
#include "table.h"
#include "pgm.h"
#include "trainer.h";

/**
 * Train multiple PGM models with restart.
 * Return best according to ll.
 */
vector<pgm> train_with_restart(const table& dt, const vector<vector<int>>& model_masks, const vector<string>& model_names, int num_restarts) {
	vector<pgm> models;
	vector<pgm> best_models;
	int num_models = model_masks.size();

	//Check model size

	assert(num_models != 0 && num_models == model_names.size());

	vector<int> all_ones(dt.num_features, 1);
	pgm parent(dt, all_ones, "PARENT");
	parent.initialize_params();

	//initialize_params all models from parent
    for (int i = 0; i < num_models; i++) {
    	assert(model_masks[i].size() == dt.num_features);
    	models.push_back(pgm(dt, model_masks[i], model_names[i]));
    	models[i].initialize_params(dt, parent);
    	best_models.push_back(models[i]);
    }

	// Train for restarts
	for (int i = 0; i < num_restarts; i++){
		parent.initialize_params();
		for (int i = 0; i < num_models; i++) {
			models[i].initialize_params(dt, parent);
			models[i].train(dt);
			if (models[i].final.loglik > best_models[i].final.loglik) {
				best_models[i] = models[i];
			}
		}
	}
	return best_models;
}
