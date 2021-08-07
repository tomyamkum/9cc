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
  Token *tok = token;
  token = token->next;
  return tok;
}

bool consume_return() {
  if(token->kind != TK_RETURN) {
    return false;
  }
  token = token->next;
  return true;
}

bool consume_if() {
  if(token->kind != TK_IF) {
    return false;
  }
  token = token->next;
  return true;
}

bool consume_else() {
  if(token->kind != TK_ELSE) {
    return false;
  }
  token = token->next;
  return true;
}

bool consume_while() {
  if(token->kind != TK_WHILE) {
    return false;
  }
  token = token->next;
  return true;
}

bool consume_for() {
  if(token->kind != TK_FOR) {
    return false;
  }
  token = token->next;
  return true;
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

bool usable_char(char p) {
  if('A'<=p && p<='Z') 
    return true;
  if('a'<=p && p<='z')
    return true;
  if('1'<=p && p<='9')
    return true;
  if(p=='_')
    return true;
  return false;
}

char *substr(char *str, int s, int t) {
  char *ans = calloc(t-s, sizeof(char));

  strncpy(ans, str+s, t-s);
  return ans;
}

LVar *find_lvar(Token *tok) {
  for(LVar *var = locals; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
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

    //if ('a' <= *user_input && *user_input <= 'z') {
    //  cur = new_token(TK_IDENT, cur, user_input++, 1);
    //  continue;
    //}
    int varlen = 0;
    if(usable_char(*user_input)) {
      while(strlen(user_input)>varlen && usable_char(user_input[varlen])) {
        varlen++;
      }
    }

    if(varlen>0) {
      if((varlen==6) && (strncmp(user_input, "return", 6)==0)) {
        cur = new_token(TK_RETURN, cur, "return", 6);
      }
      else if((varlen==2) && (strncmp(user_input, "if", 2)==0)) {
        cur = new_token(TK_IF, cur, "if", 2);
      }
      else if((varlen==4) && (strncmp(user_input, "else", 4)==0)) {
        cur = new_token(TK_ELSE, cur, "else", 4);
      }
      else if((varlen==5) && (strncmp(user_input, "while", 5)==0)) {
        cur = new_token(TK_WHILE, cur, "while", 5);
      }
      else if((varlen==3) && (strncmp(user_input, "for", 3)==0)) {
        cur = new_token(TK_FOR, cur, "for", 3);
      }
      else {
        cur = new_token(TK_IDENT, cur, substr(user_input, 0, varlen), varlen);
      }
      user_input += varlen;
      continue;
    }

    error_at(token->str, "トークナイズできません");
  }

  new_token(TK_EOF, cur, user_input, 1);
  token = head.next;
  return; 
}


void program() {
  locals = calloc(1, sizeof(LVar));
  locals->offset = 0;
  int i = 0;
  while(!at_eof()) 
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if(consume_return()) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    if(!consume(";")) {
      error_at(token->str, "';'ではないトークンです");
    }
  }
  else if(consume_if()) {
    node = calloc(1, sizeof(Node));
    if(!consume("(")) {
      error_at(token->str, "'('ではないトークンです");
    }
    node->kind = ND_IF;
    node->lhs = expr();
    if(!consume(")")) {
      error_at(token->str, "')'ではないトークンです");
    }
    node->rhs = stmt();
    if(consume_else()) {
      node->els = stmt();
    }
  }
  else if(consume_while()) {
    node = calloc(1, sizeof(Node));
    if(!consume("(")) {
      error_at(token->str, "'('ではないトークンです");
    }
    node->kind = ND_WHILE;
    node->lhs = expr();
    if(!consume(")")) {
      error_at(token->str, "')'ではないトークンです");
    }
    node->rhs = stmt();
  }
  else if(consume_for()) {
    node = calloc(1, sizeof(Node));
    if(!consume("(")) {
      error_at(token->str, "'('ではないトークンです");
    }
    node->kind = ND_FOR;
    if(!consume(";")) {
      node->forini = expr();
      if(!consume(";")) {
        error_at(token->str, "';'ではないトークンです");
      }
    }
    if(!consume(";")) {
      node->forstop = expr();
      if(!consume(";")) {
        error_at(token->str, "';'ではないトークンです");
      }
    }
    if(!consume(";")) {
      node->forpro = expr();
    }
    if(!consume(")")) {
      error_at(token->str, "')'ではないトークンです");
    }
    node->lhs = stmt();
  }
  else {
    node = expr();
    if(!consume(";")) {
      error_at(token->str, "';'ではないトークンです");
    }
  }
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

    LVar *lvar = find_lvar(tok);
    if(lvar) {
      node->offset = lvar->offset;
    }
    else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }

  return new_node_num(expect_number());
}


bool at_eof() {
  return token->kind == TK_EOF;
}

