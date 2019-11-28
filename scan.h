#ifndef __SCAN_H_
#define __SCAN_H_

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "globals.h"

using namespace std;

// 声明变量
extern vector<string> reversedWords; // 保留字
extern vector<Token> tokens; // 词法分析得到的token
extern bool scanning; // 正在scan读取的状态

// 定义方法
Token getToken(); // 获取下一个token. 和上一个版本不同的是这个版本的这个函数加上了返回值
void readin(); // 从文件读入源代码到 words

#endif // !__SCAN_H_