FROM gcc:8.2

RUN apt-get update \
  && apt install -y gdb \
