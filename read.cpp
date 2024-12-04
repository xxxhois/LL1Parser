//
// Created by 34401 on 24-12-4.
//

#include "read.h"


// 输入处理函数：从标准输入读取
std::vector<std::string> readInput() {
    std::vector<std::string> input;
    std::string line;

    std::cout << "请输入输入符号序列（以空格分隔，输入以'$'结束，例如 'num + num $'）：\n";
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::string symbol;

    while (iss >> symbol) {
        input.push_back(symbol);
    }

    // 确保以 '$' 结束
    if (input.empty() || input.back() != "$") {
        std::cerr << "输入必须以 '$' 结束。" << std::endl;
        exit(1);
    }

    return input;
}
