/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "hashtable.h"
#include <stack>
#include <iostream>
using namespace std;

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

// Program::Program(List<Decl*> *d,Hashtable<Decl*> *ht){
//     Assert(d != NULL);
//     (decls=d)->SetParentAll(this);
//     // crea nuevo scope
//     scope = ht;
//     cout << "Inicia ST: Program"<< endl;
// }

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */

    // stack que almacena diferentes tablas
    // una tabla para cada scope nuevo
    stack<shared_ptr<Hashtable<Node*> > > s;

    // scope global
    Hashtable<Node*> scope;
    s.push(make_shared<Hashtable<Node*> >(scope));
    // tree traverse
    decls->Check(s);
    //decls->Check(s.top()); //
    //s.top->Enter(const char *key, Value val, bool overwrite)
    //cout << s.top()->NumEntries() << endl; // que tiene el scope global


}

void Stmt::Check(stack<shared_ptr<Hashtable<Node*> > > s){
    // check what kinda stmt is it
    shared_ptr<Stmt> t = make_shared<Stmt>(*this);
    StmtBlock* b = dynamic_cast<StmtBlock*>(this);
    IfStmt* i = dynamic_cast<IfStmt*>(this);

    if (b){
        b->Check(s);
    }
    else if (i){
        i->Check(s);
    }
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

// StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s,Hashtable<Stmt*> *ht){
//     Assert(d != NULL && s != NULL);
//     (decls=d)->SetParentAll(this);
//     (stmts=s)->SetParentAll(this);    
//     // crea el nuevo scope
//     scope=ht;
//     //stack.push(scope);
//     cout << "Inicia ST: StmtBlock"<< endl;
// }

void StmtBlock::Check(stack<shared_ptr<Hashtable<Node*> > > s){
    Hashtable<Node*> ns; // nuevo scope
    s.push(make_shared<Hashtable<Node*> >(ns)); // agrega scope
    decls->Check(s);
    stmts->Check(s);
    s.pop();
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::Check(stack<shared_ptr<Hashtable<Node*> > > s){
    // Hashtable<Node*> ns; // nuevo scope
    // s.push(make_shared<Hashtable<Node*> >(ns)); // agrega scope
    body->Check(s);
    // s.pop(); // termina scope de then
    if(elseBody) elseBody->Check(s);
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}

SwitchStmt::SwitchStmt(Expr *test, List<Stmt*> *casestmt, List<Stmt*> *stmtList){
    // test->Print(indentLevel+1, "(test) ");
    // body->Print(indentLevel+1, "(then) ");
    Assert(stmtList != NULL);
    (stmts=stmtList)->SetParentAll(this);
}

CaseStmt::CaseStmt(IntConstant *num, List<Stmt*> *stmtList){
    //Assert(n!= NULL);
    (n=num)->SetParent(this);
    (stmts=stmtList)->SetParentAll(this);
}


