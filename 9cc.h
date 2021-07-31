#include<stdbool.h>

char *user_input;

typedef enum {
  TK_RESERVED,
  TK_NUM,
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
  ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void error_at(char *loc, char *fmt, ...);

void expect(char *op);

int expect_number();

bool consume(char *op);

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

void gen(Node *node);

bool at_eof();

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

Token *tokenize(char *p);


