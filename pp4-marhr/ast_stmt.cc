/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "scope.h"
#include "errors.h"
using namespace std;


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::Check() {
    nodeScope = new Scope();
    decls->DeclareAll(nodeScope);
    decls->CheckAll();
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}
void StmtBlock::Check() {
    // printf("check StmtBlock\n");
    nodeScope = new Scope();
    decls->DeclareAll(nodeScope);
    decls->CheckAll();
    stmts->CheckAll();
    
    // printf("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

void ConditionalStmt::Check() {
    body->Check();
    test->Check();

    if(!test->GetType()->IsEquivalentTo(Type::boolType)){
        ReportError::TestNotBoolean(test);
        return;
    }
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
void IfStmt::Check() {
    ConditionalStmt::Check();
    if (elseBody) elseBody->Check();

    if(!test->GetType()->IsEquivalentTo(Type::boolType)) {
        ReportError::TestNotBoolean(test);
        return;
    }
}

void BreakStmt::Check(){
    if(dynamic_cast<LoopStmt*>(this->GetParent())){
        return;
    }
    ReportError::BreakOutsideLoop(this);
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::Check(){
    expr->Check();
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}

void PrintStmt::Check(){

}

SwitchStmt::SwitchStmt(Expr *test, List<Stmt*> *casestmt, List<Stmt*> *stmtList){
    // test->Print(indentLevel+1, "(test) ");
    // body->Print(indentLevel+1, "(then) ");
    Assert(stmtList != NULL);
    (stmts=stmtList)->SetParentAll(this);
}

void SwitchStmt::Check(){

}

void SwitchStmt::PrintChildren(int indentLevel){
    // stmts->PrintAll(indentLevel+1, "(args) ");
}

CaseStmt::CaseStmt(IntConstant *num, List<Stmt*> *stmtList){
    //Assert(n!= NULL);
    (n=num)->SetParent(this);
    (stmts=stmtList)->SetParentAll(this);
}

void CaseStmt::Check(){

}

void CaseStmt::PrintChildren(int indentLevel){
    // stmts->PrintAll(indentLevel+1, "(case) ");
    stmts->SetParentAll(this);
}

