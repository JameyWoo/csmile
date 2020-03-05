#include "symtab.h"

vector<map<string, SymtabOne>> vec_symtabs;
map<string, SymtabOne> symtabs;

// struct SymtabOne {
//     vector<int> line;
//     int location;
//     int rank;
//     int addr;
//     SymtabOne(){}
//     SymtabOne(int lin, int loc): location(loc) {
//         line.push_back(lin);
//     }
//     SymtabOne(int lin, int loc, int rank): location(loc), rank(rank) {
//         line.push_back(lin);
//     }
//     SymtabOne(int loc): location(loc) { }
// };
// vector<map<string, SymtabOne>> vec_symtabs;
// map<string, SymtabOne> symtabs;
int address = 0;

// 一个新的函数建立一个新的符号表.
void newSymtabs() {
    // 只有当符号表不为空地时候建立
    if (symtabs.size() != 0) {
        vec_symtabs.push_back(symtabs);
    }
    symtabs.clear();
}

int st_lookup(string name) {
    if (symtabs.count(name) == 0) {
        return -1;
    } else {
        return symtabs[name].addr;
    }
}

// 新插入一个变量时, 用 rank 记录它是局部变量(2)还是作为参数(1)或者是已声明变量(3)
void st_insert(string name, int lineno, int loc, int rank) {
    if (count(fun_names.begin(), fun_names.end(), name) != 0) return;
    if (rank == 2 || rank == 3) {
        // symtabOut << "insert: " << name << endl;
        if (loc == -1) {  // 当这个变量存在在符号表中时
            symtabs[name].line.push_back(lineno);
        } else if (lineno != -1) {  // 不在符号表
            SymtabOne s(lineno, loc, rank);
            s.addr = ++address;
            symtabs[name] = s;
        } else if (lineno != -1) {  // 不在符号表, 只建立key, 不在vector中插入
            SymtabOne s(loc);
            s.addr = ++address;
            symtabs[name] = s;
        }
    } else if (rank == 1) {
        // symtabOut << "insert: " << name << endl;
        if (loc == -1) {  // 当这个变量存在在符号表中时
            symtabs[name].line.push_back(lineno);
        } else {  // 不在符号表
            SymtabOne s(lineno, loc, rank);
            s.addr = ++address;
            symtabs[name] = s;
        }
    }
}

void printSymtab() {
    vec_symtabs.push_back(symtabs);
    symtabOut << "-------------------" << endl;
    for (auto symtabs: vec_symtabs) {
        symtabOut << "function: " << fun_names.front() << endl;
        fun_names.erase(fun_names.begin());
        symtabOut << "id -- Param --- lines\n";
        for (auto sone: symtabs) {
            symtabOut.setf(ios::left,ios::adjustfield);
            symtabOut << setw(5) << sone.first << ": " 
            << setw(3) 
            << (sone.second.rank == 3 ? "no" : "yes")
            << " | ";
            for (auto v: sone.second.line) {
                symtabOut << setw(3) << v << ' ';
            }
            symtabOut << endl;
        }
        symtabOut << "-------------------" << endl;
    }
}

void error(string errMsg) {
    symtabOut << errMsg << endl;
}