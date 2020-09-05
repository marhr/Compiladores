/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "hashtable.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}
void Decl::Check(stack<shared_ptr<Hashtable<Node*> > > s){

    // check what kind of Decl is it
    shared_ptr<Decl> t = make_shared<Decl>(*this);
    VarDecl* v = dynamic_cast<VarDecl*>(this);
    ClassDecl* c = dynamic_cast<ClassDecl*>(this);
    FnDecl* f = dynamic_cast<FnDecl*>(this); 
    InterfaceDecl* i = dynamic_cast<InterfaceDecl*>(this); 
    //shared_ptr<ClassDecl> c = dynamic_pointer_cast<ClassDecl>(t);
    //shared_ptr<FnDecl> f = dynamic_pointer_cast<FnDecl>(t);

    // printf("is null? %s\n", t==nullptr?"true":"false" );
    // printf("is null? %s\n", v==nullptr?"true":"false" );
    // printf("is null? %s\n", c==nullptr?"true":"false" );
    // printf("is null? %s\n", f==nullptr?"true":"false" );

    if (v){
        cout << "es VarDecl" << endl;
        v->Check(s);
    }
    if (c){
        cout << "es ClassDecl" << endl;
        c->Check(s);
    }
    if (f){
        cout << "es FnDecl" << endl;
        f->Check(s);
    }    
    if (i){
        cout << "es InterfaceDecl" << endl;
    }
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
    name = n;
}

void VarDecl::Check(stack<shared_ptr<Hashtable<Node*> > > s){
    //auto prev = s.top()->Lookup(this->name->name);
    Node* prev = s.top()->Lookup((this->name)->name);
    // stack<shared_ptr<Hashtable<Node*> > > ss;
    if(prev!= NULL){
        Decl* p = dynamic_cast<Decl*>(prev);
        ReportError::DeclConflict(dynamic_cast<Decl*>(this), p);
    }
    else{
        s.top()->Enter((this->name)->name, this);
    }
    
}
  

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
    name = n;
}

// constructor que inicia un nuevo scope
ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m, Hashtable<Decl*> *ht) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
    // crea el nuevo scope
    scope = ht;    
}
void ClassDecl::Check(stack<shared_ptr<Hashtable<Node*> > > s){
    Node* prev = s.top()->Lookup((this->name)->name);
    if(prev!= NULL){
        Decl* p = dynamic_cast<Decl*>(prev);
        ReportError::DeclConflict(dynamic_cast<Decl*>(this), p);
    }
    else if(prev==NULL){
        s.top()->Enter((this->name)->name, this); //adds class to scope
    }
    else if (members){ // if there is FieldList
    cout << "FLAG" << endl;
        Hashtable<Node*> ns; // nuevo scope
        s.push(make_shared<Hashtable<Node*> >(ns)); // agrega scope
        members->Check(s); //
    }
    
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
    name = n;
}

void InterfaceDecl::Check(stack<shared_ptr<Hashtable<Node*> > > s){
    Node* prev = s.top()->Lookup((this->name)->name);
    if(prev!= NULL){
        Decl* p = dynamic_cast<Decl*>(prev);
        ReportError::DeclConflict(dynamic_cast<Decl*>(this), p);
    }
    else{
        s.top()->Enter((this->name)->name, this); //not quite sure of its value
    }
}

	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    name = n;
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::Check(stack<shared_ptr<Hashtable<Node*> > > s){    
    Node* prev = s.top()->Lookup((this->name)->name);
    if(prev!= NULL){
        Decl* p = dynamic_cast<Decl*>(prev);
        ReportError::DeclConflict(dynamic_cast<Decl*>(this), p);
    }
    else{
        s.top()->Enter((this->name)->name, this);
        Hashtable<Node*> ns; // nuevo scope
        s.push(make_shared<Hashtable<Node*> >(ns)); // agrega scope
        formals->Check(s); // formals perteneceran al ns
        if (body) body->Check(s); 
    }
    
}



