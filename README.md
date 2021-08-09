# Cで作るコンパイラ
* https://www.sigbus.info/compilerbook

## Docker準備
```
docker build . -t "9cc"
```

## dockerコンテナの作成とログイン
```
docker run -it 9cc /bin/bash
```

## dockerコンテナへのログイン
```
docker exec -it <container-id> /bin/bash
```

## コンパイラの動作確認
```
cd /root/9cc
./test.sh 
```
