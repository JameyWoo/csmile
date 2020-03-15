#include "assembly.h"

ofstream assout("Output-assembly.txt", ios::out);
// ! 存储变量在栈中的位置, 用map
map<string, map<string, string>> fun2param;
map<string, map<string, string>> fun2var;
int LFB_id = 0, LFE_id = 0;

void genFunc(TreeNode*);  // 处理单个函数
void debug(string);       // debug信息输出的函数

void debug(string info) {
    cout << info << endl;
}

void getParams(TreeNode* params, string func_name) {
    if (params == NULL) return;
    debug("params->child[0]->nodekind : " + params->child[0]->nodekind);
    if (params->child[0]->nodekind == "void") {
        debug("param void");
        return;
    }

    stack<string> param_stack;
    map<string, string> var2stack;
    // 接下来是有参数的
    TreeNode* param = params->child[0];
    while (param != NULL) {
        debug("param: " + param->nodekind);
        debug(param->child[0]->nodekind + " " + param->child[1]->name);
        param_stack.push(param->child[1]->name);
        param = param->sibling;
    }

    // * 给参数分配栈空间
    int bias = 8;
    while (not param_stack.empty()) {
        var2stack[param_stack.top()] = to_string(bias) + "(%ebp)";
        param_stack.pop();
        bias += 4;
    }

    fun2param[func_name] = var2stack;
}

void genLocVarDecl(TreeNode* vars, string func_name) {
    if (vars == NULL) return;
    debug(vars->child[0]->name);

    // ! 需要统计有多少个局部变量, 以分配初始的栈空间
    stack<string> var_stack;
    map<string, string> var2stack;
    int var_cnt = 0;
    TreeNode* var = vars;
    while (var != NULL) {
        var_cnt += 1;
        debug("var " + var->child[1]->name);
        var_stack.push(var->child[1]->name);        
        var = var->sibling;
    }
    debug("\tvar cnt: " + to_string(var_cnt));
    
    // * 给局部变量分配栈空间
    int bias = -4;
    while (not var_stack.empty()) {
        var2stack[var_stack.top()] = to_string(bias) + "(%ebp)";
        var_stack.pop();
        bias -= 4;
    }

    fun2var[func_name] = var2stack;
}

void genStmt(TreeNode* stmts) {
    if (stmts == NULL) return;
    
}

void genFunc(TreeNode* func) {
    // ! 函数需要分析参数的栈情况, 写入符号表. 因此这个符号表非常重要.
    debug("func name: " + func->child[1]->name);
    debug(func->child[2]->nodekind);  // 函数的参数
    debug(func->child[3]->nodekind);  // 函数的内容

    assout << "\t.global\t" << func->child[1]->name << endl
           << "\t.type\t" << func->child[1]->name << ", @function" << endl
           << func->child[1]->name << ":" << endl
           << ".LFB" + to_string(LFB_id) << ":" << endl;

    // 对参数生成对应的符号表
    getParams(func->child[2], func->child[1]->name);

    debug(func->child[3]->child[0]->nodekind);
    // 在 func->child[3] 中, 如果有临时变量声明, 那么是它的child[0], 否则child[0]是正式的内容, 所以要判断
    if (func->child[3]->child[0]->nodekind == "LocVarDecl") {
        genLocVarDecl(func->child[3]->child[0], func->child[1]->name);    // ! 局部变量声明, 更新符号表. 这个时候要保存每个变量在栈中的位置
        
        // ! 这个是最麻烦的
        genStmt(func->child[3]->child[1]);          // 生成statement
    } else if (func->child[3]->child[0] != NULL) {  // 如果没有变量声明
        debug("before statement");
        genStmt(func->child[3]->child[0]);
    }
    // debug(func->child[3]->child[0]->nodekind);
    // debug(func->child[3]->child[1]->nodekind);

    assout << ".LFE" + to_string(LFE_id) + ":" << endl
           << "\t.size\t" + func->child[1]->name << ", .-" << func->child[1]->name
           << endl;
}

void genAssembly(TreeNode* root) {
    // 汇编代码的头部
    assout << "\t.section\t.rodata" << endl
           << ".LC0:" << endl
           << "\t"
           << ".string \"%d\\n\"" << endl
           << "\t"
           << ".text" << endl;

    // 函数部分, 遍历每个 function, 单独处理
    TreeNode* func = root;
    while (func != NULL) {
        genFunc(func);
        func = func->sibling;
        LFB_id++;
        LFE_id++;
    }

    assout << "\t.ident	\"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3\"" << endl
           << "\t.section"
           << "\t"
           << ".note.GNU-stack,\"\",@progbits " << endl;

    cout << "the end..." << endl;
}
