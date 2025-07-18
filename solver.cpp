#include <vector>
#include <iostream>
#include <fstream>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

int field_size;
vector<vector<int>> field;

int max_number;
// pair_coordinates[n][0]: 値がnのエンティティ1つ目のX
// pair_coordinates[n][1]: 値がnのエンティティ1つ目のY
// pair_coordinates[n][2]: 値がnのエンティティ2つ目のX
// pair_coordinates[n][3]: 値がnのエンティティ2つ目のY
vector<vector<int>> pair_coordinates;

vector<int> ops_x = vector<int>(0);
vector<int> ops_y = vector<int>(0);
vector<int> ops_n = vector<int>(0);

void load_problem()
{
    ifstream input("problem.json");
    json problem;
    input >> problem;
    input.close();
    field_size = problem["problem"]["field"]["size"].get<int>();
    field = problem["problem"]["field"]["entities"].get<vector<vector<int>>>();
    max_number = field_size * field_size / 2;
    pair_coordinates = vector<vector<int>>(max_number, vector<int>(4, -1));
    for (int y = 0; y < field_size; y++)
    {
        for (int x = 0; x < field_size; x++)
        {
        int number = field[y][x];
            if (pair_coordinates[number][0] == -1)
            {
                pair_coordinates[number][0] = x;
                pair_coordinates[number][1] = y;
            }
            else
            {
                pair_coordinates[number][2] = x;
                pair_coordinates[number][3] = y;
            }
        }
    }
}

// デバッグ用
void print_field()
{
    cout << "手数: " << ops_x.size() << endl;
    for (int y = 0; y < field_size; y++)
    {
        for (int x = 0; x < field_size; x++)
        {
            cout << setw(3) << field[y][x] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// デバッグ用
void print_pair_coordinates()
{
    for (int number = 0; number < max_number; number++)
    {
        cout << setw(3) << number 
            << " (" << setw(2) << pair_coordinates[number][0] 
            << ", " << setw(2) << pair_coordinates[number][1] 
            << "), (" << setw(2) << pair_coordinates[number][2] 
            << ", " << setw(2) << pair_coordinates[number][3] 
            << ")" << endl;
    }
    cout << endl;
}

void rotate_field(int x, int y, int n)
{
    if (n < 2 || x < 0 || x + n > field_size || y < 0 || y + n > field_size)
    {
        cout << "導きが無効です(x: " << x << ", y: " << y << ", n: " << n << ")" << endl;
        return;
    }

    for (int number = 0; number < max_number; number++)
    {
        if (x <= pair_coordinates[number][0] && 
            pair_coordinates[number][0] < x + n && 
            y <= pair_coordinates[number][1] && 
            pair_coordinates[number][1] < y + n)
        {
            int dx = pair_coordinates[number][0] - x;
            int dy = pair_coordinates[number][1] - y;
            pair_coordinates[number][0] = x + n - 1 - dy;
            pair_coordinates[number][1] = y + dx;
        }
        if (x <= pair_coordinates[number][2] && 
            pair_coordinates[number][2] < x + n && 
            y <= pair_coordinates[number][3] && 
            pair_coordinates[number][3] < y + n)
        {
            int dx = pair_coordinates[number][2] - x;
            int dy = pair_coordinates[number][3] - y;
            pair_coordinates[number][2] = x + n - 1 - dy;
            pair_coordinates[number][3] = y + dx;
        }
    }

    vector<vector<int>> memo = vector<vector<int>>(n, vector<int>(n));
    for (int dy = 0; dy < n; dy++)
    {
        for (int dx = 0; dx < n; dx++)
        {
            memo[dy][dx] = field[y + n - 1 - dx][x + dy];
        }
    }
    for (int dy = 0; dy < n; dy++)
    {
        for (int dx = 0; dx < n; dx++)
        {
            field[y + dy][x + dx] = memo[dy][dx];
        }
    }

    ops_x.push_back(x);
    ops_y.push_back(y);
    ops_n.push_back(n);
}

void export_answer()
{
    json answer;
    for (int index = 0; index < ops_x.size(); index++)
    {
        answer["ops"][index]["x"] = ops_x[index];
        answer["ops"][index]["y"] = ops_y[index];
        answer["ops"][index]["n"] = ops_n[index];
    }
    ofstream output("answer.json");
    output << setw(4) << answer << endl;
    output.close();
}

void move_pair1(int target_entity, int goal_x, int goal_y)
{
    int current_x1 = pair_coordinates[target_entity][0];
    int current_y1 = pair_coordinates[target_entity][1];
    int current_x2 = pair_coordinates[target_entity][2];
    int current_y2 = pair_coordinates[target_entity][3];

    int target_pair_x, target_pair_y;

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

    // 目標地点 (goal_x, goal_y) の隣に移動させる
    // まずX座標を合わせる
    while (target_pair_x != goal_x + 1) // goal_x の右隣に移動
    {
        if (target_pair_x < goal_x + 1)
        {
            // 右に移動させる
            // 回転の起点を調整して、負のインデックスにならないようにする
            int rx = target_pair_x;
            int ry = target_pair_y;
            if (ry == field_size - 1) ry--; // 下端にいる場合は上にずらす
            if (rx == field_size - 1) rx--; // 右端にいる場合は左にずらす
            rotate_field(rx, ry, 2);
        }
        else // target_pair_x > goal_x + 1
        {
            // 左に移動させる
            int rx = target_pair_x - 1;
            int ry = target_pair_y;
            if (ry == field_size - 1) ry--; // 下端にいる場合は上にずらす
            if (rx < 0) rx = 0; // 左端にいる場合は0に固定
            rotate_field(rx, ry, 2);
        }
        // 座標が更新されるので再取得
        current_x1 = pair_coordinates[target_entity][0];
        current_y1 = pair_coordinates[target_entity][1];
        current_x2 = pair_coordinates[target_entity][2];
        current_y2 = pair_coordinates[target_entity][3];
        if (current_x1 == goal_x && current_y1 == goal_y)
        {
            target_pair_x = current_x2;
            target_pair_y = current_y2;
        }
        else
        {
            target_pair_x = current_x1;
            target_pair_y = current_y1;
        }
    }

    // 次にY座標を合わせる
    while (target_pair_y != goal_y)
    {
        if (target_pair_y < goal_y)
        {
            // 下に移動させる
            int rx = target_pair_x;
            int ry = target_pair_y;
            if (rx == field_size - 1) rx--; // 右端にいる場合は左にずらす
            if (ry == field_size - 1) ry--; // 下端にいる場合は上にずらす
            rotate_field(rx, ry, 2);
        }
        else // target_pair_y > goal_y
        {
            // 上に移動させる
            int rx = target_pair_x;
            int ry = target_pair_y - 1;
            if (rx == field_size - 1) rx--; // 右端にいる場合は左にずらす
            if (ry < 0) ry = 0; // 上端にいる場合は0に固定
            rotate_field(rx, ry, 2);
        }
        // 座標が更新されるので再取得
        current_x1 = pair_coordinates[target_entity][0];
        current_y1 = pair_coordinates[target_entity][1];
        current_x2 = pair_coordinates[target_entity][2];
        current_y2 = pair_coordinates[target_entity][3];
        if (current_x1 == goal_x && current_y1 == goal_y)
        {
            target_pair_x = current_x2;
            target_pair_y = current_y2;
        }
        else
        {
            target_pair_x = current_x1;
            target_pair_y = current_y1;
        }
    }
}

void solve2()
{
    for (int y = 0; y < field_size; y++) // 全ての行を対象にする
    {
        for (int x = 0; x < field_size - 1; x++) // 右隣にペアを揃えるので、最後の列は対象外
        {
            int target = field[y][x];
            // (y, x) にある target のペアを (y, x+1) に移動させる
            // ただし、既にペアが揃っている場合はスキップ
            if (field[y][x+1] == target) continue;

            move_pair1(target, x, y); // target のペアの片方を (x, y) の隣に移動
        }
    }
}


void solve1(){
    for (int y = 0; y < field_size; y+=2)
    {
        for (int x = 0; x < field_size; x+=2)
        {
            int target = field[y][x];
            // ペアが既に揃っていたらスキップ
            if (target == field[y][x+1]) continue; 
            int target_pair_x = pair_coordinates[target][2];
            int target_pair_y = pair_coordinates[target][3];

            // X 0 3 3  
            // 1 1 2 3 
            // 2 1 2 2 
            // 2 2 2 3 
            // 上は4×4の場合の例。何回でXの右にペアを作れるかを示している
            // 下の二つのif文はは1回でペアを揃えられる時の条件とその導きである
            if (x == target_pair_x && target_pair_y < field_size - x - 1){
                rotate_field(x+1, y, target_pair_y +1 );
                continue;
            }
            if(x + 1 == target_pair_x && y == target_pair_y){
                rotate_field(x, y, 2);
                continue;
            }
                

            
        }
    }
}

int main()
{
    load_problem();

    print_field();
    solve2(); // solve2 を呼び出すように変更
    print_field();

    export_answer();
}
