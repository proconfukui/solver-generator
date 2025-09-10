// optimizer.hpp
#pragma once
#include "puzzle.hpp"


struct BeamNode {
    Puzzle puzzle;
    vector<Operation> ops;
    float score;
    BeamNode(const Puzzle& p, const vector<Operation>& o, float s) : puzzle(p), ops(o), score(s) {}
};

void solve1(Puzzle &puzzle);
void greedy_algorithm(Puzzle &puzzle, int max_time);
void beam_search(Puzzle& puzzle, int search_depth);
