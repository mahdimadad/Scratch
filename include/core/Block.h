#ifndef BLOCK_H
#define BLOCK_H
#include <vector>
#include <string>
using namespace std;
enum BlockType { Move, Turn, Repeat, If , CustomFunctionCall , SetVariable , ChangeVariable , Wait , Forever };
struct Block {
    BlockType type;
    vector<Block *> children;
    vector<int> parameters;
    string text;
    Block(BlockType t) { type = t; }
};
#endif