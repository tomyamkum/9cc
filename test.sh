#!/bin/bash

cc -S test.c

assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s test.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

echo テスト
assert 0 "main() {return 0;}"
assert 42 "main() {return 42;}"
assert 21 "main() {return 5+20-4;}"
assert 41 "main() {return  12 + 34  -5 ;}"
assert 47 "main() {return 5+6*7;}"
assert 15 "main() {return 5*(9-6);}"
assert 4 "main() {return (3+5)/2;}"
assert 10 "main() {return -10+20;}"
assert 1 "main() {return 1==1;}"
assert 1 "main() {return 2!=1;}"
assert 0 "main() {return 1<1;}"
assert 0 "main() {return 1!=1;}"
assert 1 "main() {return 3+2==5;}"
assert 0 "main() {return 3*4<1+2*2;}"
assert 1 "main() {a=1; b=2; return a+1==2;}"
assert 0 "main() {d=5; z=2; return z>d;}"
assert 1 "main() {d=5; z=2; return d==5;}"
assert 1 "main() {hello=5; yey=2; return hello==5;}"
assert 1 "main() {hello=5; yey=2; return hello>yey;}"
assert 4 "main() {hello=2; yey = 2; return hello + yey; a = 1;}"
assert 1 "main() {hello=2; yey = 2; return hello == yey; a = 1;}"
assert 0 "main() {hello=1;  yey = 2;  return hello == yey; a = 1;}"
assert 1 "main() {if(3!=3) return 3; return 1;}"
assert 1 "main() {if(3!=3) return 3; else return 1;}"
assert 3 "main() {if(3!=3) return 3; else a = 1; return a+2;}"
assert 5 "main() {a=1; while(a<5) a=a+1; return a;}"
assert 10 "main() {b=0;for(a=0;a<5;a=a+1) b=b+2; return b;}"
assert 7 "main() {b=0;c=2;for(a=0;a<5;a=a+1) {b=b+2;c=c+1;} return c;}"
assert 10 "main() {a=0;while(a<10) {a=a+1;b=3;} return a;}"
assert 100 "main() {a=0;for(i=0;i<10;i=i+1){for(j=0;j<10;j=j+1) {a=a+1;}} return a;}"
assert 4 "main() {return ret_4();}"
assert 12 "main() {return mul_two(2, 5)+2;}"
assert 6 "ret_4() return 4; main() {return ret_4() + 2;}"
assert 16 "ret_pow(a) {return a*a;} main() {ret_pow(2); return ret_pow(ret_pow(2));}"
assert 15 "fib(n) {if(n<=1) return n; else return fib(n-1)+n;} main() {return fib(5);}"
assert 3 "main() {x=3;y=&x;return *y;}"
echo OK
