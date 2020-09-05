/* File: ast_stmt.h
 * ----------------
 * The Stmt class and its subclasses are used to represent
 * statements in the parse tree.  For each statment in the
 * language (for, if, return, etc.) there is a corresponding
 * node class for that construct. 
 */


#ifndef _H_ast_stmt
#define _H_ast_stmt

#include "list.h"
#include "ast.h"

class Decl;
class VarDecl;
class Expr;
class IntConstant; // M
  
class Program : public Node
{
  protected:
     List<Decl*> *decls;
     
  public:
     Program(List<Decl*> *declList);
     void Check();
};

class Stmt : public Node
{
  public:
     Stmt() : Node() {}
     Stmt(yyltype loc) : Node(loc) {}

    // virtual bool IsExpr() { return false; }
    virtual void Check() = 0;
};

class StmtBlock : public Stmt 
{
  protected:
    List<VarDecl*> *decls;
    List<Stmt*> *stmts;
    
  public:
    StmtBlock(List<VarDecl*> *variableDeclarations, List<Stmt*> *statements);
    void Check();
};

  
class ConditionalStmt : public Stmt
{
  protected:
    Expr *test;
    Stmt *body;
  
  public:
    ConditionalStmt(Expr *testExpr, Stmt *body);
    virtual void Check();
};

class LoopStmt : public ConditionalStmt 
{
  public:
    LoopStmt(Expr *testExpr, Stmt *body)
            : ConditionalStmt(testExpr, body) {}
};

class ForStmt : public LoopStmt 
{
  protected:
    Expr *init, *step;
  
  public:
    ForStmt(Expr *init, Expr *test, Expr *step, Stmt *body);
};

class WhileStmt : public LoopStmt 
{
  public:
    WhileStmt(Expr *test, Stmt *body) : LoopStmt(test, body) {}
};

class IfStmt : public ConditionalStmt 
{
  protected:
    Stmt *elseBody;
  
  public:
    IfStmt(Expr *test, Stmt *thenBody, Stmt *elseBody);
    void Check();
};

class BreakStmt : public Stmt 
{
  public:
    BreakStmt(yyltype loc) : Stmt(loc) {}
    void Check();
};

class ReturnStmt : public Stmt  
{
  protected:
    Expr *expr;
  
  public:
    ReturnStmt(yyltype loc, Expr *expr);
    void Check();
};

class PrintStmt : public Stmt
{
  protected:
    List<Expr*> *args;
    
  public:
    PrintStmt(List<Expr*> *arguments);
    void Check();
};

class SwitchStmt : public Stmt{

  protected:
    List<Stmt*> *stmts;
  public:
    SwitchStmt(Expr *test, List<Stmt*> *casestmt, List<Stmt*> *stmtList);
    const char *GetPrintNameForNode() { return "SwitchStmt"; }
    void PrintChildren(int indentLevel);
    void Check();
  
};

class CaseStmt : public Stmt{
  protected:
    List<Stmt*> *stmts;
    IntConstant *n;

  public:
    CaseStmt(IntConstant *num, List<Stmt*> *stmtList);
    const char *GetPrintNameForNode() { return "CaseStmt"; }
    void PrintChildren(int indentLevel);
    void Check();
};

#endif
