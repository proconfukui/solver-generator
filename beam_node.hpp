#include "puzzle.hpp"
#include "utils.hpp"

struct BeamNode {
    Puzzle puzzle;
    vector<Operation> ops;
    float score;
    BeamNode(const Puzzle& p, const vector<Operation>& o, float s);
    BeamNode(const BeamNode& other);
    BeamNode() = default;
};
