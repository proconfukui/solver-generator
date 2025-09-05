#include "evalution.hpp"
#include "puzzle.hpp"
#include <iostream>

int count_pairs(Puzzle &puzzle)
{
    int counter = 0;
    for (int i = 0; i < puzzle.max_number; i++)
    {
        int colm_dis = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2]);
        int row_dis = abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
        if ((colm_dis == 0 && row_dis == 1) || (colm_dis == 1 && row_dis == 0))
            counter++;
    }
    return counter;
}

float avg_pair_number(Puzzle &puzzle)
{
    return static_cast<float>(count_pairs(puzzle)) / static_cast<float>(puzzle.max_number);
}

float evaluate_square_avg_distance(Puzzle &puzzle)
{
    if (puzzle.max_number == 0)
        return 0;
    float total_dist = 0;
    for (int i = 0; i < puzzle.max_number; i++)
    {
        float dist = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2]) + abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
        total_dist += pow(dist, 2) / puzzle.field_size;
    }
    return total_dist / puzzle.max_number;
}

float evalution_func1(Puzzle &puzzle)
{
    float w1 = 1;
    float w2 = 10;
    float avg_dist = w1 * evaluate_square_avg_distance(puzzle);
    float pair_num = w2 * avg_pair_number(puzzle);
    // cout << "w1:" << avg_dist << endl;
    // cout << "w2:" << pair_num << endl;
    return pair_num - avg_dist;
}

// 1.1 隅のペアを評価する
// ペア数が多いほど、ペアがフィールドの隅に集中しているほど値が大きくなる
int evaluatie_edge_pairs(Puzzle & 6puzzle)
{
    int value = 0;
    for (int i = 0; i < puzzle.max_number; i++)
    {
        int colm_dis = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2]);
        int row_dis = abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
        if (colm_dis + row_dis == 1)
        {
            value += 100;
            for (int j = 0; j < 4; j++)
            {
                value -= min(puzzle.pair_coordinates[i][j], puzzle.field_size - 1 - puzzle.pair_coordinates[i][j]);
            }
        }
    }
    return value;
}

// 1.2 ペア候補の重心の分散を評価する。値は大きければ大きいほど良い
int center_variance(Puzzle &puzzle)
{
    int sum_x = 0, sum_x_squared = 0, sum_y = 0, sum_y_squared = 0;
    for (int number = 0; number < puzzle.max_number; number++)
    {
        int center_x = puzzle.pair_coordinates[number][0] + puzzle.pair_coordinates[number][2];
        sum_x += center_x;
        sum_x_squared += center_x * center_x;
        int center_y = puzzle.pair_coordinates[number][1] + puzzle.pair_coordinates[number][3];
        sum_y += center_y;
        sum_y_squared += center_y * center_y;
    }
    int var_x = sum_x_squared * puzzle.max_number - sum_x * sum_x;
    int var_y = sum_y_squared * puzzle.max_number - sum_y * sum_y;
    return var_x + var_y;
}
