#include <iostream>
#include <windows.h>
#include "LL1Parser.h"
#include "print.h"
#include "read.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    readGrammar("input.txt");
    eliminateLeftRecursion(grammar);
    extractLeftFactoring(grammar);
    // 打印当前的终结符
    std::cout << "当前终结符集合: ";
    for (const auto &terminal: terminals) {
        std::cout << terminal.value << " ";
    }
    std::cout << std::endl;
    computeFirst();
    computeFollow();
    generateParseTable();

    // 打印FIRST和FOLLOW集
    printFirstFollowTable();
    printParseTable();

    // 提示用户输入需要分析的字符串
    std::vector<std::string> inputStrings = readInput();
    // 调用parse函数进行分析
    parse(inputStrings, followSet, parseTable);
    //system("pause");
    return 0;
}
