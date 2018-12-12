#ifndef TRAINER_H
#define TRAINER_H
#include<vector>
#include<string>
#include "table.h"
#include "pgm.h"

vector<pgm> train_with_restart(const table& dt, const vector<vector<int>>& model_masks, const vector<string>& model_names, int num_restarts=100);

#endif