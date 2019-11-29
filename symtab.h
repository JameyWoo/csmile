#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

extern int isError;               // 符号表分析结果
extern int typeError;             // 类型检查结果
extern vector<string> fun_names;  // 保存函数的

struct SymtabOne {
    vector<int> line;
    int location;
    int rank;
    int addr;
    SymtabOne(){}
    SymtabOne(int lin, int loc): location(loc) {
        line.push_back(lin);
    }
    SymtabOne(int lin, int loc, int rank): location(loc), rank(rank) {
        line.push_back(lin);
    }
    SymtabOne(int loc): location(loc) { }
};
extern vector<map<string, SymtabOne>> vec_symtabs;
extern map<string, SymtabOne> symtabs;

void st_insert(string name, int lineno, int loc, int rank);

int st_lookup(string name);

void printSymtab();

void newSymtabs();

void error(string);

#endif