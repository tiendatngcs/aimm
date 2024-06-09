%{

int  yylex(void);
void yyerror(char * msg);
void config_assign_string( char const * field, char const * value );
void config_assign_int( char const * field, int value );
void config_assign_float( char const * field, double value );
void config_assign_addr( char const * field, unsigned long value );

#ifdef _WIN32
#pragma warning ( disable : 4102 )
#pragma warning ( disable : 4244 )
#endif

%}

%union {
  char   *name;
  int    num;
  double fnum;
  unsigned long addr;
}

%token <name> STR
%token <num>  NUM
%token <fnum> FNUM
%token <addr> ADDR

%%

commands : commands command
         | command
;

command : STR '=' STR ';'   { config_assign_string( $1, $3 ); free( $1 ); free( $3 ); }
        | STR '=' NUM ';'   { config_assign_int( $1, $3 ); free( $1 ); }
        | STR '=' FNUM ';'  { config_assign_float( $1, $3 ); free( $1 ); }
        | STR '=' ADDR ';'  { config_assign_addr( $1, $3 ); free( $1 ); }
;

%%
