#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char *user_input;

typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_RETURN,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

Token *token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQUAL,
  ND_NEQUAL,
  ND_SMALLER,
  ND_ESMALLER,
  ND_LARGER,
  ND_ELARGER,
  ND_ASSIGN,
  ND_LVAR,
  ND_NUM,
  ND_RETURN,
} NodeKind;

typedef struct Node Node;
Node *code[100];

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

void expect(char *op);

int expect_number();

bool consume(char *op);
Token *contume_ident();

void program();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

void gen_lval(Node *node);
void gen(Node *node);

bool at_eof();

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

void tokenize();

bool usable_name(char *p);

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

LVar *locals;

LVar *find_lvar(Token *tok);
