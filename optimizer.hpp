// optimizer.hpp
#pragma once
#include "puzzle.hpp"

void solve1(Puzzle &puzzle);
void solve3(Puzzle &puzzle, int max_time);
vector<Operation> best_operations_all(Puzzle &puzzle);
int rand_int(int a, int b);
vector<Operation> best_operations_random(Puzzle &puzzle);
