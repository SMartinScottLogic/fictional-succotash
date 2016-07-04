FROM alpine:3.3

RUN apk add --no-cache bash autoconf automake libtool g++ make file-dev
WORKDIR /tmp
COPY Makefile.am configure.ac NEWS README AUTHORS ChangeLog ./
COPY src src/
COPY m4 m4/
RUN autoreconf -i && ./configure && make && make install clean && rm -rf *
WORKDIR /
ENTRYPOINT ["/bin/bash"]
