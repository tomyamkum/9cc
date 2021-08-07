#!/bin/bash

assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
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
assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34  -5 ;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"
assert 1 "1==1;"
assert 1 "2!=1;"
assert 0 "1<1;"
assert 0 "1!=1;"
assert 1 "3+2==5;"
assert 0 "3*4<1+2*2;"
assert 1 "a=1; b=2; a+1==2;"
assert 0 "d=5; z=2; z>d;"
assert 1 "d=5; z=2; d==5;"
assert 1 "hello=5; yey=2; hello==5;"
assert 1 "hello=5; yey=2; hello>yey;"
assert 4 "hello=2; yey = 2; return hello + yey; a = 1;"
assert 1 "hello=2; yey = 2; return hello == yey; a = 1;"
assert 0 "hello=1;  yey = 2;  return hello == yey; a = 1;"
assert 1 "if(3==1) return 3; return 1;"
echo OK
