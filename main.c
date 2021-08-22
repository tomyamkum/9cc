#include "9cc.h"

int main(int argc, char ** argv) {
  if (argc!=2) {
    fprintf(stderr, "引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  tokenize();
  program();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  for(int i=0;code[i];i++) {
    gen(code[i]);
    // printf("  pop rax\n");
  }
  return 0;
}
