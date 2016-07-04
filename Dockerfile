FROM alpine:3.3

RUN apk add --no-cache bash autoconf automake libtool g++ make file-dev && addgroup -S dev && adduser -G dev -S dev
WORKDIR /home/dev
RUN mkdir -p m4 src 
ADD Makefile.am configure.ac NEWS README AUTHORS ChangeLog ./
ADD src src/
ADD m4 m4/
RUN find . -type d -print0 | xargs -0 chmod 755 && find . -type f -print0 | xargs -0 chmod 644
RUN find . -print0 | xargs -0 chown dev:dev
USER dev
RUN autoreconf -i && ./configure && make

ENTRYPOINT ["/bin/bash"]
