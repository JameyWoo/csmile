#ifndef __GLOBALS_H_
#define __GLOBALS_H_
/**
 * 一些通用类型, 通用变量的声明.
 * 这个文件名写作globals.h, 作为全局变量定义. 
 * 比如说TokenType, 之后的语法分析还会用到.
 * 这是模仿tiny编译器的文件结构.
 */

#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <stack>
#include <algorithm>

#define MAX_CHILDREN 4

using namespace std;

struct Token {  // 一个Token数据结构, 包括它的类型, 值和所在行
    string type;
    string value;
    int line;
    void setToken(string t, string v, int l) {
        type  = t;
        value = v;
        line  = l;
    }
};

extern vector<string>
    words;             // 以行为单位的源代码
extern ifstream read;  // 输入流

/**
 * 上面是写词法分析程序定义的
 * 下面是写语法分析时定义的
 */

typedef enum { StmtK,
               ExpK } NodeKind;
typedef enum { IfK,
               RepeatK,
               AssignK,
               ReadK,
               WriteK } StmtKind;
typedef enum { OpK,
               ConstK,
               IdK } ExpKind;

struct TreeNode {
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling;  // 兄弟
    int child_cnt = 0;
    string nodekind;
    int line;
    string kind;
    string type;

    // tiny 这里是union类型
    string op;
    int val;
    string name;
    TreeNode(string kind) : nodekind(kind) {
        sibling = NULL;
        for (int i = 0; i < MAX_CHILDREN; ++i) {
            child[i] = NULL;
        }
    }
    TreeNode() {}
};

// 输出
// parse解析的语法树
extern ofstream out;
// 符号表
extern ofstream symtabOut;
// 中间代码
extern ofstream midCodeOut;

#endif  // !__GLOBALS_H_