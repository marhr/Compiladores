/* File: ast_stmt.h
 * ----------------
 * The Stmt class and its subclasses are used to represent
 * statements in the parse tree.  For each statment in the
 * language (for, if, return, etc.) there is a corresponding
 * node class for that construct. 
 *
 * pp3: You will need to extend the Stmt classes to implement
 * semantic analysis for rules pertaining to statements.
 */


#ifndef _H_ast_stmt
#define _H_ast_stmt

#include "list.h"
#include "ast.h"
#include "hashtable.h"

class Decl;
class VarDecl;
class Expr;
class IntConstant; // M
  
class Program : public Node
{
  protected:
    List<Decl*> *decls;
     
  public:

    Hashtable<Decl*> *scope;
    Program(List<Decl*> *declList);
    //Program(List<Decl*> *declList, Hashtable<Decl*> *scope);
    void Check();
};

class Stmt : public Node
{
  public:
     Stmt() : Node() {}
     Stmt(yyltype loc) : Node(loc) {}
    virtual ~Stmt(){};
     void Check(stack<shared_ptr<Hashtable<Node*> > > s);
};

class StmtBlock : public Stmt 
{
  protected:
    List<VarDecl*> *decls;
    List<Stmt*> *stmts;
    
  public:
    Hashtable<Stmt*> *scope;
    StmtBlock(List<VarDecl*> *variableDeclarations, List<Stmt*> *statements);
    //StmtBlock(List<VarDecl*> *variableDeclarations, List<Stmt*> *statements,Hashtable<Stmt*> *scope);
    void Check(stack<shared_ptr<Hashtable<Node*> > > s);
};

  
class ConditionalStmt : public Stmt
{
  protected:
    Expr *test;
    Stmt *body;
  
  public:
    ConditionalStmt(Expr *testExpr, Stmt *body);
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
    void Check(stack<shared_ptr<Hashtable<Node*> > > s);
    IfStmt(Expr *test, Stmt *thenBody, Stmt *elseBody);
};

class BreakStmt : public Stmt 
{
  public:
    BreakStmt(yyltype loc) : Stmt(loc) {}
};

class ReturnStmt : public Stmt  
{
  protected:
    Expr *expr;
  
  public:
    ReturnStmt(yyltype loc, Expr *expr);
};

class PrintStmt : public Stmt
{
  protected:
    List<Expr*> *args;
    
  public:
    PrintStmt(List<Expr*> *arguments);
};


class SwitchStmt : public Stmt{

  protected:
    List<Stmt*> *stmts;
  public:
    SwitchStmt(Expr *test, List<Stmt*> *casestmt, List<Stmt*> *stmtList);
};

class CaseStmt : public Stmt{
  protected:
    List<Stmt*> *stmts;
    IntConstant *n;

  public:
    CaseStmt(IntConstant *num, List<Stmt*> *stmtList);
};


#endif
