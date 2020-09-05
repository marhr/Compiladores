
/*
 * file:  dpp.l
 * ------------
 * Lex inupt file to generate the scanner for the preprocessor.
 * If you decide to use lex for the preprocesor, put your rules
 * here, otherwise the file can remain empty.
 */

%{

#include <iostream>
#include "errors.h"
#include "Hashtable.h"
enum {
	LOOKUP = 0,
	DEF,
	NAM
};

int key = 0;
Hashtable* table = new Hashtable();
// int add_word(int type, char *word);
// int lookup_word(char *word);
string find(const string& key);  
void insert(const string& key, const string& value);
string remove(const string& key);

%}

%option yylineno

SINGLE_LINE_COMMENT ([\/\/].*)
BEGIN_COMMENT ("/*")
END_COMMENT ("*/")
MULTI_LINE_COMMENT ({BEGIN_COMMENT}+([^{END_COMMENT}]|\n)*{END_COMMENT})
DEFINE (#define)
NAME ([A-Z]+)
REPLACEMENT ({DEFINE}[ ]{NAME}[ ].*)
SHARP (#{NAME})

%%

\n { printf("\n");}
{MULTI_LINE_COMMENT} { printf("\n"); }

{REPLACEMENT} { int i = 8;
		char space = ' ';
		while((char)yytext[i] - space != 0){
			// printf("DEFINE i:%d, yytext: %c \n", i, yytext[i]);
			i++;}
		int name_size = i-7;
		char name[name_size];
		char rep[yyleng-i];
		strncpy(name, yytext+8, name_size );
		strncpy(rep, yytext+(8+name_size), (yyleng-i));
		printf("name: %s replacement: %s", name, rep);
 
		if(table -> find(name).c_str() == NULL){ /* couldnt find */
			table->insert(name, rep);
			table->print();
		}
		else{
			ReportError::InvalidDirective(yylineno);
		}}
{SHARP} { int j=0;
		char space = ' ';
		while((char)yytext[j] - space != 0){
			// printf("DEFINE i:%d, yytext: %c \n", j, yytext[j]);
			j++;}}

%%

