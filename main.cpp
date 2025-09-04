// main.cpp
#include "puzzle.hpp"
#include "optimizer.hpp"
#include "test.hpp"

int main()
{
    Puzzle my_puzzle("problem.json");
    solve3(my_puzzle,1000);
    //solve1(my_puzzle);
    my_puzzle.print_field();\
    my_puzzle.export_answer("answer.json");

    return 0;
}