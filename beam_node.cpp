#include "beam_node.hpp"
#include "puzzle.hpp"
#include "utils.hpp"

BeamNode::BeamNode(const Puzzle& p, const vector<Operation>& o, float s){
        puzzle = p;
        ops = o;
        score = s;
};
BeamNode::BeamNode(const BeamNode& other){
    puzzle = other.puzzle;
    ops = other.ops;
    score = other.score;
};