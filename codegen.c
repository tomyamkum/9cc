#include "9cc.h"

char *argregs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) 
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

static int count(void) {
  static int i = 1;
  return i++;
}

// 作成したnodeを深さ優先探索しながらアセンブリに落としていく
void gen(Node *node) {
  if(node->kind==ND_BLOCK) {
    int i = 0;
    while(node->stmt[i]) {
      gen(node->stmt[i]);
      printf("  pop rax\n");
      i++;
    }
    return;
  }
  if(node->kind==ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }
  if(node->kind==ND_IF) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    int label = count();
    printf("  je .Lelse%d\n", label);
    gen(node->rhs);
    printf("  jmp .Lend%d\n", label);
    printf(".Lelse%d:\n", label);
    if(node->els)
      gen(node->els);
    printf(".Lend%d:\n", label);
    return;
  }
  if(node->kind==ND_WHILE) {
    int label = count();
    printf(".Lbegin%d:\n", label);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", label);
    gen(node->rhs);
    printf("  jmp .Lbegin%d\n", label);
    printf(".Lend%d:\n", label);
    return;
  }
  if(node->kind==ND_FOR) {
    int label = count();
    gen(node->forini);
    printf(".Lbegin%d:\n", label);
    gen(node->forstop);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", label);
    gen(node->lhs);
    gen(node->forpro);
    printf("  jmp .Lbegin%d\n", label);
    printf(".Lend%d:\n", label);
    return;
  }
  switch(node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_FUNC:
      for(int i=0;i<node->argslen;++i) {
        printf("  mov %s, %d\n", argregs[i], node->args[i]);
      }
      printf("  call %s\n", node->name);
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQUAL:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NEQUAL:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_SMALLER:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_ESMALLER:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LARGER:
      printf("  cmp rdi, rax\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_ELARGER:
      printf("  cmp rdi, rax\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }
  printf("  push rax\n");
}

