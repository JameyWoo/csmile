#include "cgen.h"
#include "code.h"
#include "symtab.h"

void cGen(TreeNode* root) {
    if (root == NULL) return;
    emitComment(root->nodekind);
    TreeNode* p1, *p2, *p3;
    if ("FunDecl" == root->nodekind) {
        p1 = root->child[2];
        p2 = root->child[3];
        // TODO: 添加函数参数的中间代码
        cGen(p2);
    } else if ("Compound" == root->nodekind) {
        p1 = root->child[0];
        p2 = root->child[1];
        cGen(p1);
        cGen(p2);
    } else if ("LocVarDecl" == root->nodekind) {
        cGen(root->child[1]);
        cGen(root->sibling);
    } else if ("Assign" == root->nodekind) {
        int addr = st_lookup(root->child[0]->name);
        if (addr != -1) {
            string left = to_string(addr);
            int right_addr = st_lookup(root->child[1]->name);
            if (right_addr != -1) {
                emit("r" + left + " = " + "r" + to_string(right_addr) + "\n");
            } else {
                error("no such a id");
            }
        } else {
            error("no such a id");
        }   
    } else if 
    
    // * id 是不单独拿出来的
    //  else if ("Id" == root->nodekind) {
    //     int addr = st_lookup(root->name);
    //     if (addr != -1) {
    //         // * 如果符号表找到了名字,返回地址, 前面代码要改动
    //         emit(to_string(addr));  // 
    //     }
    // }
}

void codeGen(TreeNode* root) {
    emitComment("cminus IR generate");
    cGen(root);
}