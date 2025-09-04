// optimizer.cpp
#include "optimizer.hpp"
#include "utils.hpp"
#include "evalution.hpp"
#include <functional>
#include <array>
#include <iostream>


void move_pair1(Puzzle& puzzle, int target_entity, int goal_x, int goal_y,int layer)
{
    int current_x1 = puzzle.pair_coordinates[target_entity][0];
    int current_y1 = puzzle.pair_coordinates[target_entity][1];
    int current_x2 = puzzle.pair_coordinates[target_entity][2];
    int current_y2 = puzzle.pair_coordinates[target_entity][3];

    int target_pair_x, target_pair_y;
    int goal_pair_x = goal_x + 1;
    int goal_pair_y = goal_y;

    // goal_x, goal_y にあるのが1つ目と仮定し、2つ目を動かす
    if (current_x1 == goal_x && current_y1 == goal_y)
    {
        target_pair_x = current_x2;
        target_pair_y = current_y2;
    }
    else // goal_x, goal_y にあるのが2つ目と仮定し、1つ目を動かす
    {
        target_pair_x = current_x1;
        target_pair_y = current_y1;
    }

    if (goal_pair_x == target_pair_x && goal_pair_y == target_pair_y)
    {
        return;
    }

    // targetがgoalのすぐ下にある場合
    if (goal_pair_x - 1 == target_pair_x && goal_pair_y + 1 == target_pair_y)
    {
        puzzle.apply_rotation({goal_pair_x - 1, goal_pair_y, 2});
        return;
    }

    // targetが上に達して、かつgoalの右側にあるとき
    if (goal_pair_x < target_pair_x && goal_pair_y == target_pair_y)
    {
        int max_rotate_size;
        if (puzzle.field_size - target_pair_y < target_pair_x - goal_pair_x)
        {
            max_rotate_size = puzzle.field_size;
        }
        else
        {
            max_rotate_size = target_pair_x - goal_pair_x + 1;
        }
        puzzle.apply_rotation({target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size});
        puzzle.apply_rotation({target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size});
        puzzle.apply_rotation({target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size});
        return;
    }
    // targerがgoalのすぐ右下にある場合
    if(goal_pair_x == target_pair_x && target_pair_y - 1 ==  goal_pair_y){
        puzzle.apply_rotation({target_pair_x-1,target_pair_y,2});
        return;
    }

    // targetがgoalの左に達した場合
    if (goal_pair_x == target_pair_x)
    {
        if (goal_pair_y == target_pair_y)
            return;
        int max_roteta_size;
        if (puzzle.field_size - layer - target_pair_x < target_pair_y - goal_pair_y + 1 )
        {
            max_roteta_size = puzzle.field_size - target_pair_x -layer;
        }
        else
        {
            max_roteta_size = target_pair_y - goal_pair_y + 1;
        }
        // targetが端にあるときのための処理
        if (target_pair_x + 1  == puzzle.field_size -layer){
            puzzle.apply_rotation({target_pair_x - 1, target_pair_y - 1, 2});
            return;
        }
        puzzle.apply_rotation({target_pair_x, target_pair_y - max_roteta_size + 1, max_roteta_size});
        return;
    }

    // targetが上に達して、かつgoalの左側にあるとき
    if (goal_pair_x > target_pair_x && goal_pair_y + 1 == target_pair_y)
    {
        int max_rotate_size;
        if (puzzle.field_size - target_pair_y - layer < goal_pair_x - target_pair_x)
        {
            max_rotate_size = puzzle.field_size - target_pair_y - layer;
        }
        else
        {
            max_rotate_size = goal_pair_x - target_pair_x;
        }
        puzzle.apply_rotation({target_pair_x, target_pair_y, max_rotate_size});
        return;
    }

    // 動かしたいエンティティが目標の右下にある場合
    if (goal_pair_x < target_pair_x && goal_pair_y < target_pair_y)
    {
        int max_rotate_size;
        if (target_pair_x - goal_pair_x < target_pair_y - goal_pair_y)
        {
            max_rotate_size = target_pair_x - goal_pair_x + 1;
        }
        else
        {
            max_rotate_size = target_pair_y - goal_pair_y + 1;
        }
        puzzle.apply_rotation({target_pair_x - max_rotate_size + 1, target_pair_y - max_rotate_size + 1, max_rotate_size});
        return;
    }
    // 動かしたいエンティティが目標の左下にある場合
    if (goal_pair_x > target_pair_x && goal_pair_y < target_pair_y)
    {
        int max_rotate_size;
        if (goal_pair_x - target_pair_x < target_pair_y - goal_pair_y)
        {
            max_rotate_size = goal_pair_x - target_pair_x + 1;
        }
        else
        {
            max_rotate_size = target_pair_y - goal_pair_y;
        }
        puzzle.apply_rotation({target_pair_x, target_pair_y - max_rotate_size + 1, max_rotate_size});
        return;
    }
}

void solve1(Puzzle& puzzle) {
    cout << "rest pair:" << puzzle.max_number - count_pairs(puzzle) << endl;
    if(count_pairs(puzzle)== puzzle.max_number || count_pairs(puzzle) == puzzle.max_number - 2 ) return;
    for (int i = 0; i < puzzle.field_size/2; i += 2)
    {
       
        for (int x = i; x < puzzle.field_size-i; x += 2)
        {
            while (puzzle.field.grid[i][x] != puzzle.field.grid[i][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i][x], x, i,i);
                puzzle.print_field();
            }
        }
        for (int x = i; x < puzzle.field_size-i; x += 2)
        {
            while (puzzle.field.grid[i + 1][x] != puzzle.field.grid[i + 1][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i + 1][x], x, i+1,i);
                puzzle.print_field();
            }
        }
        puzzle.apply_rotation({0, 0, puzzle.field_size});
        puzzle.print_field();
        
        for (int x = i ; x < puzzle.field_size-i-2; x += 2)
        {
            while (puzzle.field.grid[i][x] != puzzle.field.grid[i][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i][x], x, i,i+2);
                puzzle.print_field();
            }
        }
        for (int x = i ; x < puzzle.field_size-i-2; x += 2)
        {
            while (puzzle.field.grid[i + 1][x] != puzzle.field.grid[i + 1][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i + 1][x], x, i + 1,i+2);
                puzzle.print_field();
            }
        }
        // 最後に4×4のフィールドができた時に、2回の回転で終わらせる
        if(puzzle.field_size/2 - i ==2) break;
        puzzle.apply_rotation({0, 0, puzzle.field_size});
        puzzle.print_field();
        for (int x = i ; x < puzzle.field_size-i-2; x += 2)
        {
            while (puzzle.field.grid[i][x] != puzzle.field.grid[i][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i][x], x, i,i+2);
                puzzle.print_field();
            }
        }
        for (int x = i ; x < puzzle.field_size-i-2; x += 2)
        {
            while (puzzle.field.grid[i + 1][x] != puzzle.field.grid[i + 1][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i + 1][x], x, i + 1,i+2);
                puzzle.print_field();
            }
        }
        puzzle.apply_rotation({0, 0, puzzle.field_size});
        puzzle.print_field();
        for (int x = i +2; x < puzzle.field_size-i-2; x += 2)
        {
            while (puzzle.field.grid[i][x] != puzzle.field.grid[i][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i][x], x, i,i+2);
                puzzle.print_field();
            }
        }
        for (int x = i +2; x < puzzle.field_size-i-2; x += 2)
        {
            while (puzzle.field.grid[i + 1][x] != puzzle.field.grid[i + 1][x + 1])
            {
                move_pair1(puzzle,puzzle.field.grid[i + 1][x], x, i + 1,i+2);
                puzzle.print_field();
            }
        }
    }
}

void find_and_apply_best_move(Puzzle& puzzle, const function<float(Puzzle&)>& evaluator) {
    float best_value = -1000000;
    Operation best_op;

    for (int y = 0; y < puzzle.field_size - 1; y++) {
        for (int x = 0; x < puzzle.field_size - 1; x++) {
            for (int n = 2; x + n <= puzzle.field_size && y + n <= puzzle.field_size; n++) {
                Operation op = {x,y,n};
                if(!puzzle.check_rotation_value(op)) break;             
                puzzle.rotate_for_simulation(op);
                if(puzzle.field_history.count(puzzle.field)) continue;
                float current_value = evaluator(puzzle);
                if (current_value > best_value) {
                    best_value = current_value;
                    best_op = op;
                }
                puzzle.undo_rotation(op);
            }
        }
    }
    puzzle.apply_rotation(best_op);
    puzzle.print_field();
}

void solve3(Puzzle& puzzle, int max_time) {
    for (int i = 0; i < max_time; i++) {
        find_and_apply_best_move(puzzle, evalution_func1);
        cout << "pair:" << 100 * count_pairs(puzzle) / puzzle.max_number <<"%"<< endl;
        if (count_pairs(puzzle) == puzzle.max_number) return;
    }
}