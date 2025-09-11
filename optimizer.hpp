// optimizer.hpp
#pragma once
#include "puzzle.hpp"

void solve1(Puzzle &puzzle);
void greedy_algorithm(Puzzle &puzzle, int max_time);
void beam_search(Puzzle& puzzle, int search_depth);
