#include "9cc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

////////////////// ここからtokenを直接触る関数 //////////////////
void expect(char *op) {
  if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *ans = calloc(1, sizeof(Token));
  ans->str = token->str;
  token = token->next;
  return ans;
}

////////////////// ここまでtokenを直接触る関数 //////////////////

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

void tokenize() {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*user_input) {
    if (isspace(*user_input)) {
      user_input++;
      continue;
    }

    if ((strlen(user_input) >= 2) && ((strncmp(user_input, "==", 2)==0) || (strncmp(user_input, "!=", 2)==0) || (strncmp(user_input, ">=", 2)==0) || (strncmp(user_input, "<=", 2)==0))) {
      cur = new_token(TK_RESERVED, cur, user_input, 2);
      user_input += 2;
      continue;
    }

    if (*user_input == '+' || *user_input == '-' || *user_input == '*' || *user_input == '/' || *user_input == '(' || *user_input == ')' || *user_input == '<' || *user_input == '>' || *user_input == '=' || *user_input == ';') {
      cur = new_token(TK_RESERVED, cur, user_input++, 1);
      continue;
    }

    if (isdigit(*user_input)) {
      cur = new_token(TK_NUM, cur, user_input, 0);
      cur->val = strtol(user_input, &user_input, 10);
      continue;
    }

    if ('a' <= *user_input && *user_input <= 'z') {
      cur = new_token(TK_IDENT, cur, user_input++, 1);
      continue;
    }

    error_at(token->str, "トークナイズできません");
  }

  new_token(TK_EOF, cur, user_input, 1);
  token = head.next;
  return; 
}


void program() {
  int i = 0;
  while(!at_eof()) 
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("=")) 
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  for(;;) {
    if(consume("==")) 
      node = new_node(ND_EQUAL, node, relational());
    else if(consume("!="))
      node = new_node(ND_NEQUAL, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for(;;) {
    if(consume("<"))
      node = new_node(ND_SMALLER, node, add());
    else if(consume("<="))
      node = new_node(ND_ESMALLER, node, add());
    else if(consume(">"))
      node = new_node(ND_LARGER, node, add());
    else if(consume(">="))
      node = new_node(ND_ELARGER, node, add());
    else 
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for(;;) {
    if(consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if(consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for(;;) {
    if(consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if(consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if(consume("+"))
    return primary();
  else if(consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if(tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8;
    return node;
  }

  return new_node_num(expect_number());
}


bool at_eof() {
  return token->kind == TK_EOF;
}

