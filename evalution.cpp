#include "evalution.hpp"
#include "puzzle.hpp"
#include <iostream>

int count_pairs(Puzzle &puzzle)
{
    int counter = 0;
    for (int i = 0; i < puzzle.max_pair_number; i++)
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
    return static_cast<float>(count_pairs(puzzle)) / static_cast<float>(puzzle.max_pair_number);
}

float evaluate_square_avg_distance(Puzzle &puzzle)
{
    if (puzzle.max_pair_number == 0)
        return 0;
    float total_dist = 0;
    for (int i = 0; i < puzzle.max_pair_number; i++)
    {
        float dist = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2]) + abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
        total_dist += pow(dist, 2) / puzzle.field_size;
    }
    return total_dist / puzzle.max_pair_number;
}

// ペア数とペア間の距離の二乗を参考に評価値を計算
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
// ペアがフィールドの隅に集中しているほど値が大きくなる
float evaluatie_edge_pairs(Puzzle &puzzle, float edge_weight)
{
    float value = 0;
    for (int i = 0; i < puzzle.max_pair_number; i++)
    {
        int colm_dis = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2]);
        int row_dis = abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
        if (colm_dis + row_dis == 1)
        {
            value += 100;
            float max_dist_from_edge = (puzzle.field_size - 1) / 2.0;
            for (int j = 0; j < 4; j++)
            {   
                float dist_from_edge = min(puzzle.pair_coordinates[i][j], puzzle.field_size - 1 - puzzle.pair_coordinates[i][j]);
                float normalized_dist = dist_from_edge / max_dist_from_edge;
                value -= edge_weight * normalized_dist;
            }   
            if(value < 0){
                value = 0;
            }
        }
    }
    return value/static_cast<float>(puzzle.max_pair_number); 
}

// 1.2 ペア候補の重心の分散を評価する。値は大きければ大きいほど良い
float evaluete_center_variance(Puzzle &puzzle)
{
    int sum_x = 0, sum_x_squared = 0, sum_y = 0, sum_y_squared = 0;
    for (int number = 0; number < puzzle.max_pair_number; number++)
    {
        int center_x = puzzle.pair_coordinates[number][0] + puzzle.pair_coordinates[number][2];
        sum_x += center_x;
        sum_x_squared += center_x * center_x;
        int center_y = puzzle.pair_coordinates[number][1] + puzzle.pair_coordinates[number][3];
        sum_y += center_y;
        sum_y_squared += center_y * center_y;
    }
    int var_x = sum_x_squared * puzzle.max_pair_number - sum_x * sum_x;
    int var_y = sum_y_squared * puzzle.max_pair_number - sum_y * sum_y;
    // 標準化
    float normalized_var = static_cast<float>(var_x + var_y) / static_cast<float>(puzzle.field_size * puzzle.field_size);
    return normalized_var;
}

float evalution_func2_internal(Puzzle &puzzle,bool test)
{
    float w_pair_dis = 0;
    float w_pair_num = 1;
    float w_pair_edge = 25;
    float w_pair_vartex = 0;
    float pair_dis = w_pair_dis * evaluate_square_avg_distance(puzzle);
    float pair_edge = w_pair_num * evaluatie_edge_pairs(puzzle,w_pair_edge);
    float pair_vartex = w_pair_vartex * evaluete_center_variance(puzzle);
    if(test){
        cout << "pair_dis:" << pair_dis << endl;
        cout << "pair_edge:" << pair_edge << endl;
        cout << "pair_vartex:" << pair_vartex << endl;
    }
    return pair_edge + pair_vartex - pair_dis;
}

float evalution_func2(Puzzle &puzzle){
    return evalution_func2_internal(puzzle,false);
}

float evalution_func2_for_check(Puzzle &puzzle)
{
    return evalution_func2_internal(puzzle,true);
}
