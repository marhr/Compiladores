/* File:  scanner.l
 * ----------------
 * Lex input file to generate the scanner for the compiler.
 */

%{

/* The text within this first region delimited by %{ and %} is assumed to
 * be C/C++ code and will be copied verbatim to the lex.yy.c file ahead
 * of the definitions of the yylex() function. Add other header file inclusions
 * or C++ variable declarations/prototypes that are needed by your code here.
 */

#include <string.h>
#include "scanner.h"
#include "utility.h" // for PrintDebug()
#include "errors.h"

/* Global variable: yylval
 * -----------------------
 * This global variable is how we get attribute information about the token
 * just scanned to the client. The scanner sets the global variable
 * appropriately and since it's global the client can just read it.  In the
 * future, this variable will be declared for us in the y.tab.c file
 * produced by Yacc, but for now, we declare it manually.
 */
YYSTYPE yylval;  // manually declared for pp1, later Yacc provides

/* Global variable: yylloc
 * -----------------------
 * This global variable is how we get position information about the token
 * just scanned to the client. (Operates similarly to yylval above)
 */
struct yyltype yylloc; // manually dclared for pp1, later Yacc provides

/* Macro: YY_USER_ACTION 
 * ---------------------
 * This flex built-in macro can be defined to provide an action which is
 * always executed prior to any matched rule's action. Basically, it is
 * a way of having a piece of code common to all actions factored out to
 * this routine.  We already defined it for you and left the empty
 * function DoBeforeEachAction ready for your use as needed. It will
 * be called once for each pattern scanned from the file, before
 * executing its action.
 */
static void DoBeforeEachAction(); 
#define YY_USER_ACTION DoBeforeEachAction();

/* some counters for DoBeforeEachAction() */
int count_ini = 1;
int count_fin = 1;
int space = 0;
%}

 /* The section before the first %% is the Definitions section of the lex
  * input file. Here is where you set options for the scanner, define lex
  * states, and can set up definitions to give names to regular expressions
  * as a simple substitution mechanism that allows for more readable
  * entries in the Rules section later. 
  */

/* adds yylineno */
%option yylineno

/* state */
%s STRING

UNTERM_STRING_INIT (\"[^\"]*\n)
UNTERM_STRING_FIN (\"\n)
VALID_STRING (\".*\"\n)


%%             /* BEGIN RULES SECTION */
 /* All patterns and actions should be placed between the start and stop
  * %% markers which delimit the Rules section. 
  */ 

[ ]+ { space++; }
[\t]+ { space += 4; }


void { return T_Void; }
string { return T_String; }
this { return T_This; }
while { return T_While; }
break { return T_Break; }
int { return T_Int; }
class { return T_Class; }
extends { return T_Extends; }
if { return T_If; }
New { return T_New; }
double { return T_Double; }
interface { return T_Interface; }
implements { return T_Implements; }
else { return T_Else; }
NewArray { return T_NewArray; }
bool { return T_Bool; }
null { return T_Null; }
for { return T_For; }
return { return T_Return; }



true {  yylval.boolConstant = true; return T_BoolConstant ; }
false { yylval.boolConstant = false; return T_BoolConstant ; }

"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"%" { return '%'; }
"<" { return '<'; }
">" { return '>'; }
"=" { return '='; }
"!" { return '!'; }
";" { return ';'; }
"," { return ','; }
"." { return '.'; }
"[" { return '['; }
"]" { return ']'; }
"(" { return '('; }
")" { return ')'; }
"{" { return '{'; }
"}" { return '}'; }
"<=" { return T_LessEqual; }
">=" { return T_GreaterEqual; }
"!=" { return T_NotEqual; }
"==" { return T_Equal; }
"&&" { return T_And; }
"||" { return T_Or; }

"$" { ReportError::UnrecogChar(&yylloc, '$'); }
"@" { ReportError::UnrecogChar(&yylloc, '@'); }
"^" { ReportError::UnrecogChar(&yylloc, '^'); }
"&" { ReportError::UnrecogChar(&yylloc, '&'); }
"~" { ReportError::UnrecogChar(&yylloc, '~'); }
"`" { ReportError::UnrecogChar(&yylloc, '`'); }
"?" { ReportError::UnrecogChar(&yylloc, '?'); }

{UNTERM_STRING_INIT} {count_ini = 0; count_fin = 0; space = 0;
                      ReportError::UntermString(&yylloc, yytext); }
{VALID_STRING} {yylval.stringConstant = strdup(yytext) ; return T_StringConstant;}

[a-zA-Z][a-zA-Z0-9"_"]* { 
  strcpy(yylval.identifier, yytext);
  if(yyleng > 31) ReportError::LongIdentifier(&yylloc, yytext);
  return T_Identifier; }

[0-9]+ { yylval.integerConstant = atoi(yytext); return T_IntConstant; }
0[xX][0-9a-fA-F]+ { yylval.integerConstant = strtol(yytext, NULL,16); return T_IntConstant; }
[0]*[0-9]+"."[0-9]* {yylval.doubleConstant = atof(yytext); return T_DoubleConstant; }
([0-9]+)|([0-9]+\.[0-9]*)([eE][-+]?[0-9]+) { yylval.doubleConstant = atof(yytext); return T_DoubleConstant; }

\"[^\"\n]*["\n] { yylval.stringConstant = strdup(yytext) ; return T_StringConstant; }

\n { count_ini = 1; count_fin = 1; space = 0; }
. { ECHO;}

%%
/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated lex.yy.c file.
 * This section is where you put definitions of helper functions.
 */


/* Function: InitScanner
 * ---------------------
 * This function will be called before any calls to yylex().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the scanner (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yy_flex_debug that controls whether flex prints debugging information
 * about each token and what rule was matched. If set to false, no information
 * is printed. Setting it to true will give you a running trail that might
 * be helpful when debugging your scanner. Please be sure the variable is
 * set to false when submitting your final version.
 */
void InitScanner()
{
    PrintDebug("lex", "Initializing scanner");
    yy_flex_debug = false;
}


/* Function: DoBeforeEachAction()
 * ------------------------------
 * This function is installed as the YY_USER_ACTION. This is a place
 * to group code common to all actions.
 */
static void DoBeforeEachAction()
{
	count_ini = count_fin;
  count_ini = count_ini+space;
	count_fin = count_ini + yyleng;
	yylloc.first_line = yylineno;
	yylloc.first_column = count_ini;
	yylloc.last_column = count_fin-1;  
}
