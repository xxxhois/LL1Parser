//
// Created by 34401 on 24-12-2.
//

#include "print.h"
#include "LL1Parser.h"
void printFirstFollowTable() {
    // 计算列宽
    size_t colWidth = 25; // 增加列宽以支持较长的字符串

    // 打印表头
    std::cout << std::left << std::setw(colWidth) << "非终结符"
              << std::setw(colWidth) << "FIRST 集"
              << std::setw(colWidth) << "FOLLOW 集" << std::endl;

    // 打印分隔符
    std::cout << std::string(colWidth * 3, '-') << std::endl;

    // 遍历每个非终结符
    for (const auto &prod : grammar) {
        char nonTerminal = prod.nonTerminal;

        // 构建 FIRST 集字符串
        std::string firstStr;
        for (const auto &first : firstSet[nonTerminal]) {
            if (!firstStr.empty()) firstStr += ", ";
            firstStr += first; // 直接拼接 string 类型
        }

        // 构建 FOLLOW 集字符串
        std::string followStr;
        for (const auto &follow : followSet[nonTerminal]) {
            if (!followStr.empty()) followStr += ", ";
            followStr += follow; // 直接拼接 string 类型
        }

        // 打印行
        std::cout << std::left << std::setw(colWidth) << nonTerminal
                  << std::setw(colWidth) << firstStr
                  << std::setw(colWidth) << followStr << std::endl;
    }
}

void printParseTable() {
    // 获取所有非终结符和终结符
    std::vector<char> nonTerminals;
    std::unordered_set<std::string> terminals;

    for (const auto &prod : grammar) {
        nonTerminals.push_back(prod.nonTerminal);
    }

    for (const auto &[key, value] : parseTable) {
        terminals.insert(key.second); // key.second 是终结符
    }
    terminals.insert("$"); // 添加结束符

    // 计算列宽
    size_t colWidth = 20;

    // 打印表头
    std::cout << std::left << std::setw(colWidth) << "非终结符";
    for (const auto &terminal : terminals) {
        std::cout << std::setw(colWidth) << terminal;
    }
    std::cout << std::endl;

    // 打印分隔符
    std::cout << std::string(colWidth * (terminals.size() + 1), '-') << std::endl;

    // 打印每个非终结符的行
    for (const auto &nonTerminal : nonTerminals) {
        std::cout << std::left << std::setw(colWidth) << nonTerminal;

        for (const auto &terminal : terminals) {
            auto it = parseTable.find({nonTerminal, terminal});
            if (it != parseTable.end()) {
                std::cout << std::setw(colWidth) << it->second; // 打印产生式
            } else {
                std::cout << std::setw(colWidth) << "-"; // 空白处填充 "-"
            }
        }
        std::cout << std::endl;
    }
}
