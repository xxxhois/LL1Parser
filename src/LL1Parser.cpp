#include "LL1Parser.h"

// 定义全局变量
std::vector<Production> grammar;
std::vector<Terminal> terminals;
std::unordered_map<char, std::unordered_set<std::string>> firstSet;
std::unordered_map<char, std::unordered_set<std::string>> followSet;
std::map<std::pair<char, std::string>, std::string> parseTable;


void readGrammar(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件：" << filename << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        char nonTerminal = line[0];            // 提取非终结符
        std::string rhs = line.substr(3);      // 提取右部（跳过 "->"）
        std::stringstream ss(rhs);
        std::string token;

        Production prod = {nonTerminal};

        // 逐个分割右部产生式
        while (std::getline(ss, token, '|')) {
            prod.productions.push_back(token); // 保存该产生式

            // 提取终结符
            std::string currentTerminal;
            for (size_t i = 0; i < token.size(); ++i) {
                char ch = token[i];
                if (islower(ch) || (!isalpha(ch) && !isspace(ch))) {
                    // 当前字符属于终结符，追加到currentTerminal
                    currentTerminal += ch;
                } else if (!currentTerminal.empty()) {
                    // 遇到非终结符或分隔符，将当前终结符保存
                    auto it = std::find_if(terminals.begin(), terminals.end(),
                                           [&currentTerminal](const Terminal &t) { return t.value == currentTerminal; });
                    if (it == terminals.end()) {
                        terminals.push_back({currentTerminal});
                    }
                    currentTerminal.clear(); // 清空currentTerminal
                }

                // 遇到分隔符或末尾，检查是否有剩余的终结符
                if (isupper(ch) || i == token.size() - 1) {
                    if (!currentTerminal.empty()) {
                        auto it = std::find_if(terminals.begin(), terminals.end(),
                                               [&currentTerminal](const Terminal &t) { return t.value == currentTerminal; });
                        if (it == terminals.end()) {
                            terminals.push_back({currentTerminal});
                        }
                        currentTerminal.clear(); // 清空currentTerminal
                    }
                }
            }
        }
        grammar.push_back(prod); // 保存产生式到文法
    }
    file.close();
}



//消除左递归和提取左公因子的函数实现
void eliminateLeftRecursion(vector<Production> &grammar) {
    cout << "开始消除左递归...\n";
    vector<Production> newGrammar;
    bool hasLeftRecursion = false;

    for (auto &prod : grammar) {
        char A = prod.nonTerminal;
        vector<string> alpha, beta;

        cout << "检查非终结符: " << A << "\n";
        for (const auto &rule : prod.productions) {
            if (rule[0] == A) {
                alpha.push_back(rule.substr(1));
                hasLeftRecursion = true;
            } else {
                beta.push_back(rule);
            }
        }

        if (!alpha.empty()) {
            char newNonTerminal = A + 2; // 新增非终结符
            cout << "发现左递归，新增非终结符: " << newNonTerminal << "\n";

            //terminals.push_back({"ε"});//!!!!!!!!
            Production newProd = {newNonTerminal};
            for (const auto &a : alpha) {
                newProd.productions.push_back(a + newNonTerminal);
            }
            newProd.productions.push_back("ε"); // 空产生式
            newGrammar.push_back(newProd);

            prod.productions.clear();
            for (const auto &b : beta) {
                prod.productions.push_back(b + newNonTerminal);
            }
        } else {
            cout << "无左递归: " << A << "\n";
        }

        newGrammar.push_back(prod);
    }

    if (!hasLeftRecursion) {
        cout << "整个文法无左递归。\n";
    }
    else {
        // 将 "ε" 添加到 terminals 向量中
        auto it = std::find_if(terminals.begin(), terminals.end(),
                               [](const Terminal &t) { return t.value == "ε"; });
        if (it == terminals.end()) {
            terminals.push_back({"ε"});
            //terminals.push_back({"$"});
        }
    }
    grammar = newGrammar;

    cout << "左递归消除后的文法:\n";
    for (const auto &prod : grammar) {
        cout << prod.nonTerminal << " -> ";
        for (const auto &rule : prod.productions) {
            cout << rule << " | ";
        }
        cout << "\n";
    }
}

void extractLeftFactoring(vector<Production> &grammar) {
    cout << "开始提取左公因子...\n";
    vector<Production> newGrammar;
    bool hasLeftFactoring = false;

    for (auto &prod : grammar) {
        cout << "检查非终结符: " << prod.nonTerminal << "\n";
        map<string, vector<string>> prefixMap;

        for (const auto &rule : prod.productions) {
            string prefix = rule.substr(0, 1); // 只检查一个字符前缀
            prefixMap[prefix].push_back(rule);
        }

        prod.productions.clear();
        for (auto &[prefix, rules] : prefixMap) {
            if (rules.size() > 1) {
                hasLeftFactoring = true;
                char newNonTerminal = prod.nonTerminal + 2;
                cout << "发现左公因子 \"" << prefix << "\"，新增非终结符: " << newNonTerminal << "\n";

                Production newProd = {newNonTerminal};
                for (const auto &rule : rules) {
                    newProd.productions.push_back(rule.substr(prefix.length()));
                }
                newProd.productions.push_back("ε");
                newGrammar.push_back(newProd);
                prod.productions.push_back(prefix + newNonTerminal);
            } else {
                prod.productions.push_back(rules[0]);
            }
        }

        newGrammar.push_back(prod);
    }

    if (!hasLeftFactoring) {
        cout << "整个文法无左公因子。\n";
    }
    grammar = newGrammar;

    cout << "左公因子提取后的文法:\n";
    for (const auto &prod : grammar) {
        cout << prod.nonTerminal << " -> ";
        for (const auto &rule : prod.productions) {
            cout << rule << " | ";
        }
        cout << "\n";
    }
}

//计算FIRST集和FOLLOW集
// 判断是否为终结符
bool isTerminal(const std::string &symbol) {
    for (const auto &terminal : terminals) {
        if (terminal.value == symbol) {
            return true;
        }
    }
    return false;
}

// 获取某个非终结符的FIRST集
std::unordered_set<std::string> getFirst(char nonTerminal) {
    // 如果已经计算过，直接返回
    if (firstSet.count(nonTerminal)) {
        return firstSet[nonTerminal];
    }

    std::unordered_set<std::string> result;

    // 找到非终结符的产生式
    for (const auto &prod : grammar) {
        if (prod.nonTerminal != nonTerminal) continue;

        // 遍历每个产生式右部
        for (const auto &production : prod.productions) {
            bool canDeriveEmpty = true; // 标记是否可以推导出空串

            for (size_t i = 0; i < production.size(); ) { // 注意：没有 ++i
                std::string symbol;

                // 判断当前符号
                if (isupper(production[i])) { // 非终结符，大写字母
                    symbol = std::string(1, production[i]); // 取单个字符
                    ++i; // 移动到下一个字符
                } else { // 终结符，小写字母或其他多字符符号
                    size_t start = i;
                    while (i < production.size() && !isupper(production[i])) {
                        ++i; // 继续读取直到遇到大写字母或字符串结束
                    }
                    symbol = production.substr(start, i - start);
                }

                if (isTerminal(symbol)) {
                    // 如果是终结符，直接加入FIRST集
                    result.insert(symbol);
                    canDeriveEmpty = false;
                    break;
                } else {
                    // 如果是非终结符，递归计算其FIRST集
                    auto subFirst = getFirst(symbol[0]); // 注意：symbol[0] 是非终结符的字符
                    for (const auto &item : subFirst) {
                        if (item != "ε") { // 非空串加入结果
                            result.insert(item);
                        }
                    }
                    // 检查是否有空串
                    if (subFirst.find("ε") == subFirst.end()) {
                        canDeriveEmpty = false;
                        break;
                    }
                }
            }

            // 如果所有符号都可以推导出空串，则加入"ε"
            if (canDeriveEmpty) {
                result.insert("ε");
            }
        }
    }

    firstSet[nonTerminal] = result;

    return result;
}

// 初始化所有非终结符的FIRST集
void computeFirst() {
    for (const auto &prod : grammar) {
        getFirst(prod.nonTerminal);
    }
}




// 获取某个非终结符的 FOLLOW 集
std::unordered_set<std::string> getFollow(char nonTerminal) {
    static std::unordered_set<char> inProgress; // 处理中标记

    // 如果已经完全计算过，直接返回
    if (followSet.count(nonTerminal)) {
        return followSet[nonTerminal];
    }

    // 如果当前非终结符正在处理中，避免死循环
    if (inProgress.count(nonTerminal)) {
        return {}; // 返回空集，防止死循环
    }

    // 标记当前非终结符正在处理中
    inProgress.insert(nonTerminal);

    std::unordered_set<std::string> result;

    // 如果是起始符，加入 "$"
    if (nonTerminal == 'E') {
        result.insert("$");
    }

    // 遍历文法规则
    for (const auto &prod : grammar) {
        for (const auto &production : prod.productions) {
            for (size_t i = 0; i < production.size();) {
                // 动态解析当前符号（代码保持原样）
                std::string symbol;
                if (isupper(production[i])) {
                    symbol = std::string(1, production[i]);
                    ++i;
                } else {
                    size_t start = i;
                    while (i < production.size() && !isupper(production[i])) {
                        ++i;
                    }
                    symbol = production.substr(start, i - start);
                }

                // 如果当前符号不是目标非终结符，跳过
                if (symbol[0] != nonTerminal) {
                    continue;
                }

                bool addFollowOfLeft = true;

                // 查看后面是否还有符号
                while (i < production.size()) {
                    std::string nextSymbol;
                    if (isupper(production[i])) {
                        nextSymbol = std::string(1, production[i]);
                        ++i;
                    } else {
                        size_t start = i;
                        while (i < production.size() && !isupper(production[i])) {
                            ++i;
                        }
                        nextSymbol = production.substr(start, i - start);
                    }

                    if (isTerminal(nextSymbol)) {
                        result.insert(nextSymbol);
                        addFollowOfLeft = false;
                        break;
                    } else {
                        auto nextFirst = getFirst(nextSymbol[0]);
                        for (const auto &item : nextFirst) {
                            if (item != "ε") {
                                result.insert(item);
                            }
                        }
                        if (nextFirst.find("ε") == nextFirst.end()) {
                            addFollowOfLeft = false;
                            break;
                        }
                    }
                }

                // 如果后面没有符号或所有符号的 FIRST 集包含空串
                if (addFollowOfLeft) {
                    auto leftFollow = getFollow(prod.nonTerminal);
                    result.insert(leftFollow.begin(), leftFollow.end());
                }
            }
        }
    }

    // 移除处理中标记，记录计算完成
    inProgress.erase(nonTerminal);
    followSet[nonTerminal] = result; // 存储结果

    return result;
}


// 初始化所有非终结符的 FOLLOW 集
void computeFollow() {
    for (const auto &prod : grammar) {
        getFollow(prod.nonTerminal);
    }
}


//生成预测分析表
void generateParseTable() {
    for (const auto &prod : grammar) {
        char A = prod.nonTerminal;

        for (const auto &rule : prod.productions) {
            // 解析产生式右部符号
            std::unordered_set<std::string> currentFirstSet;
            bool containsEpsilon = true; // 空串传播

            for (size_t i = 0; i < rule.size() && containsEpsilon; ++i) {
                std::string symbol;

                if (isupper(rule[i])) { // 非终结符
                    symbol = std::string(1, rule[i]);
                } else { // 终结符：提取连续小写字母或特殊符号
                    size_t j = i;
                    while (j < rule.size() && !isupper(rule[j])) {
                        ++j;
                    }
                    symbol = rule.substr(i, j - i);
                    i = j - 1;
                }

                if (isupper(symbol[0])) { // 非终结符
                    const auto &first = firstSet[symbol[0]];
                    currentFirstSet.insert(first.begin(), first.end());
                    containsEpsilon = first.count("ε") > 0;
                } else { // 终结符
                    currentFirstSet.insert(symbol);
                    containsEpsilon = false; // 终结符不传播空串
                }
            }

            // 添加 FIRST 集到预测分析表
            for (const auto &term : currentFirstSet) {
                if (term != "ε") {
                    parseTable[{A, term}] = rule;
                }
            }

            // 如果空串传播到最后，添加 FOLLOW(A)
            if (containsEpsilon) {
                for (const auto &term : followSet[A]) {
                    parseTable[{A, term}] = "ε";
                }
            }
        }
    }

    // 确保 FOLLOW 集中包含 "$" 的非终结符处理
    for (const auto &entry : followSet) {
        char A = entry.first;
        const auto &follow = entry.second;

        for (const auto &term : follow) {
            if (term == "$") {
                // 如果 FOLLOW(A) 包含 "$"，确保解析表中有对应项
                if (parseTable.find({A, "$"}) == parseTable.end()) {
                    parseTable[{A, "$"}] = "ε";
                }
            }
        }
    }
}




// 格式化输出预测分析表
/*void printParseTable() {
    cout << "LL(1)预测分析表：" << endl;
    for (const auto &[key, rule] : parseTable) {
        cout << key.first << ", " << key.second << " -> " << rule << endl;
    }
}*/
void handleParsingError(char nonTerminal, const std::string& currentSymbol) {
    std::cerr << "Syntax error: unexpected symbol '" << currentSymbol
              << "' when parsing non-terminal '" << nonTerminal << "'." << std::endl;
}

std::stack<Symbol> parsingStack;
std::vector<std::string> parseProduction(const std::string& production) {
    std::vector<std::string> symbols;

    // 输出调试信息，打印产生式右部
    //std::cout << "解析产生式右部: " << production << std::endl;

    size_t i = 0;
    while (i < production.size()) {
        char c = production[i];

        // 如果是非终结符（大写字母），直接添加到符号列表
        if (isupper(c)) {
            std::string nonTerminal(1, c);
            symbols.push_back(nonTerminal);
            //std::cout << "提取非终结符: '" << nonTerminal << "'" << std::endl;
            i++; // 移动到下一个字符
        }
        // 如果是终结符（可能是小写字母、数字或其他符号），需要进一步检查
        else if (islower(c) || isdigit(c) || ispunct(c)) {
            std::string terminal;
            // 终结符可能由多个字符组成（如 "id", "num"）
            while (i < production.size() && (islower(production[i]) || isdigit(production[i]) || ispunct(production[i]))) {
                terminal += production[i];
                i++;
            }
            symbols.push_back(terminal);
            //std::cout << "提取终结符: '" << terminal << "'" << std::endl;
        }
        // 忽略空格或其他无效字符
        else {
            i++;
        }
    }

    // 输出调试信息，显示所有提取的符号
    // std::cout << "分割后的符号序列: ";
    // for (const auto& s : symbols) {
    //     std::cout << "'" << s << "' ";
    // }
    // std::cout << std::endl;

    return symbols;
}
void parse(const std::vector<std::string>& input,
           const std::unordered_map<char, std::unordered_set<std::string>>& followSet,
           const std::map<std::pair<char, std::string>, std::string>& parseTable) {
    // 初始化栈
    parsingStack.push(Symbol('$')); // 栈底符号
    parsingStack.push(Symbol('E')); // E 是文法的起始符号

    size_t currentIndex = 0;

    // 调试函数：打印当前栈状态
    auto printStack = [&]() {
        std::stack<Symbol> tempStack = parsingStack;
        std::vector<std::string> stackContent;
        while (!tempStack.empty()) {
            Symbol s = tempStack.top();
            tempStack.pop();
            stackContent.push_back(s.isTerminal ? s.terminal : std::string(1, s.nonTerminal));
        }
        std::cout << "当前栈状态：";
        for (auto it = stackContent.rbegin(); it != stackContent.rend(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    };

    while (!parsingStack.empty()) {
        std::string currentSymbol = input[currentIndex];
        Symbol top = parsingStack.top();

        // 输出当前状态
        std::cout << "\n当前输入符号：" << currentSymbol << std::endl;
        printStack();

        // // 如果栈顶符号为 '$' 且当前输入符号为 '$'，解析成功
        // if (top.terminal == "$" && currentSymbol == "$") {
        //     std::cout << "解析成功：输入符号和栈顶均为 '$'。" << std::endl;
        //     parsingStack.pop(); // 弹出栈顶符号
        //     break;
        // }

        // 栈顶为终结符
        if (top.isTerminal) {
            // 如果栈顶符号为 '$' 且当前输入符号为 '$'，解析成功
            if (top.terminal == "$" && currentSymbol == "$") {
                std::cout << "解析成功：输入符号和栈顶均为 '$'。" << std::endl;
                parsingStack.pop(); // 弹出栈顶符号
                break;
            }
            if (top.terminal == currentSymbol) {
                std::cout << "匹配终结符：" << top.terminal << std::endl;
                parsingStack.pop();
                currentIndex++; // 匹配终结符，移动到下一个输入符号
            } else {
                std::cerr << "Syntax error: unexpected terminal '" << currentSymbol
                          << "', expected '" << top.terminal << "'." << std::endl;
                currentIndex++; // 跳过输入符号以同步
            }
        } else { // 栈顶为非终结符
            char nonTerminal = top.nonTerminal;
            auto key = std::make_pair(nonTerminal, currentSymbol);

            if (parseTable.find(key) != parseTable.end()) {
                parsingStack.pop(); // 弹出栈顶非终结符
                std::string production = parseTable.at(key); // 获取产生式右部
                std::cout << "匹配非终结符 '" << nonTerminal << "', 使用产生式："
                          << nonTerminal << " -> " << production << std::endl;

                // 将产生式右部符号反向推入栈
                std::vector<std::string> symbols = parseProduction(production);

                // 将分割的符号从右到左压入栈
                for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) {
                    if (*it != "ε") { // 忽略空符号
                        if (isupper((*it)[0])) { // 非终结符
                            parsingStack.push(Symbol((*it)[0]));
                        } else { // 终结符
                            parsingStack.push(Symbol(*it));
                        }
                    }
                }
            }else {
                handleParsingError(nonTerminal, currentSymbol); // 调用错误处理函数

                std::cerr << "当前非终结符 '" << nonTerminal << "' 和输入符号 '" << currentSymbol
                          << "' 未找到对应产生式，进入错误恢复。" << std::endl;

                // 错误恢复：同步 FOLLOW 集
                const auto& syncSet = followSet.at(nonTerminal);
                if (syncSet.find(currentSymbol) != syncSet.end()) {
                    std::cout << "输入符号在 FOLLOW 集内，弹出非终结符 '" << nonTerminal << "'。" << std::endl;
                    parsingStack.pop(); // 弹出非终结符进行恢复
                } else {
                    std::cout << "输入符号不在 FOLLOW 集内，跳过符号 '" << currentSymbol << "' 进行恢复。" << std::endl;
                    currentIndex++; // 跳过输入符号进行继续分析
                }
            }
        }
    }

    // 判断解析结果
    if (parsingStack.empty()) {
        std::cout << "\n解析完成，语法分析成功！" << std::endl;
    } else {
        std::cerr << "\n解析完成，但存在未匹配的符号，语法分析失败。" << std::endl;
    }
}









