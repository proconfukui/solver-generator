// optimizer.hpp
#pragma once
#include "puzzle.hpp"

int rand_int(int a, int b);
void greedy_algorithm(Puzzle &puzzle, int max_time);
void beam_search(Puzzle &puzzle, int search_depth,size_t beam_width,int step, int max_time);
