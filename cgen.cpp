#include "cgen.h"
#include "code.h"
#include "symtab.h"

// * 全局变量, 假设有无数多个寄存器, 初始化变量时就保存到寄存器上了. 
// * L的标号也是递增的, 从 L1 开始
int rs = 0;
int ls = 0;

string getL() {
    ls++;
    return "L" + to_string(ls);
}

string getR() {
    rs++;
    return "r" + to_string(rs);
}

string cGen(TreeNode* root) {
    if (root == NULL) return "";
    emitComment(root->nodekind);
    TreeNode* p1, *p2, *p3;
    if ("FunDecl" == root->nodekind) {
        p1 = root->child[2];
        p2 = root->child[3];
        // TODO: 添加函数参数的中间代码
        cGen(p2);
        cGen(root->sibling);
    } else if ("Compound" == root->nodekind) {
        p1 = root->child[0];
        p2 = root->child[1];
        cGen(p1);
        cGen(p2);
    } else if ("LocVarDecl" == root->nodekind) {
        // cGen(root->child[1]);
        cGen(root->sibling);
    } else if ("Assign" == root->nodekind) {
        int addr = st_lookup(root->child[0]->name);
        if (addr != -1) {
            string left = getR();
            emit("LD " + left + ", " + to_string(symtabs[root->child[0]->name].addr) + "\n");
            string reg = cGen(root->child[1]);
            // int right_addr = st_lookup(root->child[1]->name); 
            emit("[" + left + "]" + " = " + reg + "\n");
        } else {
            error("no such a id");
        }
        cGen(root->sibling);
    } else if ("Selection" == root->nodekind) {
        string cond = root->child[0]->name;  // 条件
        string l1 = getL(), l2 = getL();
        emit("if " + cGen(root->child[0]));
        emit(" goto ");
        emit(l1 + "\n");
        emit("goto ");
        emit(l2 + "\n");
        emit(l1 + ":\n");
        cGen(root->child[1]);
        emit(l2 + ":\n");
        cGen(root->sibling);  // * 这句一定要加
    } else if ("Iteration" == root->nodekind) {
        string l1 = getL(), l2 = getL();
        emit(l1 + ":\n");
        emit("if " + cGen(root->child[0]));
        emit(" goto ");
        emit(l2 + "\n");
        cGen(root->child[1]);
        emit(l2 + ":\n");
    } else if ("CompareOp" == root->nodekind) {
        // 比较操作符, 作为
        string reg1 = getR(), reg2 = cGen(root->child[0]), reg3 = cGen(root->child[0]);
        emit(reg1 + " = " + reg2 + " " + root->op + " " + reg3 + "\n");
        return reg1;
    } else if ("PMOp" == root->nodekind || "MDOp" == root->nodekind) {
        string reg1 = getR(), reg2 = cGen(root->child[0]), reg3 = cGen(root->child[1]);
        emit(reg1 + " = " + reg2 + " + " + reg3 + "\n");
        return reg1;
    } else if ("Id" == root->nodekind) {
        if (st_lookup(root->name) != -1) {
            string reg = getR();
            emit("LD " + reg + ", [" + to_string(symtabs[root->name].addr) + "]\n");
            return reg;
        } else {
            error("no such a id !!!");
            return "";
        }
    } else if ("Const" == root->nodekind) {
        string reg = getR();
        emit("LD " + reg + " " + to_string(root->val) + "\n");
        return reg;
    }
    return "";
}

void codeGen(TreeNode* root) {
    emitComment("cminus IR generate");
    cGen(root);
    emit("halt");
}