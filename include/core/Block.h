#ifndef BLOCK_H
#define BLOCK_H
#include <vector>
#include <string>
using namespace std;
enum BlockType { Move, Turn , Repeat , If , CustomFunctionCall , SetVariable , ChangeVariable , Wait , Forever , CallFunction , DefineFunction , RepeatUntil , Number , Variable , Add , Sub , Mul , Div , Equal , Greater , Less , And , Or , Not , VariableValue , MessageReceived , Broadcast , BroadcastAndWait , GreenFlagClicked } ;
struct Block {
    BlockType type;
    vector<Block *> children;
    vector<int> parameters;
    std::string text;
    Block(BlockType t) { type = t; }
};
#endif