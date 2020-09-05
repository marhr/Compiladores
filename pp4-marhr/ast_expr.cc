/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>
#include <iostream>
using namespace std;

#include "errors.h"

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

Type* CompoundExpr::GetType(){
    if(left){
        return left->GetType();
    }
    return right->GetType();
}

void CompoundExpr::Check(){
    if (left){
    // printf("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
        left->Check();
    }
    else
        right->Check();

}

Type* ArithmeticExpr::GetType(){
    if(left && right){
        Type* l = left->GetType();
        Type* r = right->GetType();
        if(l->IsEquivalentTo(Type::intType) && r->IsEquivalentTo(Type::intType))
            return l;
        if(l->IsEquivalentTo(Type::doubleType) && r->IsEquivalentTo(Type::doubleType)){
            return l;
        }
        // return Type::errorType; // SEG FAULT
    }
    Type* r;
    if (right){
        r = right->GetType();
    }
    else
        r = left->GetType();
    if(r->IsEquivalentTo(Type::intType) || r->IsEquivalentTo(Type::doubleType))
        return r;
    return Type::errorType;

}  
void ArithmeticExpr::Check(){
    if (left)
        left->Check();
    if (right){
        right->Check();
    }
    if(left && right){
        Type* l = left->GetType();
        Type* r = right->GetType();
        if(l->IsEquivalentTo(Type::errorType) && r->IsEquivalentTo(Type::errorType))
            return;
        if(l->IsEquivalentTo(Type::intType) && r->IsEquivalentTo(Type::intType))
            return;
        if(l->IsEquivalentTo(Type::doubleType) && r->IsEquivalentTo(Type::doubleType))
            return;
        ReportError::IncompatibleOperands(op, l, r);
        return;
    }
    return;

    
}
Type* RelationalExpr::GetType(){
    Type* l = left->GetType();
    Type* r = right->GetType();
    if(l->IsEquivalentTo(Type::intType) && r->IsEquivalentTo(Type::intType))
        return Type::boolType;
    if(l->IsEquivalentTo(Type::doubleType) && r->IsEquivalentTo(Type::doubleType))
        return Type::boolType;
    return Type::errorType;
}  
void RelationalExpr::Check(){
    left->Check();
    right->Check();Type* l = left->GetType();
    Type* r = right->GetType();
    if(l->IsEquivalentTo(Type::intType) && r->IsEquivalentTo(Type::intType))
        return;
    if(l->IsEquivalentTo(Type::doubleType) && r->IsEquivalentTo(Type::doubleType))
        return;
    ReportError::IncompatibleOperands(op, l, r);
}

Type* EqualityExpr::GetType(){
    Type* l = left->GetType();
    Type* r = right->GetType();
    if (l->IsEquivalentTo(r) || r->IsEquivalentTo(l)){
        return Type::boolType;
    }
    return Type::errorType;
}  
void EqualityExpr::Check(){
    left->Check();
    right->Check();
    Type* l = left->GetType();
    Type* r = right->GetType();

    if(!(l->IsEquivalentTo(r) ||r->IsEquivalentTo(l)))
        ReportError::IncompatibleOperands(op, l, r);
    if(l->IsEquivalentTo(Type::voidType) || r->IsEquivalentTo(Type::voidType))
        ReportError::IncompatibleOperands(op, l, r);
}

Type* LogicalExpr::GetType(){
    Type* r = right->GetType();
    if(left==NULL){
        if (r->IsEquivalentTo(Type::boolType)){
            return Type::boolType;
        }
        return Type::errorType;
    }
    Type* l = left->GetType();
    if (l->IsEquivalentTo(Type::boolType) && r->IsEquivalentTo(Type::boolType)){
        return Type::boolType;
    }
    return Type::errorType;
} 

void LogicalExpr::Check(){
    if (left)
        left->Check();
    right->Check();

    Type* r = right->GetType();
    if (left==NULL){
        if (r->IsEquivalentTo(Type::boolType)){
            return;
        }
        ReportError::IncompatibleOperand(op,r);
        return;
    }
    Type* l = left->GetType();
    if (l->IsEquivalentTo(Type::boolType) && r->IsEquivalentTo(Type::boolType)){
        return;
    }
    ReportError::IncompatibleOperands(op,l,r);
}

Type* AssignExpr::GetType(){
    Type* l = left->GetType();
    Type* r = right->GetType();

    if (l->IsEquivalentTo(r)){
        return l;
    }
    else{
        ReportError::IncompatibleOperands(op,l,r);
        return Type::errorType;
    }
    
}  
void AssignExpr::Check(){
    // printf("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
        left->Check();
        right->Check();
    Type* ae = this->GetType();
    if (!ae->IsEquivalentTo(Type::errorType)){
        return;
    }

    // Type* l = left->GetType();
    // Type* r = right->GetType();
    // if (l->IsEquivalentTo(r)){
    //     return;
    // }
    
}

Decl* This::GetClass(){
    Node *n = this;
    while(n){
        ClassDecl* cd = dynamic_cast<ClassDecl*>(n);
        if (cd){
            return cd;
        }
        n = n->GetParent();
    } 

    return NULL;
}

Type* This::GetType(){
    ClassDecl* cd = dynamic_cast<ClassDecl*>(this->GetClass());
    if (cd){
        // Decl* d = dynamic_cast<Decl*>(cd);
        // return new NamedType(d->id);
        return cd->extends;
    }
    ReportError::ThisOutsideClassScope(this);
    return Type::errorType;
}  
void This::Check(){
    ClassDecl* cd = dynamic_cast<ClassDecl*>(this->GetClass());
    if (cd){
        return;
    }
    ReportError::ThisOutsideClassScope(this);
    
}
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}

Type* ArrayAccess::GetType() {
    ArrayType* at = dynamic_cast<ArrayType*>(base->GetType());
    if (at!= NULL){
        return at->GetElemType();
    }
    return base->GetType();
}

void ArrayAccess::Check() {
    base->Check();
    subscript->Check();
    ArrayType* at = dynamic_cast<ArrayType*>(base->GetType());
    Type* st = subscript->GetType();
    Type* e = Type::errorType;
    if(at == NULL){
        ReportError::BracketsOnNonArray(base);
    }
    if(!st->IsEquivalentTo(Type::intType))
        ReportError::SubscriptNotInteger(subscript);

    return;
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}

void FieldAccess::Check(){
    if (base){
        base->Check();
    }    
    Decl *d = FindDecl(field);
    ClassDecl* cd = dynamic_cast<ClassDecl*>(d);
    NamedType* nt;
    Type* t;
    if (base == NULL){
        if (cd == NULL){
            if (d == NULL){
                ReportError::IdentifierNotDeclared(field, LookingForVariable);
                return;
            }
        }
        else{
            nt = new NamedType(d->GetId());
            d = FindDecl(field);
            if (d == NULL){
                ReportError::FieldNotFoundInBase(field, nt);
                return;
            }
        }
    }
    else{
        t = base->GetType();
        d = FindDecl(field);
        if (d==NULL){
            ReportError::FieldNotFoundInBase(field, t);
            return;
        }
        else if(cd == NULL){
            ReportError::InaccessibleField(field, t);
            return;
        }
    }
    if(dynamic_cast<VarDecl*>(d) == NULL)
        ReportError::InaccessibleField(field,base->GetType());
}

Type* FieldAccess::GetType(){
    Decl *d;
    NamedType *nt;
    if (base==NULL){  // si no hay base
        d = FindDecl(field); //busca declaracion
        if (dynamic_cast<VarDecl*>(d)!=NULL){ // si es una variable
            return dynamic_cast<VarDecl*>(d)->GetDeclaredType(); // el tipo d la decl
        }
        else if(dynamic_cast<ClassDecl*>(d)){ // si es una clase 
            nt = dynamic_cast<ClassDecl*>(d)->extends;
            return nt;
        }
        else if(dynamic_cast<FnDecl*>(d)!=NULL){
            return dynamic_cast<FnDecl*>(d)->returnType;
        }
    }
    else{ // si hay base
        d = FindDecl(field); // buscar decl de field
        // dynamic_cast<VarDecl*>(d)->GetDeclaredType(); // obtener el tipo
        // return base->GetType(); // regresa el tipo de la base
    }
    if(d==NULL || dynamic_cast<VarDecl*>(d)==NULL)
        return Type::errorType;
    return dynamic_cast<VarDecl*>(d)->GetDeclaredType();

    // Decl *d;
    // ClassDecl* cd = GetClassDecl(scope);
    // return Type::intType;
}

  Postfix::Postfix(Operator *o, Expr *ex):LValue(*ex->GetLocation()){
    (op=o)->SetParent(this);
    (right=ex)->SetParent(this);
  }


Type* Postfix::GetType(){
    return Type::errorType;
}  


Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

Type* Call::GetType(){
    Decl* d;
    ClassDecl* cd;
    NamedType* nt;
    Type* t;
    d = FindDecl(field);
    if (base==NULL){
        cd = dynamic_cast<ClassDecl*>(d);
        if (cd==NULL){
            d = FindDecl(field);
        }
        // else{
        //     d = FindDecl(field);
        // }

    }
    else{
        t = base->GetType();
        d = FindDecl(field);
        if(d==NULL && dynamic_cast<ArrayType*>(t)!=NULL && strcmp(field->GetName(), "length")==0)
            return Type::intType;
    }
    if (d==NULL || dynamic_cast<FnDecl*>(d)==NULL){
        return Type::errorType;
    }
    return dynamic_cast<FnDecl*>(d)->returnType;
    
        
}
void Call::ValidateActuals(){
    Decl* d = FindDecl(field);
    List<VarDecl*> *formals = dynamic_cast<FnDecl*>(d)->formals;//aqui
    int numFormals = formals->NumElements();
    int numActuals = actuals->NumElements();
    if (numActuals != numFormals){
        ReportError::NumArgsMismatch(field, numFormals, numActuals);
        return;
    }
    for (int i = 0; i < numFormals; i++){
        Type* tipoFormals = formals->Nth(i)->GetDeclaredType();
        Type* tipoActuals = actuals->Nth(i)->GetType();
        if (!tipoFormals->IsEquivalentTo(tipoActuals)){
            ReportError::ArgMismatch(actuals->Nth(i), i+1, tipoActuals, tipoFormals);
            return;
        }
    }

}

void Call::Check(){
    if (base){
        base->Check();
    }
    
    Decl* d;
    ClassDecl* cd = dynamic_cast<ClassDecl*>(this);
    Type* t;
    if (base ==NULL){
        if (cd==NULL){
            d=FindDecl(field);
            if (d==NULL){
                ReportError::IdentifierNotDeclared(field, LookingForFunction);
                return;
            }
        }    
        else{
            t = base->GetType();
            d = FindDecl(field);
            if (d== NULL){
                ReportError::IdentifierNotDeclared(field, LookingForFunction);
                return;
            }
        }
    }
    else{
        t = base->GetType();
        d = FindDecl(field);
        if (d ==NULL){
            if(dynamic_cast<ArrayType*>(t)!=NULL){
                return;
            }
            ReportError::FieldNotFoundInBase(field,t);
            return;
        }
    }    
    actuals->CheckAll();
    if(actuals->NumElements()){
        ValidateActuals();
    }
    FnDecl* fd = dynamic_cast<FnDecl*>(d);
    if (fd==NULL){
        return;
    }
    
} 


NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}

Type* NewExpr::GetType(){
    return Type::errorType;
}  
void NewExpr::Check(){
    
}

NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}

Type* NewArrayExpr::GetType(){
    // printf("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);

    if (!size->GetType()->IsEquivalentTo(Type::intType)){
        ReportError::NewArraySizeNotInteger(size);
    }
    if(dynamic_cast<NamedType*>(elemType)){
        if (!elemType->IsPrimitiveType() && !elemType->IsEquivalentTo(Type::voidType)){
        return new ArrayType(elemType);
        }
    }
    return new ArrayType(elemType);
}

void NewArrayExpr::Check(){
    // printf("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
    GetType(); // get type incluye lo de check

}
Type* ReadLineExpr::GetType(){
    return Type::errorType;
}   
Type* ReadIntegerExpr::GetType(){
    return Type::errorType;
}

       
