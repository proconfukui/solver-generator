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


struct Point
{
    int x = 0;
    int y = 0;

    // 座標の比較を簡単にするための等価演算子
    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }
};

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

// 回転のコアロジックを実装する内部関数
// direction: 1 で時計回り, -1 で反時計回り
void rotate_field_internal(int x, int y, int n, int direction)
{
    if (n < 2 || x < 0 || x + n > field_size || y < 0 || y + n > field_size)
    {
        cout << "導きが無効です(x: " << x << ", y: " << y << ", n: " << n << ")" << endl;
        return;
    }

    // pair_coordinatesの更新 (ループで簡潔に)
    for (int number = 0; number < max_number; number++)
    {
        for (int i = 0; i <= 2; i += 2) // 1つ目の座標 (0,1) と2つ目の座標 (2,3) を処理
        {
            int& px = pair_coordinates[number][i];
            int& py = pair_coordinates[number][i + 1];

            if (x <= px && px < x + n && y <= py && py < y + n)
            {
                int dx = px - x;
                int dy = py - y;
                if (direction == 1) // 時計回り
                {
                    px = x + n - 1 - dy;
                    py = y + dx;
                }
                else // 反時計回り
                {
                    px = x + dy;
                    py = y + n - 1 - dx;
                }
            }
        }
    }

    // fieldの更新
    vector<vector<int>> memo(n, vector<int>(n));
    for (int dy = 0; dy < n; dy++)
    {
        for (int dx = 0; dx < n; dx++)
        {
            if (direction == 1) // 時計回り
                memo[dy][dx] = field[y + n - 1 - dx][x + dy];
            else // 反時計回り
                memo[dy][dx] = field[y + dx][x + n - 1 - dy];
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

// 元の rotate_field の役割を担う関数
void apply_rotation(int x, int y, int n)
{
    rotate_field_internal(x, y, n, 1); // 時計回りに回転
    ops_x.push_back(x);
    ops_y.push_back(y);
    ops_n.push_back(n);
}

// 試行錯誤用の回転関数 (操作を記録しない)
void rotate_for_simulation(int x, int y, int n)
{
    rotate_field_internal(x, y, n, 1);
}

// 試行錯誤した操作を元に戻す関数
void undo_rotation(int x, int y, int n)
{
    rotate_field_internal(x, y, n, -1); // 反時計回りに回転
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

/**
 * @brief 指定したエンティティ(アンカー)の隣に、その相方(パートナー)を移動させる
 * @param target_entity アンカーとなるエンティティの番号
 * @param anchor_pos アンカーの座標
 * @param layer 現在処理中の層の深さ (回転領域を制限するために使用)
 */
void move_partner_next_to_anchor(int target_entity, Point anchor_pos, int layer)
{
    // === 1. 初期設定: パートナーの現在地と目的地を特定 ===
    Point partner_pos;
    Point p1 = {pair_coordinates[target_entity][0], pair_coordinates[target_entity][1]};
    Point p2 = {pair_coordinates[target_entity][2], pair_coordinates[target_entity][3]};

    // anchor_posにいるのがp1かp2かを判定し、じゃない方をパートナーとする
    if (p1 == anchor_pos) {
        partner_pos = p2;
    } else {
        partner_pos = p1;
    }

    // 目的地はアンカーの右隣
    Point destination = {anchor_pos.x + 1, anchor_pos.y};

    // すでに目的地にいれば、なにもせず終了
    if (partner_pos == destination) {
        return;
    }

    // === 2. 戦略決定: パートナーの相対位置に応じて最適な回転を判断 ===
    int dx = partner_pos.x - destination.x;
    int dy = partner_pos.y - destination.y;

    // Case 1: パートナー(P)が目的地の右下、または左下にいる場合 (斜め移動)
    // 目的地(D)に近づけるように、左上または右上に移動させる
    // D . .     . . D
    // . . .  or . . .
    // . P .     . P .
    if (dy > 0) {
        if (dx > 0) { // 右下にいる ->左上へ
            int size = min(dx, dy) + 1;
            apply_rotation(partner_pos.x - size + 1, partner_pos.y - size + 1, size);
        } else { // 左下にいる -> 右上へ
            int size = min(-dx, dy) + 1;
            apply_rotation(partner_pos.x, partner_pos.y - size + 1, size);
        }
        return;
    }
    // Case 2: パートナーが目的地の真横に到達した場合 (垂直移動)
    // パートナーを上に移動させて目的地に揃える
    // P
    // |
    // D
    else if (dx == 0 && dy < 0) {
        // 端にいる場合の特殊処理
        if (partner_pos.x + 1 >= field_size - layer){
            apply_rotation(partner_pos.x - 1, partner_pos.y - 1, 2);
            return;
        }
        int size = -dy + 1;
        size = min(size, field_size - layer - partner_pos.x);
        apply_rotation(partner_pos.x, partner_pos.y, size);
        return;
    }
    // Case 3: パートナーが目的地の真上、または1段上に到達した場合 (水平移動)
    // 目的地に近づけるように、横に移動させる
    // P -> D  または  D <- P
    else if (dy == -1 && dx < 0) { // 1段上で左側にいる -> 右へ
        int size = -dx + 1;
        size = min(size, field_size - partner_pos.y);
        apply_rotation(partner_pos.x, partner_pos.y, size);
        return;
    }
    else if (dy == -1 && dx > 0) { // 1段上で右側にいる -> 左へ（逆回転を利用）
        int size = dx + 1;
        size = min(size, field_size - partner_pos.y);
        // 3回時計回り = 1回反時計回り
        undo_rotation(partner_pos.x - size + 1, partner_pos.y, size);
        return;
    }
    // Case 4: 特殊な位置関係の処理
    // A D
    //   P
    else if (partner_pos.x == anchor_pos.x && partner_pos.y == anchor_pos.y + 1) {
        apply_rotation(anchor_pos.x, anchor_pos.y, 2);
        return;
    }
}

/**
 * @brief 指定された行の、指定された範囲のエンティティがペアになるまで揃える
 * @param y 対象の行番号
 * @param x_start 処理を開始するx座標
 * @param x_end 処理を終了するx座標 (この値は含まない)
 * @param layer 現在処理中の層の深さ (move_pair1に渡すため)
 */
void align_pairs_in_row(int y, int x_start, int x_end, int layer)
{
    for (int x = x_start; x < x_end; x += 2)
    {
        // field[y][x] とその右隣 (field[y][x+1]) がペアになるまで操作を繰り返す
        while (field[y][x] != field[y][x + 1])
        {
            // field[y][x] にあるエンティティのペアを (x+1, y) に移動させる
            Point current_anchor_pos = {x, y};
            move_partner_next_to_anchor(field[y][x], current_anchor_pos, layer);
            
            // デバッグ用に盤面表示が必要な場合は、以下のコメントを解除してください
            // print_field();
        }
    }
}

/**
 * @brief フィールドの外側の層から内側に向かって、辺ごとにペアを揃えていくアルゴリズム
 */
void solve1()
{
    // layer: 現在処理している層のオフセット (0, 2, 4, ...)
    // 外側から2層ずつ内側に入っていく
    for (int layer = 0; layer < field_size / 2; layer += 2)
    {
        // 4つの辺 (上、左、下、右) を順番に処理する
        for (int side = 0; side < 4; ++side)
        {
            // 処理対象は、常に盤面の一番上にある2行 (y = layer, layer+1)
            // 辺を処理するごとに盤面全体を90度回転させることで、
            // 結果的に4辺すべてを同じロジックで処理できる
            int start_x = layer;
            int end_x = field_size - layer;

            align_pairs_in_row(layer,     start_x, end_x, layer);
            align_pairs_in_row(layer + 1, start_x, end_x, layer);

            // 次の辺を処理するために、盤面全体を90度時計回りに回転させる
            // 4回目の回転で、次のlayerの処理開始前に盤面の向きが元に戻る
            apply_rotation(0, 0, field_size);
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
#include <functional> // std::function を使うために必要

// 評価関数を引数に取り、最善手を見つけて適用する汎用関数
// Evaluatorは、現在の盤面の評価値を返す引数なしの関数オブジェクト(ラムダ式など)
void find_and_apply_best_move(const function<int()>& evaluator)
{
    int best_value = evaluator(); // 現在の状態の評価値
    array<int, 3> best_op = {-1, -1, -1}; // 最善手 (x, y, n) を保存

    for (int y = 0; y < field_size - 1; y++)
    {
        for (int x = 0; x < field_size - 1; x++)
        {
            // 回転サイズnのループ条件を効率化
            for (int n = 2; x + n <= field_size && y + n <= field_size; n++)
            {
                // 1. 操作を試す
                rotate_for_simulation(x, y, n);

                // 2. ラムダ式で渡された評価基準で評価する
                int current_value = evaluator();
                if (current_value > best_value)
                {
                    best_value = current_value;
                    best_op = {x, y, n};
                }

                // 3. 操作を元に戻す
                undo_rotation(x, y, n);
            }
        }
    }

    // 最善手が見つかった場合、それを盤面に適用する
    if (best_op[0] != -1)
    {
        apply_rotation(best_op[0], best_op[1], best_op[2]);
    }
    // 注: 改善する手がない場合は何もしない
}

void solve2(int max_time)
{
    for (int i = 0; i < max_time; i++)
    {
        // 評価基準として「ペアの数」を返すラムダ式を渡す
        find_and_apply_best_move([]() {
            return count_pair();
        });

        cout << "手数 " << i + 1 << ": ペアの数：" << count_pair() << endl;
        if (count_pair() == max_number) return;
    }
}


void solve3(int max_time)
{
    for (int i = 0; i < max_time; i++)
    {
        // 評価基準として「重み付けした評価値」を返すラムダ式を渡す
        find_and_apply_best_move([]() {
            const int w1 = 2; // ペア数の重み
            const int w2 = 1; // 距離の重み
            int avg_dist = evaluate_manhattan_distanece_average();

            // ゼロ除算を回避
            if (avg_dist == 0) return w1 * count_pair();
            
            return w1 * count_pair() + w2 * 10 / avg_dist;
        });

        cout << "手数 " << i + 1 << ": ペアの数：" << count_pair() << endl;
        if (count_pair() == max_number) return;
    }
}

int main()
{
    load_problem();

    solve1();    


    export_answer();
}
