# Cで作るコンパイラ
* https://www.sigbus.info/compilerbook

## Docker準備
```
docker build . -t "9cc"
```

## dockerコンテナの作成とログイン
```
docker run -it --name "9cc" -v $(pwd):/9cc 9cc /bin/sh
```

## dockerコンテナへのログイン
```
docker exec -it <container-id> /bin/sh
```

## コンパイラの動作確認
```
cd /9cc
make
./test.sh 
```
