// optimizer.cpp
#include "optimizer.hpp"
#include "utils.hpp"
#include "evalution.hpp"
#include "beam_node.hpp"
#include "steps_table.hpp"
#include <functional>
#include <array>
#include <iostream>

void find_and_apply_best_move(Puzzle &puzzle, const function<float(Puzzle &)> &evaluator)
{
    float best_value = -1000000;
    Operation best_op;

    for (int y = 0; y < puzzle.field_size - 1; y++)
    {
        for (int x = 0; x < puzzle.field_size - 1; x++)
        {
            for (int n = 2; x + n <= puzzle.field_size && y + n <= puzzle.field_size; n++)
            {
                if( x == 0 && y == 0 && n == puzzle.field_size) continue;
                Operation op = {x, y, n};
                if (!puzzle.check_rotation_value(op))
                    break;
                puzzle.rotate_for_simulation(op);
                if (puzzle.field_history.count(puzzle.calculate_hash()))
                    continue;
                float current_value = evaluator(puzzle);
                if (current_value > best_value)
                {
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

// a以上b以下のランダムな整数
int rand_int(int a, int b)
{
    return a + rand() % (b - a + 1);
}
//貪欲法
void greedy_algorithm(Puzzle &puzzle, int max_time)
{
    for (int i = 0; i < max_time; i++)
    {
        find_and_apply_best_move(puzzle, evalution_func2);
        cout << "pair:" << 100 * count_pairs(puzzle) / puzzle.max_pair_number << "%" << endl;
        evalution_func2_for_check(puzzle);
        if (count_pairs(puzzle) == puzzle.max_pair_number)
            return;
    }
}

// 2.1 全ての手の評価値を計算し、上位10手を返す
vector<Operation> best_operations_all(Puzzle &puzzle,const function<float(Puzzle &)> &evaluator)
{
    vector<pair<float, Operation>> candidates(0);
    for (int y = 0; y < puzzle.field_size - 1; y++)
    {
        for (int x = 0; x < puzzle.field_size - 1; x++)
        {
            for (int n = 2; x + n <= puzzle.field_size && y + n <= puzzle.field_size; n++)
            {
                Operation op = {x, y, n};
                puzzle.rotate_for_simulation(op);
                candidates.push_back(make_pair(evaluator(puzzle), op));
                puzzle.undo_rotation(op);
            }
        }   
    }
    sort(candidates.rbegin(), candidates.rend());
    vector<Operation> result(10);
    for (int index = 0; index < 10; index++)
    {
        result[index] = candidates[index].second;
    }
    return result;
}

// 2.2 ランダムにの手の評価値を計算し、上位10手を返す
vector<Operation> best_operations_random(const Puzzle &puzzle,const function<float(Puzzle &)> &evaluator,int num_sample)
{
    // 取りうる全ての手よりもサンプル数
    num_sample = min(num_sample,steps_table[puzzle.field_size]);
    // puzzleをpureな関数にするため必要
    Puzzle temp_puzzle = puzzle;
    set<pair<float,Operation>> candidates;
    while(candidates.size() < num_sample)
    {
        int n = rand_int(2, temp_puzzle.field_size);
        int x = rand_int(0, temp_puzzle.field_size - n), y = rand_int(0, temp_puzzle.field_size - n);
        if(n == temp_puzzle.field_size && x == 0 && y == 0) continue;
        Operation op = {x, y, n};
        
        temp_puzzle.rotate_for_simulation(op);
        //evalution_func2_for_check(temp_puzzle);
        candidates.insert(make_pair(evaluator(temp_puzzle),op));
        temp_puzzle.undo_rotation(op);        
    }

    vector<Operation> result;
    auto it = candidates.rbegin();
    for (int i = 0; i < 10 && it != candidates.rend(); ++i,++it)
    {
        result.push_back(it->second);
    }
    return result;
}

void beam_search_step(Puzzle& puzzle, int search_depth,size_t beam_width,int commit_step){
    if(search_depth < commit_step){
        cout << "This parameter is invalid" << "(search_depth: " << search_depth << " < commit_step: " << commit_step << ")" << endl;
        exit(1);
    }
    vector<BeamNode> beam;
    beam.emplace_back(puzzle, vector<Operation>(), evalution_func2(puzzle));

    for (int depth = 0; depth < search_depth; ++depth) {
        vector<BeamNode> next_beam;
        next_beam.reserve(beam.size()*10);
        for (auto& node : beam) {
            // 候補手をランダムに生成
            vector<Operation> candidates = best_operations_random(node.puzzle, evalution_func2,100);
            for (const auto& op : candidates) {
                node.puzzle.rotate_for_simulation(op);
                float score = evalution_func2(node.puzzle);
                vector<Operation> next_ops = node.ops;
                next_ops.push_back(op);
                next_beam.emplace_back(node.puzzle, next_ops, score);
                // Puzzleオブジェクトのコピーの削減のため、履歴を後で追加する
                next_beam.back().puzzle.field_history.insert(node.puzzle.calculate_hash());
                node.puzzle.undo_rotation(op);
            }
        }
        // 評価値で降順ソート
        sort(next_beam.begin(), next_beam.end(), [](const BeamNode& a, const BeamNode& b) {
            return a.score > b.score;
        });
        // 上位beam_width個だけ残す
        if (next_beam.size() > beam_width) next_beam.resize(beam_width);
        beam = move(next_beam);
    }
    // 最終的に最も評価値が高いノードを選択
    if (!beam.empty()) {
        const auto& best = beam.front();
        // 操作列を元のpuzzleに適用
        for (int i = 0; i < commit_step;i++) {
            puzzle.apply_rotation(best.ops[i]);
        }
    }
}

//ビームサーチ
void beam_search(Puzzle &puzzle, int search_depth,size_t beam_width,int commit_step, int max_time)
{
    string evalution_output = "";
    string pair_num_output = "";
    for (int i = 0; i < max_time/commit_step; i++)
    {
        beam_search_step(puzzle,search_depth,beam_width,commit_step);
        evalution_output += to_string(evalution_func2_for_check(puzzle))+",";
        pair_num_output += to_string(100 * count_pairs(puzzle) / puzzle.max_pair_number)+",";
        cout << "pair:" << 100 * count_pairs(puzzle) / puzzle.max_pair_number << "%" << endl;
        puzzle.print_field();
        if (count_pairs(puzzle) == puzzle.max_pair_number){
            cout << pair_num_output << endl;
            cout << evalution_output << endl;
            return;
        }    
    }
    cout << pair_num_output << endl;
    cout << evalution_output << endl;
}