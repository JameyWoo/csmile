#include "assembly.h"

ofstream assout("Output-assembly.txt", ios::out);
// 存储变量在栈中的位置, 用map
map<string, string> var2stack;

void genFunc(TreeNode*);  // 处理单个函数
void debug(string);       // debug信息输出的函数

void debug(string info) {
    cout << info << endl;
}

void genLocVarDecl(TreeNode* vars) {
    if (vars == NULL) return;
    // ! 需要统计有多少个局部变量, 以分配初始的栈空间
}

void genStmt(TreeNode* stmts) {
}

void genFunc(TreeNode* func) {
    // ! 函数需要分析参数的栈情况, 写入符号表. 因此这个符号表非常重要.
    debug("func name: " + func->child[1]->name);
    debug(func->child[2]->nodekind);  // 函数的参数
    debug(func->child[3]->nodekind);  // 函数的内容
    // 在 func->child[3] 中, 如果有临时变量声明, 那么是它的child[0], 否则child[0]是正式的内容, 所以要判断
    if (func->child[3]->child[0]->nodekind == "LocVarDecl") {
        genLocVarDecl(func->child[3]->child[0]);    // ! 局部变量声明, 更新符号表. 这个时候要保存每个变量在栈中的位置
        genStmt(func->child[3]->child[1]);          // 生成statement
    } else if (func->child[3]->child[0] != NULL) {  // 如果没有变量声明
        genStmt(func->child[3]->child[0]);
    }
    // debug(func->child[3]->child[0]->nodekind);
    debug(func->child[3]->child[1]->nodekind);
}

void genAssembly(TreeNode* root) {
    // 汇编代码的头部
    assout << "\t.section\t.rodata" << endl
           << ".LC0:" << endl
           << "\t"
           << ".string \"%d\\n\"" << endl
           << "\t"
           << ".text" << endl;

    // 函数部分
    TreeNode* func = root;
    while (func != NULL) {
        genFunc(func);
        func = func->sibling;
    }

    assout << "\t.ident	\"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3\"" << endl
           << "\t.section.note.GNU - stack,\"\",@progbits " << endl;

    cout << "the end..." << endl;
}
