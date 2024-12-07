#ifndef LL1PARSER_H
#define LL1PARSER_H

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stack>
using namespace std;

// 数据结构定义
// 产生式结构
struct Production {
    char nonTerminal;                // 非终结符
    std::vector<std::string> productions;   // 对应的产生式
};

// 终结符结构
struct Terminal {
    std::string value;  // 终结符字符串
};

struct Symbol {
    char nonTerminal;       // 非终结符（如果是非终结符）
    std::string terminal;   // 终结符（如果是终结符）
    bool isTerminal;        // 标识符号是否为终结符

    // 初始化函数，根据传入的字符或字符串进行分类
    Symbol(const std::string& s) {
        if (s.length() == 1 && std::isupper(s[0])) {  // 如果是单个大写字母
            nonTerminal = s[0];
            isTerminal = false;
            terminal.clear();  // 确保terminal清空
        } else {  // 否则是终结符
            terminal = s;
            isTerminal = true;
            nonTerminal = '\0';  // 非终结符清空
        }
    }

    // 如果传入的是单个字符（char），将其转换为字符串进行处理
    Symbol(char c) : Symbol(std::string(1, c)) {}
};



// 全局变量
extern std::vector<Production> grammar;    // 存储产生式
extern std::vector<Terminal> terminals;   // 存储终结符
extern std::unordered_map<char, std::unordered_set<std::string>> firstSet;;// 保存每个非终结符的FIRST集
extern std::unordered_map<char, std::unordered_set<std::string>> followSet; // 保存每个非终结符的FOLLOW集
extern std::map<std::pair<char, std::string>, std::string> parseTable; // LL(1)预测分析表
extern std::stack<Symbol> parsingStack;//字符串分析栈

// 函数声明
void readGrammar(const std::string &filename);               // 从文件读取文法
void eliminateLeftRecursion(std::vector<Production> &grammar); // 消除左递归
void extractLeftFactoring(std::vector<Production> &grammar);   // 提取左公因子
bool isTerminal(const std::string &symbol);
//std::unordered_set<std::string> getFirst(char nonTerminal);
void computeFirst();                                         // 计算FIRST集
void computeFollow();                                        // 计算FOLLOW集
void generateParseTable();                                   // 生成预测分析表
//void printParseTable();                                      // 打印预测分析表
void handleParsingError(char nonTerminal, const std::string& inputSymbol);
std::vector<std::string> parseProduction(const std::string& production);
void parse(const std::vector<std::string>& input,
           const std::unordered_map<char, std::unordered_set<std::string>>& followSet,
           const std::map<std::pair<char, std::string>, std::string>& parseTable);
#endif // LL1PARSER_H
