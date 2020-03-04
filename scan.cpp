#include "scan.h"

bool scanning = true;
vector<Token> tokens;  // token流
vector<string> tokenstypes({"ReversedWord", "EndFile", "Id", "Num", "Assign",
                            "CompareOp", "CalucateOp", "PMOP", "MDOP", "LeftBracket", "RightBracket",
                            "Semicolon", "Comment", "LeftBigBkt", "RightBigBkt", "Comma",
                            "int", "void", "if", "else", "while", "return", "input", "output"});  // token的类型

vector<string> reversedWords({"if", "else", "while", "void", "int", "return", "input", "output"});  // 保留字

typedef enum {
    // 各种状态类型
    START,
    INASSIGN,
    INNUM,
    INID,
    DONE,
    INCOMMENT,
    INCOMMENT2,
    INCOMMENT3,
    COMPARE1,
    COMPARE2,
    INNEGATIVE,
    INNE,
} StateType;

static string cur_str;       // current string 当前字符串
static size_t position = 0;  // 当前字符位置
vector<string> words;
int word_cnt = -1;

void readin() {  // 读取源文件按行写入到words数组中
    int cnt = 0;
    string str_tmp;
    while (getline(read, str_tmp))  // 一行一行读入
        words.push_back(str_tmp);
    words.push_back("EOF");
}

char getNextChar(int &line) {  // 获取下一个字符
    if (position >= cur_str.size()) {
        cur_str = words[++word_cnt];
        if (cur_str == "EOF") {
            scanning = false;
            return EOF;
        }
        cur_str += ' ';  // 默认加一个空格. 因为如果独立两行都没有空格的话, 会被合并
        position = 0;
    }
    position += 1;
    line = word_cnt;
    return cur_str[position - 1];
}

void rollback() {  // 回滚一个字符
    position -= 1;
}

Token getToken() {        // 获取token, 主函数里循环调用
    string currentToken;  // 当前token
    string token_value;
    int line;
    StateType state = START;
    bool save;  // save 表示是否保存该字符
    while (state != DONE) {
        char c = getNextChar(line);
        save   = true;                                               // 默认不返回
        if ((c == ' ') || (c == '\t') || (c == '\n')) save = false;  // ! 修复<, >后面接空格的bug
        switch (state) {
            case START:
                if (isdigit(c))
                    state = INNUM;
                else if (isalpha(c) || c == '_')
                    state = INID;
                else if ((c == ' ') || (c == '\t') || (c == '\n'))  // 这条还是不能少
                    save = false;
                else if (c == '<') {
                    state = COMPARE1;
                } else if (c == '>') {
                    state = COMPARE2;
                } else if (c == '/') {
                    state = INCOMMENT;
                } else if (c == '-') {
                    state = INNEGATIVE;
                } else if (c == '=') {
                    state = INASSIGN;
                } else if (c == '!') {  // 不等于判断
                    state = INNE;
                } else if (c == EOF) {
                    save         = false;
                    state        = DONE;
                    currentToken = "EndFile";
                } else {
                    state = DONE;  // 下面的都是单个字符出现的, 统一状态DONE
                    switch (c) {
                        case '(':
                            currentToken = "LeftBracket";
                            break;
                        case ')':
                            currentToken = "RightBracket";
                            break;
                        case '{':
                            currentToken = "LeftBigBkt";
                            break;
                        case '}':
                            currentToken = "RightBigBkt";
                            break;
                        case ';':
                            currentToken = "Semicolon";
                            break;
                        case '+':
                            currentToken = "PMOP";
                            break;
                        // - 有些特殊, 他可能是负数的开始
                        case '*':
                            currentToken = "MDOP";
                            break;
                        case ',':
                            currentToken = "Comma";
                            break;
                        default:
                            currentToken = "ErrorToken";
                    }
                }
                break;  // ! 由于没有写break引发的惨案...
            case INNUM:
                if (not isdigit(c)) {
                    rollback();
                    state        = DONE;
                    currentToken = "Num";
                    save         = false;  // rollback了不保存
                }
                break;
            case INID:
                if (not(isalpha(c) or isdigit(c) or c == '_')) {  // 如果不是字母和数字
                    rollback();
                    save         = false;
                    state        = DONE;
                    currentToken = "Id";
                }
                break;
            case INASSIGN:
                if (c == '=') {
                    state        = DONE;
                    currentToken = "CompareOp";
                } else {
                    rollback();
                    save         = false;
                    state        = DONE;
                    currentToken = "Assign";
                }
                break;
            case INCOMMENT:
                if (c == '*') {
                    state = INCOMMENT2;
                } else {
                    save = false;
                    rollback();
                    state        = DONE;
                    currentToken = "MDOP";
                }
                break;
            case INCOMMENT2:
                if (c != '*') {
                    state = INCOMMENT2;
                    save  = true;  // 让注释的空格保存下来
                    if (c == EOF) {
                        state        = DONE;
                        currentToken = "ErrorToken";
                    }
                } else
                    state = INCOMMENT3;
                break;
            case INCOMMENT3:
                if (c == '/') {
                    state        = DONE;
                    currentToken = "Comment";
                } else {
                    state = INCOMMENT2;
                }
                break;
            case COMPARE1:                        // 检测到<之后到这个状态来
                if (not(c == '=' || c == '>')) {  // <= 或者 <> 不等于
                    rollback();
                    save = false;
                }
                state        = DONE;
                currentToken = "CompareOp";
                break;
            case COMPARE2:
                if (c != '=') {
                    rollback();

                    save = false;
                }
                state        = DONE;
                currentToken = "CompareOp";
                break;
            case INNEGATIVE:
                if (isdigit(c)) {
                    state = INNUM;
                } else {
                    rollback();
                    save         = false;
                    state        = DONE;
                    currentToken = "PMOP";
                }
                break;
            case INNE:
                if (c == '=') {  // 不等于
                    state        = DONE;
                    currentToken = "CompareOp";
                } else {
                    currentToken = "ErrorToken";
                    state        = DONE;
                }
                break;
            case DONE:
            default:
                currentToken = "ErrorToken";
        }
        if (save) token_value += c;
    }
    if (count(reversedWords.begin(), reversedWords.end(), token_value) != 0)  // 判断Id是否为保留字
        currentToken = token_value;
    Token token;
    token.setToken(currentToken, token_value, line + 1);  // 行号从1开始
    tokens.push_back(token);                              // 保存token
    if (token.type == "Comment") return getToken();       // 忽略注释
    return token;
}