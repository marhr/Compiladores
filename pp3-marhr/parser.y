/* File: parser.y
 * --------------
 * Yacc input file to generate the parser for the compiler.
 *
 * pp3: add parser rules and tree construction from your pp2. You should
 *      not need to make any significant changes in the parser itself. After
 *      parsing completes, if no syntax errors were found, the parser calls
 *      program->Check() to kick off the semantic analyzer pass. The
 *      interesting work happens during the tree traversal.
 */

%{

#include "scanner.h" // for yylex
#include "parser.h"
#include "errors.h"

void yyerror(char *msg); // standard error-handling routine

%}

 
/* yylval 
 * ------
 */
%union {
    int integerConstant;
    bool boolConstant;
    char *stringConstant;
    double doubleConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null
    Decl *decl;
    List<Decl*> *declList;


    VarDecl *variableDecl;
    ClassDecl *classDecl;
    InterfaceDecl *interfaceDecl;
    FnDecl *functionDecl;
    FnDecl *defineFunction;

    //ast_expr
    Expr *expr;
    Expr *constant;
    Expr *call;
    List<Expr*> *actuals;
    List<Expr*> *exprlist;


    //ast_stmt
    StmtBlock *stmtBlock;
    WhileStmt *whilestmt;
    IfStmt *ifstmt;
    ForStmt *forstmt;
    ReturnStmt *returnstmt;
    PrintStmt *printstmt;
    BreakStmt *breakstmt;
    Stmt *elseOptional;
    SwitchStmt *switchStmt;
    CaseStmt *casestmt;


    //ast_type
    Type *type;

    // others 
    VarDecl *var;
    List<VarDecl*> *varList;
    List<VarDecl*> *vardeclList;
    List<VarDecl*> *formals;
    // List<Decl*> *stmtList;
    Stmt *stmt;
    List<Stmt*> *stmtList;
    Expr *exprOptional;
    NamedType *extIdentOptional;
    List<NamedType*> *implIdentOptional;
    List<NamedType*> *identList;
    Decl *field;
    List<Decl*> *fieldList;
    List<Decl*> *prototypeList;
    FnDecl *prototype;
    LValue *lval;
}


/* Tokens
 * ------
 */
%token   T_Void T_Bool T_Int T_Double T_String T_Class 
%token   T_LessEqual T_GreaterEqual T_Equal T_NotEqual T_Dims T_MinMin T_PlusPlus
%token   T_And T_Or T_Null T_Extends T_This T_Interface T_Implements
%token   T_While T_For T_If T_Else T_Return T_Break T_Switch T_Case T_Default
%token   T_New T_NewArray T_Print T_ReadInteger T_ReadLine

%token   <identifier> T_Identifier
%token   <stringConstant> T_StringConstant 
%token   <integerConstant> T_IntConstant
%token   <doubleConstant> T_DoubleConstant
%token   <boolConstant> T_BoolConstant


/* Non-terminal types
 * ------------------
 */
%type <declList>  DeclList 
%type <decl>              Decl
%type <variableDecl>      VariableDecl
%type <var>               Variable
%type <type>              Type
%type <functionDecl>      FunctionDecl
%type <defineFunction>    DefineFunction
%type <formals>           Formals
%type <varList>           VariableList
%type <classDecl>         ClassDecl
%type <extIdentOptional>  ExtIdentOptional
%type <implIdentOptional> ImplIdentOptional
%type <identList>         IdentifierList
%type <fieldList>         FieldList
// %type <field>             Field
%type <interfaceDecl>     InterfaceDecl 
%type <prototypeList>     PrototypeList
%type <prototype>         Prototype
%type <stmtBlock>         StmtBlock
%type <vardeclList>       VariableDeclList
%type <stmtList>          StmtList CaseList
%type <casestmt>          CaseStmt
%type <stmt>              Stmt
%type <exprOptional>      ExprOptional
%type <ifstmt>            IfStmt
//%type <elseOptional>      ElseOptional
%type <whilestmt>         WhileStmt
%type <forstmt>           ForStmt
%type <returnstmt>        ReturnStmt
%type <breakstmt>         BreakStmt
%type <printstmt>         PrintStmt
%type <switchStmt>        SwitchStmt
%type <exprlist>          ExprList
%type <expr>              Expr
%type <lval>              LValue
%type <call>              Call
%type <actuals>           Actuals
%type <constant>          Constant

/* Operands precedence */

%left '='
%left '+'
%left '-'
%left '*'
%left '/'
%left '%'
%left '>'
%left '<'
%left '!'
%left '.'
%left '['
%left T_Or
%left T_And
%left T_NotEqual
%left T_Equal
%left T_GreaterEqual
%left T_LessEqual
%left UMINUS T_MinMin T_PlusPlus
%nonassoc noElse
%nonassoc T_Else

%%
/* Rules
 * -----
	 
 */
Program           :   DeclList            { 
                                            // @1; 
                                            /* pp2: The @1 is needed to convince 
                                             * yacc to set up yylloc. You can remove 
                                             * it once you have other uses of @n*/
                                            Program *program = new Program($1);//,new Hashtable<Decl*>);
                                            // if no errors, advance to next phase
                                            // if (ReportError::NumErrors() == 0) 
                                               // program->Print(0);
                                            program->Check();
                                          }
                  ;

DeclList          :   DeclList Decl        { ($$=$1)->Append($2); }
                  |   Decl                 { ($$ = new List<Decl*>)->Append($1); }
                  ;

Decl              :   VariableDecl         { $$ = $1; }
                  |   FunctionDecl         { $$ = $1; }
                  |   ClassDecl            { $$ = $1; }
                  |   InterfaceDecl        { $$ = $1; }
                  ;

VariableDecl      :   Variable ';'        { $$ = $1; }
                  ;

Variable          :   Type T_Identifier    { $$ = new VarDecl(new Identifier(@2,$2), $1); }
                  ;

Type              :   T_Int                { $$ = Type::intType; }
                  |   T_Double             { $$ = Type::doubleType; }
                  |   T_Bool               { $$ = Type::boolType; }
                  |   T_String             { $$ = Type::stringType; }
                  |   T_Identifier         { $$ = new NamedType(new Identifier(@1,$1));}
                  |   Type T_Dims          { $$ = new ArrayType(Join(@1, @2),$1); }
                  ;

FunctionDecl      :   DefineFunction StmtBlock   { FnDecl *fun = $1; fun->SetFunctionBody($2); }
                  ;

DefineFunction    :   Type T_Identifier '(' Formals ')'     { $$ = new FnDecl(new Identifier(@2,$2), $1, $4); }
                  |   T_Void T_Identifier '(' Formals ')'   { $$ = new FnDecl(new Identifier(@2,$2), Type::voidType, $4); }
                  ;

Formals           :   VariableList         { $$ = $1; }
                  |   /* epsilon */        { $$ = new List<VarDecl*>; }
                  ;                  
          
VariableList      :   VariableList ',' Variable  { ($$ = $1)->Append($3); }
                  |   Variable                   { ($$ =new List<VarDecl*>)->Append($1); }
                  ;

ClassDecl         :   T_Class T_Identifier ExtIdentOptional ImplIdentOptional '{'FieldList'}' { $$ = new ClassDecl(new Identifier(@2, $2), $3, $4, $6); }
                  |   T_Class T_Identifier ExtIdentOptional ImplIdentOptional '{' '}'         { $$ = new ClassDecl(new Identifier(@2, $2), $3, $4, new List<Decl*>); }
                  ;

ExtIdentOptional  :   T_Extends T_Identifier { $$ = new NamedType(new Identifier(@2, $2)); }
                  |   /* zero or one occur */{ $$ = NULL; }
                  ;

ImplIdentOptional :   T_Implements IdentifierList     { $$ = $2; }
                  |   /* zero or one occur */         { $$ = new List<NamedType*>; }
                  ;

IdentifierList    :   IdentifierList ',' T_Identifier { ($$=$1)->Append(new NamedType(new Identifier(@3, $3))); }
                  |   T_Identifier                    { ($$=new List<NamedType*>)->Append(new NamedType(new Identifier(@1, $1))); }
                  ;

FieldList         :   FieldList VariableDecl          { ($$ = $1)->Append($2); }
                  |   FieldList FunctionDecl          { ($$ = $1)->Append($2); }
                  |   VariableDecl                    { ($$ = new List<Decl*>)->Append($1); }
                  |   FunctionDecl                    { ($$ = new List<Decl*>)->Append($1); }
                  ;

InterfaceDecl     :   T_Interface T_Identifier '{' PrototypeList '}' { $$ = new InterfaceDecl(new Identifier(@2,$2), $4); }
                  ;

PrototypeList     :   PrototypeList Prototype   { ($$=$1)->Append($2); }
                  |   Prototype                 { ($$= new List<Decl*>)->Append($1); } 
                  //|   /* zero or more occur */  { ($$= new List<Decl*>); }
                  ;



Prototype         :   Type T_Identifier '(' Formals ')' ';'    { $$ = new FnDecl(new Identifier(@2,$2), $1, $4); }
                  |   T_Void T_Identifier '(' Formals ')' ';'  { $$ = new FnDecl(new Identifier(@2,$2), Type::voidType, $4);  }
                  ;

StmtBlock         :   '{' VariableDeclList StmtList '}'  { $$ = new StmtBlock($2, $3);}//, new Hashtable<Stmt*>); }
                  |   '{' StmtList '}'                   { $$ = new StmtBlock(new List<VarDecl*>, $2);}//, new Hashtable<Stmt*>); }
                  |   '{' VariableDeclList '}'           { $$ = new StmtBlock($2, new List<Stmt*>);}//, new Hashtable<Stmt*>); }
                  |   '{''}'                             { $$ = new StmtBlock(new List<VarDecl*>, new List<Stmt*>); }
                  ;

VariableDeclList  :   VariableDeclList VariableDecl { ($$ = $1)->Append($2); }
                  |   VariableDecl                  { ($$ = new List<VarDecl*>)->Append($1); }
                  //|   /* zero or more occur */      { ($$ = new List<VarDecl*>); }
                  ;

StmtList          :   StmtList Stmt            { ($$=$1)->Append($2); }
                  |   Stmt                     { ($$= new List<Stmt*>)->Append($1); }
                  //|   /* zero or more occur */ { ($$= new List<Stmt*>); }
                  ;

Stmt              :   ExprOptional';'  { $$ = $1; }
                  |   IfStmt           { $$ = $1; }
                  |   WhileStmt        { $$ = $1; } 
                  |   ForStmt          { $$ = $1; } 
                  |   BreakStmt        { $$ = $1; }
                  |   ReturnStmt       { $$ = $1; }
                  |   PrintStmt        { $$ = $1; }
                  |   StmtBlock        { $$ = $1; }
                  |   SwitchStmt       { $$ = $1; }
                  ;

ExprOptional      :   Expr                             { $$ = $1; }
                  |   /* zero or one occur */          { $$ = new EmptyExpr(); }
                  ;                  

IfStmt            :    T_If '(' Expr ')' Stmt %prec noElse             { $$ = new IfStmt($3, $5, NULL) ; }
                  |    T_If '(' Expr ')' Stmt T_Else Stmt               { $$ = new IfStmt($3, $5, $7) ; }
                  ;

//ElseOptional      :   T_Else Stmt                           { $$ = $2; }
 //                 |   /* zero or more occur */              { $$ = NULL; }
  //                ;

WhileStmt         :   T_While '(' Expr ')' Stmt             {$$ = new WhileStmt($3, $5); }
                  ;

ForStmt           :   T_For '(' ExprOptional ';' Expr ';' ExprOptional ')' Stmt { $$ = new ForStmt($3, $5, $7, $9); }
                  ;

ReturnStmt        :   T_Return ExprOptional ';'             { $$ = new ReturnStmt(@1, $2); }
                  ;

BreakStmt         :   T_Break ';'                           { $$ = new BreakStmt(@1); }
                  ;

PrintStmt         :   T_Print '(' ExprList  ')' ';'         { $$ = new PrintStmt($3); }
                  ;

ExprList          :   ExprList ',' Expr                     { ($$=$1)->Append($3); }
                  |   Expr                                  { ($$ = new List<Expr*>)->Append($1); }
                  ;

SwitchStmt        :   T_Switch '(' Expr ')' '{' CaseList T_Default ':' StmtList '}'  { $$ = new SwitchStmt($3, $6, $9) ;}
                  |   T_Switch '(' Expr ')' '{' CaseList '}'                        { $$ = new SwitchStmt($3, $6, NULL)  ;}
                  ;

CaseList          :   CaseList CaseStmt                                { ($$ = $1)->Append($2) ; } 
                  |   CaseStmt                                         { ($$ = new List<Stmt*>)->Append($1); }
                  ;

CaseStmt          :   T_Case T_IntConstant ':' StmtList                 { $$ = new CaseStmt(new IntConstant(@2, $2), $4) ;}
                  |   T_Case T_IntConstant ':'                          { $$ = new CaseStmt(new IntConstant(@2, $2), new List<Stmt*>) ;}
                  ;

Expr              :   LValue '=' Expr                        { $$ = new AssignExpr($1,new Operator(@2, "="), $3); }
                  |   Constant                             { $$ = $1; }
                  |   LValue                                 { $$ = $1; }
                  |   T_This                                 { $$ = new This(@1); }  
                  |   Call                                  { $$ = $1; }
                  |   '('Expr')'                             { $$ = $2; }
                  |   '!' Expr                              { $$ = new LogicalExpr(new Operator(@1, "!"),$2); }
                  |   T_ReadInteger '('')'                  { $$ = new ReadIntegerExpr(@1); }
                  |   T_ReadLine '('')'                     { $$ = new ReadLineExpr(@1); }
                  |   T_New '(' T_Identifier ')'            { $$ = new NewExpr(Join(@1,@4), new NamedType(new Identifier(@3,$3))); }
                  |   T_NewArray '(' Expr ',' Type ')'      { $$ = new NewArrayExpr(Join(@1,@6), $3, $5); }
                  |   '-' Expr %prec UMINUS                 { $$ = new ArithmeticExpr(new Operator(@1,"-"), $2);} 
                  |   Expr '+' Expr                         { $$ = new ArithmeticExpr($1, new Operator(@2, "+"), $3); }
                  |   Expr '-' Expr                         { $$ = new ArithmeticExpr($1, new Operator(@2, "-"), $3); }
                  |   Expr '*' Expr                         { $$ = new ArithmeticExpr($1, new Operator(@2, "*"), $3); }
                  |   Expr '/' Expr                         { $$ = new ArithmeticExpr($1, new Operator(@2, "/"), $3); }
                  |   Expr '%' Expr                         { $$ = new ArithmeticExpr($1, new Operator(@2, "%"), $3); }
                  |   Expr '<' Expr                         { $$ = new RelationalExpr($1, new Operator(@2, "<"), $3); }
                  |   Expr T_LessEqual Expr                 { $$ = new RelationalExpr($1,new Operator(@2, "<="), $3); }
                  |   Expr '>' Expr                         { $$ = new RelationalExpr($1, new Operator(@2, ">"), $3); }
                  |   Expr T_GreaterEqual Expr              { $$ = new RelationalExpr($1,new Operator(@2, ">="), $3); }
                  |   Expr T_Equal Expr                     { $$ = new EqualityExpr($1,new Operator(@2, "=="), $3); }
                  |   Expr T_NotEqual Expr                  { $$ = new EqualityExpr($1,new Operator(@2, "!="), $3); }
                  |   Expr T_And Expr                       { $$ = new LogicalExpr($1, new Operator(@2, "&&"), $3); } 
                  |   Expr T_Or Expr                        { $$ = new LogicalExpr($1, new Operator(@2, "||"), $3); } 
                  ;

LValue            :   T_Identifier                     { $$ = new FieldAccess(NULL, new Identifier(@1, $1)); }
                  |   Expr '.' T_Identifier            { $$ = new FieldAccess($1, new Identifier(@3, $3)); }
                  |   Expr '[' Expr ']'                { $$ = new ArrayAccess(Join(@1,@4), $1, $3); }
                  |   LValue T_MinMin                  { $$ = new Postfix(new Operator(@2, "--"), $1); }                                                     
                  |   LValue T_PlusPlus                { $$ = new Postfix(new Operator(@2, "++"), $1); }  
                  ;

Call              :   T_Identifier '(' Actuals ')'     { $$ = new Call(Join(@1,@4), NULL,new Identifier(@1, $1), $3); }
                  |   Expr'.'T_Identifier'('Actuals')' { $$ = new Call(Join(@1,@6), $1, new Identifier(@3,$3), $5); }
                  ;

Actuals           :   ExprList                         { $$ = $1; }
                  |   /* epsilon */                    { $$ = new List<Expr*>; }
                  ;

Constant          :   T_IntConstant                    { $$ = new IntConstant(@1, $1); }
                  |   T_DoubleConstant                 { $$ = new DoubleConstant(@1, $1); }
                  |   T_BoolConstant                   { $$ = new BoolConstant(@1, $1); }
                  |   T_StringConstant                 { $$ = new StringConstant(@1, $1); }
                  |   T_Null                           { $$ = new NullConstant(@1); }
                  ;


%%


/* Function: InitParser
 * --------------------
 * This function will be called before any calls to yyparse().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the parser (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yydebug that controls whether yacc prints debugging information about
 * parser actions (shift/reduce) and contents of state stack during parser.
 * If set to false, no information is printed. Setting it to true will give
 * you a running trail that might be helpful when debugging your parser.
 * Please be sure the variable is set to false when submitting your final
 * version.
 */
void InitParser()
{
   PrintDebug("parser", "Initializing parser");
   yydebug = false;
}
