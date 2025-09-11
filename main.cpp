// main.cpp
#include "puzzle.hpp"
#include "optimizer.hpp"
#include "test.hpp"
#include <chrono>

int main()
{
    Puzzle my_puzzle("problem.json");
    //greedy_algorithm(my_puzzle,300);
    //solve1(my_puzzle);

    auto start_time = std::chrono::high_resolution_clock::now();
    beam_search(my_puzzle,5,10,5,200);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "実行時間: " << duration.count() << " ミリ秒" << std::endl;

    my_puzzle.print_field();
    my_puzzle.export_answer("answer.json");

    return 0;
}