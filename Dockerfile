FROM gcc:8.2

RUN apt-get update \
  && apt install -y gdb
  && apt-get install git
  && git clone https://github.com/tomyamkum/9cc.git
  && make
