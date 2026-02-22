#ifndef BLOCK_H
#define BLOCK_H
#include <vector>
#include <string>
using namespace std;
enum class BlockType { Move, Turn, Repeat, If, CustomFunctionCall };
struct Block {
    BlockType type;
    vector<Block *> children;
    vector<int> parameters;
    Block(BlockType t) { type = t; }
};
#endif