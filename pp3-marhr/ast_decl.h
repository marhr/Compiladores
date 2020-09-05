/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp3: You will need to extend the Decl classes to implement 
 * semantic processing including detection of declaration conflicts 
 * and managing scoping issues.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"
#include "hashtable.h"
#include "errors.h" // M
#include <stack>
#include <memory>
#include <iostream>
using namespace std;

class Type;
class NamedType;
class Identifier;
class Stmt;

class Decl : public Node 
{
  protected:
    Identifier *id;
  
  public:
    Decl(Identifier *name);
    void Check(stack<shared_ptr<Hashtable<Node*> > > s);
    virtual ~Decl(){};
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    Identifier *name;
    
  public:
    VarDecl(Identifier *name, Type *type);
    void Check(stack<shared_ptr<Hashtable<Node*> > > s);
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;
    Identifier *name;

  public:
    Hashtable<Decl*> *scope;
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
    ClassDecl(Identifier *name, NamedType *extends, List<NamedType*> *implements, 
              List<Decl*> *members, Hashtable<Decl*> *scope);
    void Check(stack<shared_ptr<Hashtable<Node*> > > s);
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    Identifier *name;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    void Check(stack<shared_ptr<Hashtable<Node*> > > s);
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    Identifier *name;
    
  public:
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    void Check(stack<shared_ptr<Hashtable<Node*> > > s);
};

#endif
