#ifndef TRAINER_H
#define TRAINER_H
#include <string>
#include <vector>
#include "pgm.h"
#include "table.h"

vector<pgm> train_with_restart(const table& dt,
                               const vector<vector<int>>& model_masks,
                               const vector<string>& model_names,
                               int num_restarts = 100);

#endif