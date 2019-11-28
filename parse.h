#ifndef __PARSE_H_
#define __PARSE_H_

#include "globals.h"
#include <fstream>
using namespace std;

static int blank = 0;
static Token ptoken;
static ofstream out("output.txt", ios::out);
static int error_cnt = 0;

void error();
void preOrder(TreeNode* t);
Token match(string expected);
TreeNode* param(TreeNode* k);
TreeNode* param_list(TreeNode* k);
TreeNode* params();
TreeNode* local_declaration();
TreeNode* selection_stmt();
TreeNode* iteration_stmt();
TreeNode* var();
TreeNode* args();
TreeNode* call(TreeNode* k);
TreeNode* factor(TreeNode* k);
TreeNode* term(TreeNode* k);
TreeNode* additive_expression(TreeNode* k);
TreeNode* simple_expression(TreeNode* k);
TreeNode* expression();
TreeNode* return_stmt();
TreeNode* expression_stmt();
TreeNode* statement();
TreeNode* statement_list();
TreeNode* compound_stmt();
TreeNode* declaration();
TreeNode* declaration_list();
TreeNode* parse();

#endif  // !__PARSE_H_
