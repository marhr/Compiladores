/* File: ast_expr.h
 * ----------------
 * The Expr class and its subclasses are used to represent
 * expressions in the parse tree.  For each expression in the
 * language (add, call, New, etc.) there is a corresponding
 * node class for that construct. 
 */


#ifndef _H_ast_expr
#define _H_ast_expr

#include "ast.h"
#include "ast_stmt.h"
#include "list.h"
#include "ast_type.h"
#include <iostream>
using namespace std;

class NamedType; // for new
class Type; // for NewArray
class Identifier;


class Expr : public Stmt 
{
  public:
    Expr(yyltype loc) : Stmt(loc) {}
    Expr() : Stmt() {}

    virtual Type* GetType() = 0;
    virtual void Check() {};

    // virtual ~Expr(){};
    // virtual bool IsExpr() { return true; }
    // virtual bool IsFieldAccess(){ return false; }
    // virtual bool IsArrayAccess(){ return false; }
    // virtual bool IsIntConstant(){ return false; }
    // virtual bool IsDoubleConstant() { return false; }
    // virtual bool IsBoolConstant(){ return false; }
    // virtual bool IsStringConstant(){ return false; }
    // virtual bool IsNullConstant(){ return false; }

};

/* This node type is used for those places where an expression is optional.
 * We could use a NULL pointer, but then it adds a lot of checking for
 * NULL. By using a valid, but no-op, node, we save that trouble */
class EmptyExpr : public Expr
{
  public:
    Type* GetType(){ return Type::errorType; } //nullType; }  

};

class IntConstant : public Expr 
{
  protected:
    int value;
  
  public:
    IntConstant(yyltype loc, int val);
    // virtual bool IsIntConstant(){ return true;}
    Type* GetType(){ return Type::intType; }
};

class DoubleConstant : public Expr 
{
  protected:
    double value;
    
  public:
    DoubleConstant(yyltype loc, double val);
    // virtual bool IsDoubleConstant(){ return true; }
    Type* GetType(){ return Type::doubleType; }
};

class BoolConstant : public Expr 
{
  protected:
    bool value;
    
  public:
    BoolConstant(yyltype loc, bool val);
    // virtual bool IsBoolConstant(){ return true; }
    Type* GetType(){ return Type::boolType;}
};

class StringConstant : public Expr 
{ 
  protected:
    char *value;
    
  public:
    StringConstant(yyltype loc, const char *val);
    // virtual bool IsStringConstant(){ return true; }
    Type* GetType(){ return Type::stringType; }
};

class NullConstant: public Expr 
{
  public: 
    NullConstant(yyltype loc) : Expr(loc) {}
    // virtual bool IsNullConstant(){ return true; }
    Type* GetType(){return Type::nullType; }
};

class Operator : public Node 
{
  protected:
    char tokenString[4];
    
  public:
    Operator(yyltype loc, const char *tok);
    friend std::ostream& operator<<(std::ostream& out, Operator *o) { return out << o->tokenString; }
    const char *str() { return tokenString; }
 };
 
class CompoundExpr : public Expr
{
  protected:
    Operator *op;
    Expr *left, *right; // left will be NULL if unary
    
  public:
    CompoundExpr(Expr *lhs, Operator *op, Expr *rhs); // for binary
    CompoundExpr(Operator *op, Expr *rhs);             // for unary
    virtual Type* GetType();
    virtual void Check(); 
};

class ArithmeticExpr : public CompoundExpr 
{
  public:
    ArithmeticExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    ArithmeticExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
    void Check();
    Type* GetType();
};

class RelationalExpr : public CompoundExpr 
{
  public:
    RelationalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    void Check();
    Type* GetType();
};

class EqualityExpr : public CompoundExpr 
{
  public:
    EqualityExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "EqualityExpr"; }
    void Check();
    Type* GetType();
};

class LogicalExpr : public CompoundExpr 
{
  public:
    LogicalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    LogicalExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
    const char *GetPrintNameForNode() { return "LogicalExpr"; }
    void Check();
    Type* GetType();
};

class AssignExpr : public CompoundExpr 
{
  public:
    AssignExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "AssignExpr"; }
    void Check();
    Type* GetType();
};

class LValue : public Expr 
{
  public:
    LValue(yyltype loc) : Expr(loc) {}
};

class This : public Expr 
{
  public:
    This(yyltype loc) : Expr(loc) {}
    void Check();
    Type* GetType();
    Decl *GetClass();
};

class ArrayAccess : public LValue 
{
  protected:
    Expr *base, *subscript;
    
  public:
    ArrayAccess(yyltype loc, Expr *base, Expr *subscript);
    Type* GetType();
    void Check();
};

/* Note that field access is used both for qualified names
 * base.field and just field without qualification. We don't
 * know for sure whether there is an implicit "this." in
 * front until later on, so we use one node type for either
 * and sort it out later. */
class FieldAccess : public LValue 
{
  // protected:
    Expr *base;	// will be NULL if no explicit base
    Identifier *field;
    
  public:
    // Expr *base; // will be NULL if no explicit base
    // Identifier *field;
    FieldAccess(Expr *base, Identifier *field); //ok to pass NULL base
    // virtual bool IsFieldAccess(){ return true; }
    void Check();
    Type* GetType();
};

/* Class for postfix expressions */
class Postfix : public LValue{
  protected:
    Operator *op;
    Expr *right;
  public:
    Postfix(Operator *op, Expr *rhs);
    const char *GetPrintNameForNode() {return "Postfix";}
    void PrintChildren(int indentLevel);
    Type* GetType();
};

/* Like field access, call is used both for qualified base.field()
 * and unqualified field().  We won't figure out until later
 * whether we need implicit "this." so we use one node type for either
 * and sort it out later. */
class Call : public Expr 
{
  protected:
    Expr *base;	// will be NULL if no explicit base
    Identifier *field;
    List<Expr*> *actuals;
    
  public:
    Call(yyltype loc, Expr *base, Identifier *field, List<Expr*> *args);
    void Check();
    Type* GetType();
    void ValidateActuals();
};

class NewExpr : public Expr
{
  protected:
    NamedType *cType;
    
  public:
    NewExpr(yyltype loc, NamedType *clsType);
    void Check();
    Type* GetType();
};

class NewArrayExpr : public Expr
{
  protected:
    Expr *size;
    Type *elemType;
    
  public:
    NewArrayExpr(yyltype loc, Expr *sizeExpr, Type *elemType);
    Type* GetType();
    void Check();
};

class ReadIntegerExpr : public Expr
{
  public:
    ReadIntegerExpr(yyltype loc) : Expr(loc) {}
    Type* GetType();
};

class ReadLineExpr : public Expr
{
  public:
    ReadLineExpr(yyltype loc) : Expr (loc) {}
    Type* GetType();
};

    
#endif
