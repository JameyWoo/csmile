#include "utils.h"

// 打印的信息是否打印所有的类别出来还是只打印某些类别

void printTokens(vector<Token> tokens) {
    /**
     * 打印token的通用函数
     */
    int last_line = -1;
    for (auto token : tokens) {
        if (last_line != token.line) {                                    // 先把源码输出. 由于注释源码可能有多行才一个token
            for (int line = last_line + 1; line <= token.line; ++line) {  // 为多行注释特别设计的多行打印
                cout << line << ": ";
                size_t s = 0;
                while (words[line][s++] == ' ')
                    ;  // 输出做一些修改, 把源码前面的空格忽略掉了
                for (size_t i = s - 1; i < words[line].size(); ++i) {
                    cout << words[line][i];
                }
                cout << endl;
            }
            last_line = token.line;
        }

// 可选的打印信息类别
#ifdef ALL_TYPE
        cout << '\t';
        cout << token.line << ": " << token.type << ", value = \"" << token.value << "\"" << endl;
#else
        if (token.type == "ReversedWord" || token.type == "Id" || token.type == "Num" \
        || token.type == "Comment" || token.type == "ErrorToken") {
            cout << '\t';
            cout << token.line << ": " << token.type << ", value = \"" << token.value << "\"" << endl;
        } else {
            cout << '\t' << token.line << ": " << token.value << endl;
        }
#endif  // ALL_TYPE
    }
}