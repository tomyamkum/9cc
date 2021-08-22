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
bool expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

bool consume(TokenKind tk) {
  if(token->kind != tk) {
    return false;
  }
  token = token->next;
  return true;
}

int consume_num() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
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

bool at_eof() {
  return token->kind == TK_EOF;
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

    if (*user_input == '+' || *user_input == '-' || *user_input == '*' || *user_input == '/' || *user_input == '(' || *user_input == ')' || *user_input == '<' || *user_input == '>' || *user_input == '=' || *user_input == ';' || *user_input=='{' || *user_input=='}' || *user_input==',' || *user_input=='&') {
      cur = new_token(TK_RESERVED, cur, user_input++, 1);
      continue;
    }

    if (isdigit(*user_input)) {
      cur = new_token(TK_NUM, cur, user_input, 0);
      cur->val = strtol(user_input, &user_input, 10);
      continue;
    }

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
      else if((varlen==3) && (strncmp(user_input, "int", 3)==0)) {
        cur = new_token(TK_INT, cur, "int", 3);
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
  int i = 0;
  while(!at_eof()) {
    code[i++] = top_level();
  }
  code[i] = NULL;
}

Node *top_level() {
  locals = calloc(1, sizeof(LVar));
  locals->offset = 0;
  Node *node;
  if(!consume(TK_INT)) 
    error_at(token->str, "型定義してください");
  Token *tok = consume_ident();
  if(tok) {
    Node *node = calloc(1, sizeof(Node));
    if(expect("(")) {
      node->kind = ND_FUNC;
      node->name = tok->str;
      node->argslen = 0;
      while(!expect(")")) {
        consume(TK_INT);
        Token *tok = consume_ident();
        LVar *lvar;
        lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = locals->offset + 8;
        locals = lvar;
        node->funcargs[node->argslen] = calloc(1, sizeof(Node));
        node->funcargs[node->argslen]->offset = lvar->offset;
        node->funcargs[node->argslen]->kind = ND_LVAR;
        node->argslen++;
        expect(",");
      }
      if(expect("{")) {
        int i = 0;
        while(!expect("}")) {
          node->stmt[i++] = stmt();
        }
        node->stmt[i] = NULL;
      }
      else {
        node->stmt[0] = stmt();
        node->stmt[1] = NULL;
      }
      return node;
    }
  }
}

Node *stmt() {
  Node *node;
  node = calloc(1, sizeof(Node));

  if(expect("{")) {
    int stmtnum = 0;
    node->kind = ND_BLOCK;
    int i = 0;
    while(!expect("}")) 
      node->stmt[i++] = stmt();
    node->stmt[i] = NULL;
  }
  else if(consume(TK_INT)) {
    Token *tok = consume_ident();
    node->kind = ND_LVAR;
    LVar *lvar;
    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->offset = locals->offset + 8;
    node->offset = lvar->offset;
    locals = lvar;
    if(!expect(";")) {
      error_at(token->str, "';'ではないトークンです");
    }
  }
  else if(consume(TK_RETURN)) {
    node->kind = ND_RETURN;
    node->lhs = expr();
    if(!expect(";")) {
      error_at(token->str, "';'ではないトークンです");
    }
  }
  else if(consume(TK_IF)) {
    if(!expect("(")) {
      error_at(token->str, "'('ではないトークンです");
    }
    node->kind = ND_IF;
    node->lhs = expr();
    if(!expect(")")) {
      error_at(token->str, "')'ではないトークンです");
    }
    node->rhs = stmt();
    if(consume(TK_ELSE)) {
      node->els = stmt();
    }
  }
  else if(consume(TK_WHILE)) {
    if(!expect("(")) {
      error_at(token->str, "'('ではないトークンです");
    }
    node->kind = ND_WHILE;
    node->lhs = expr();
    if(!expect(")")) {
      error_at(token->str, "')'ではないトークンです");
    }
    node->rhs = stmt();
  }
  else if(consume(TK_FOR)) {
    if(!expect("(")) {
      error_at(token->str, "'('ではないトークンです");
    }
    node->kind = ND_FOR;
    if(!expect(";")) {
      node->forini = expr();
      if(!expect(";")) {
        error_at(token->str, "';'ではないトークンです");
      }
    }
    if(!expect(";")) {
      node->forstop = expr();
      if(!expect(";")) {
        error_at(token->str, "';'ではないトークンです");
      }
    }
    if(!expect(";")) {
      node->forpro = expr();
    }
    if(!expect(")")) {
      error_at(token->str, "')'ではないトークンです");
    }
    node->lhs = stmt();
  }
  else {
    node = expr();
    if(!expect(";")) {
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
  if (expect("=")) 
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  for(;;) {
    if(expect("==")) 
      node = new_node(ND_EQUAL, node, relational());
    else if(expect("!="))
      node = new_node(ND_NEQUAL, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for(;;) {
    if(expect("<"))
      node = new_node(ND_SMALLER, node, add());
    else if(expect("<="))
      node = new_node(ND_ESMALLER, node, add());
    else if(expect(">"))
      node = new_node(ND_LARGER, node, add());
    else if(expect(">="))
      node = new_node(ND_ELARGER, node, add());
    else 
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for(;;) {
    if(expect("+"))
      node = new_node(ND_ADD, node, mul());
    else if(expect("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for(;;) {
    if(expect("*"))
      node = new_node(ND_MUL, node, unary());
    else if(expect("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if(expect("+"))
    return primary();
  else if(expect("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  else if(expect("&"))
    return new_node(ND_ADDR, unary(), new_node_num(0));
  else if(expect("*"))
    return new_node(ND_DEREF, unary(), new_node_num(0));
  return primary();
}

Node *primary() {
  if (expect("(")) {
    Node *node = expr();
    if(!expect(")"))
      error_at(token->str, "')'ではありません");
    return node;
  }

  Token *tok = consume_ident();
  if(tok) {
    Node *node = calloc(1, sizeof(Node));
    if(expect("(")) {
      node->kind = ND_CALL;
      node->name = tok->str;
      node->argslen = 0;
      while(!expect(")")) {
        node->callargs[node->argslen++] = expr();
        expect(",");
      }
      return node;
    }
    node->kind = ND_LVAR;
    LVar *lvar = find_lvar(tok);

    if(lvar) {
      node->offset = lvar->offset;
    }
    else {
      printf("%s\n", tok->str);
      error_at(token->str, "定義されていない変数です");
    }
    return node;
  }

  return new_node_num(consume_num());
}

