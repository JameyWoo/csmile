#ifndef __UTILS_H_
#define __UTILS_H_

#include "globals.h"
#include <vector>
#include <iostream>
using namespace std;

// #define ALL_TYPE // 定义是否输出全部的类别, 比如像 * 这种字符是直接输出还是和Id那样给出类别

/**
 * 一些通用功能的实现
 */
void printTokens(vector<Token>); // 打印tokens

#endif // !__UTILS_H_