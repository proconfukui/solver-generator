// main.cpp
#include "puzzle.hpp"
#include "optimizer.hpp"
#include "test.hpp"

int main()
{
    Puzzle my_puzzle("problem.json");
    greedy_algorithm(my_puzzle,300);
    //solve1(my_puzzle);
    my_puzzle.print_field();
    my_puzzle.export_answer("answer.json");

    return 0;
}