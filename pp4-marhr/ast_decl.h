/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"

class Type;
class NamedType;
class Identifier;
class Stmt;
class FnDecl;
class InterfaceDecl;

class Decl : public Node 
{
  // protected:
  //   Identifier *id;
  
  public:
    Identifier *id;
    Decl(Identifier *name);
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
    Identifier *GetId() { return id; }
    const char *GetName() { return id->GetName(); }
    
    virtual bool ConflictsWithPrevious(Decl *prev);

    virtual bool IsVarDecl() { return false; } // jdz: could use typeid/dynamic_cast for these
    virtual bool IsClassDecl() { return false; }
    virtual bool IsInterfaceDecl() { return false; }
    virtual bool IsFnDecl() { return false; } 
    virtual bool IsMethodDecl() { return false; }
    virtual void Check() = 0;
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    VarDecl(Identifier *name, Type *type);
    void Check();
    Type *GetDeclaredType() { return type; }
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    // NamedType *extends;  
    List<NamedType*> *implements;
    Type *cType;
    List<InterfaceDecl*> *convImp;

  public:
    NamedType *extends;
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
    void Check();
    // bool IsClassDecl() { return true; }
    Scope *PrepareScope();
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    Identifier *id;
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    void Check();
    // bool IsInterfaceDecl() { return true; }
    Scope *PrepareScope();
};

class FnDecl : public Decl 
{
  protected:
    // List<VarDecl*> *formals;
    // Type *returnType;
    Stmt *body;
    
  public:
    List<VarDecl*> *formals;
    Type *returnType;
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    void Check();
    // bool IsFnDecl() { return true; }
    bool IsMethodDecl();
    bool ConflictsWithPrevious(Decl *prev);
    bool MatchesPrototype(FnDecl *other);
};

#endif
