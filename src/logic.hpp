#pragma once
#ifndef LOGIC
#define LOGIC
#include<random>
#include<vector>
#include<string.h>
#include<algorithm>

using namespace std;
namespace CARD
{
    
    struct Card
    {
        int flo, num; //flo = flower, num = number
        bool operator<(const Card&)const;
        Card():flo(0), num(0) {}
        Card(int f,int n): flo(f), num(n) {}
    };
    struct out_card
    {
        Card A[25];
        int t, k, sym;
        //1单张，2对子，3三张，4三带一，5三带二，6炸弹，7顺子，8连对，9三顺子，10飞机带单翅膀，11四带二, 12王炸，13飞机带双翅膀
        void identify();
        void get_card(vector<Card> cs);
        out_card():k(0),t(0),sym(0){}
    };
}

#endif