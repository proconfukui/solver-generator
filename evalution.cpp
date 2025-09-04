#include "evalution.hpp"
#include "puzzle.hpp"
#include <iostream>

int count_pairs(Puzzle& puzzle){
    int counter = 0;
    for (int i = 0; i < puzzle.max_number; i++) {
        int colm_dis = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2]);
        int row_dis = abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
        if ((colm_dis == 0 && row_dis == 1) || (colm_dis == 1 && row_dis == 0))
            counter++;
    }
    return counter;
}

float avg_pair_number(Puzzle& puzzle){
    return static_cast<float>(count_pairs(puzzle)) / static_cast<float>(puzzle.max_number);
}

float evaluate_square_avg_distance(Puzzle& puzzle){
    if (puzzle.max_number == 0) return 0;
    float total_dist = 0;
    for (int i = 0; i < puzzle.max_number; i++) {
        float dist = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2])+abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
        total_dist += pow(dist,2) / puzzle.field_size; 
    }
    return total_dist / puzzle.max_number;
}

float evalution_func1(Puzzle& puzzle){
    float w1 = 1;
    float w2 = 10;
    float avg_dist = w1  * evaluate_square_avg_distance(puzzle);
    float pair_num = w2 * avg_pair_number(puzzle);
    // cout << "w1:" << avg_dist << endl;
    // cout << "w2:" << pair_num << endl;
    return pair_num - avg_dist;
}