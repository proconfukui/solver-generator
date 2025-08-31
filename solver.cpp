#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <random>
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

// peforemance[n][0][0]: 評価値
// peforemance[n][1][0]: 導きのx座標
// peforemance[n][1][1]: 導きのy座標
// peforemance[n][1][2]: 導きの大きさ
vector<vector<vector<int>>> perpormance;

vector<int> ops_x = vector<int>(0);
vector<int> ops_y = vector<int>(0);
vector<int> ops_n = vector<int>(0);

random_device seed_gen;
mt19937 engine(seed_gen());

void load_problem()
{
    ifstream input("problem.json");
    if (!input)
    {
        cerr << "Error: problem.json not found." << endl;
        exit(1);
    }
    json problem;
    try
    {
        input >> problem;
        input.close();

        // contains()でキーの存在を確認してからアクセスする
        if (problem.contains("problem") &&
            problem["problem"].contains("field") &&
            problem["problem"]["field"].contains("size") &&
            problem["problem"]["field"]["size"].is_number_integer())
        {
            field_size = problem["problem"]["field"]["size"].get<int>();
        }
        else
        {
            throw std::runtime_error("JSON key 'size' is missing or not an integer.");
        }

        if (problem["problem"]["field"].contains("entities") &&
            problem["problem"]["field"]["entities"].is_array())
        {
            field = problem["problem"]["field"]["entities"].get<vector<vector<int>>>();
        }
        else
        {
            throw std::runtime_error("JSON key 'entities' is missing or not a 2D array.");
        }
    }
    catch (const nlohmann::json::parse_error &e)
    {
        cerr << "JSON parse error: " << e.what() << endl;
        exit(1);
    }
    catch (const nlohmann::json::type_error &e)
    {
        cerr << "JSON type error: " << e.what() << endl;
        exit(1);
    }
    catch (const std::exception &e)
    {
        cerr << "An error occurred: " << e.what() << endl;
        exit(1);
    }

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
    cout << "   ";
    for (int i = 0; i < field_size; i++)
    {
        cout << setw(3) << i << "|";
    }
    cout << endl;
    for (int y = 0; y < field_size; y++)
    {
        cout << setw(2) << y << "|";
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

void rotate_field_temp(int x, int y, int n){
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

void move_pair1(int target_entity, int goal_x, int goal_y,int layer)
{

    int current_x1 = pair_coordinates[target_entity][0];
    int current_y1 = pair_coordinates[target_entity][1];
    int current_x2 = pair_coordinates[target_entity][2];
    int current_y2 = pair_coordinates[target_entity][3];

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
        rotate_field(goal_pair_x - 1, goal_pair_y, 2);
        return;
    }

    // targetが上に達して、かつgoalの右側にあるとき
    if (goal_pair_x < target_pair_x && goal_pair_y == target_pair_y)
    {
        int max_rotate_size;
        if (field_size - target_pair_y < target_pair_x - goal_pair_x)
        {
            max_rotate_size = field_size;
        }
        else
        {
            max_rotate_size = target_pair_x - goal_pair_x + 1;
        }
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size);
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size);
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size);
        return;
    }

    // targetがgoalの左に達した場合
    if (goal_pair_x == target_pair_x)
    {
        if (goal_pair_y == target_pair_y)
            return;
        int max_roteta_size;
        if (field_size - layer - target_pair_x < target_pair_y - goal_pair_y + 1 )
        {
            max_roteta_size = field_size - target_pair_x -layer;
        }
        else
        {
            max_roteta_size = target_pair_y - goal_pair_y + 1;
        }
        // targetが端にあるときのための処理
        if (target_pair_x + 1  == field_size -layer){
            rotate_field(target_pair_x - 1, target_pair_y - 1, 2);
            return;
        }
        rotate_field(target_pair_x, target_pair_y - max_roteta_size + 1, max_roteta_size);
        return;
    }

    // targetが上に達して、かつgoalの左側にあるとき
    if (goal_pair_x > target_pair_x && goal_pair_y + 1 == target_pair_y)
    {
        int max_rotate_size;
        if (field_size - target_pair_y < goal_pair_x - target_pair_x)
        {
            max_rotate_size = field_size - target_pair_y;
        }
        else
        {
            max_rotate_size = goal_pair_x - target_pair_x;
        }
        rotate_field(target_pair_x, target_pair_y, max_rotate_size);
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
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y - max_rotate_size + 1, max_rotate_size);
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
        rotate_field(target_pair_x, target_pair_y - max_rotate_size + 1, max_rotate_size);
        return;
    }
}

void solve1()
{

    for (int i = 0; i < field_size/2; i += 2)
    {
        for (int x = i; x < field_size-i; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i);
                print_field();
            }
        }
        for (int x = i; x < field_size-i; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i+1,i);
                print_field();
            }
        }
        rotate_field(0, 0, field_size);
        print_field();
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i+2);
                print_field();
            }
        }
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i + 1,i+2);
                print_field();
            }
        }
        rotate_field(0, 0, field_size);
        print_field();
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i+2);
                print_field();
            }
        }
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i + 1,i+2);
                print_field();
            }
        }
        rotate_field(0, 0, field_size);
        print_field();
        for (int x = i +2; x < field_size-i-2; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i+2);
                print_field();
            }
        }
        for (int x = i +2; x < field_size-i-2; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i + 1,i+2);
                print_field();
            }
        }
    }
}

// ペアの数を数える
int count_pair(){
    int counter = 0;
    for(int i = 0;i<max_number;i++){
        int colm_dis = abs(pair_coordinates[i][0] - pair_coordinates[i][2]);
        int row_dis = abs(pair_coordinates[i][1] - pair_coordinates[i][3]);
        if(colm_dis == 0 && row_dis == 1 || colm_dis == 1 && row_dis == 0) counter++;
    }
    return counter;
}

// ペア同士のマンハッタン距離の平均を求める
int evaluate_manhattan_distanece_average(){
    int counter = 0;
    for(int i = 0;i<max_number;i++){
        int colm_dis = abs(pair_coordinates[i][0] - pair_coordinates[i][2]);
        int row_dis = abs(pair_coordinates[i][1] - pair_coordinates[i][3]);
        counter += colm_dis + row_dis;
    }
    return counter/max_number;
}

// ペアの数が多いものを優先する
void move2(){
    int max_pair = 0;
    // すべての手でペア数が0のときのために初期値をランダムに設定する
    uniform_int_distribution<> distrib(0,field_size-1);
    int random_x = distrib(seed_gen);
    int random_y = distrib(seed_gen);
    if(random_x > random_y){
        uniform_int_distribution(1,field_size - random_x);
    } else{
        uniform_int_distribution(1,field_size - random_y);
    }
    int random_n = distrib(seed_gen);
    array<int,3> ops = {random_x,random_y,random_n};
    for(int i = 0 ; i < field_size - 1 ; i++){
        for(int j = 0 ; j < field_size -1 ; j++){
            for(int k = 2; k <= field_size ; k++){
                int colm_limit = field_size - i;
                int row_limit = field_size - j;
                if(!(k < colm_limit && k < row_limit)) break;
                rotate_field_temp(i,j,k);
                if(max_pair < count_pair()){
                    max_pair = count_pair();
                    ops = {i,j,k};
                }
                //元の位置に戻すため3回回転、逆回転1回で済むため用修正
                rotate_field_temp(i,j,k);
                rotate_field_temp(i,j,k);
                rotate_field_temp(i,j,k);
            }
        }
    }

    rotate_field(ops[0],ops[1],ops[2]);
}

void solve2(int max_time){
    for(int i = 0; i<max_time;i++){
        move2();
        cout <<  "ペアの数：" << count_pair() << endl;
        if(count_pair()==max_number) return;
    }
}

// w1はペア数の重み、w2はペアの距離の重み
void move3(int w1,int w2){
    for(int i = 0 ; i < field_size - 1 ; i++){
        for(int j = 0 ; j < field_size -1 ; j++){
            for(int k = 2; k <= field_size ; k++){
                int colm_limit = field_size - i;
                int row_limit = field_size - j;
                if(!(k < colm_limit && k < row_limit)) break;
                rotate_field_temp(i,j,k);
                int value = w1 * count_pair() + w2 * 10 / evaluate_manhattan_distanece_average(); 
                cout << "ペア数："<< count_pair() << endl;
                cout << "マンハッタン距離の平均："<<evaluate_manhattan_distanece_average() << endl;
                cout << "評価："<<value << endl;
                perpormance.push_back({{value},{i,j,k}});
                //元の位置に戻すため3回回転、逆回転1回で済むため用修正
                rotate_field_temp(i,j,k);
                rotate_field_temp(i,j,k);
                rotate_field_temp(i,j,k);
            }
        }
    }
    vector<vector<int>> max_value_pair={{0},{0,0,0}};
    for(int i = 0; i < perpormance.size();i++){
        if(max_value_pair[0][0] < perpormance[i][0][0]){
            max_value_pair = perpormance[i];
        }
    }
    rotate_field(max_value_pair[1][0],max_value_pair[1][1],max_value_pair[1][2]);
}


void solve3(int max_time){
    for(int i = 0; i<max_time;i++){
        move3(2,1);
        cout <<  "ペアの数：" << count_pair() << endl;
        if(count_pair()==max_number) return;
    }
}

int main()
{
    load_problem();

    solve3(100);    


    export_answer();
}
