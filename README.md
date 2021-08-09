# Cで作るコンパイラ
* https://www.sigbus.info/compilerbook

## Docker準備
```
docker build . -t "9cc"
```

## dockerコンテナの初期作成+ログイン
```
docker run -it --name "9cc" -v $(pwd):/9cc 9cc /bin/bash
```

## dockerコンテナへのログイン
```
docker exec -it <container-id> /bin/bash
```

## コンパイラの動作確認
```
cd /9cc
make
./test.sh 
```
