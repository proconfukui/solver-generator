// utils.hpp
#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include<iostream>
#include "puzzle.hpp"
using namespace std;

struct Operation
{
    int x = 0;
    int y = 0;
    int n = 0;
    void print() const {
        cout << "x :" << x << ", y :" << y << ", n :" << n << endl;
    }
    bool operator==(const Operation& other) const
    {
        return x == other.x && y == other.y && n == other.n;
    }
    bool operator < (const Operation& other) const
    {
        if(x != other.x) return x < other.x;
        if(y != other.y) return y < other.y;
        return n < other.n; 
    }
};

struct Field
{
    vector<vector<int>> grid;

    // Setの要素にするため比較演算子を定義
    bool operator < (const Field& other) const {
        int field_size = grid.size();
        for(int i = 0; i < field_size-1;i++){
            for(int j = 0; j < field_size-1;j++){
                if(grid[i][j] == other.grid[i][j]) continue;
                return grid[i][j] < other.grid[i][j];
            }
        }
        return false;
    } 
};