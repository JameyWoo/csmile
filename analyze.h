
#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "globals.h"
#include "symtab.h"

/**
 * 符号表与类型检查
 */

void buildSymtab(TreeNode *);

void typeCheck(TreeNode *);

#endif