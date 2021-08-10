FROM alpine AS BUILDIMAGE

RUN apk add build-base gcc abuild binutils make argp-standalone linux-headers
RUN mkdir /tmp/build
WORKDIR /tmp/build

COPY lib /tmp/build/lib
COPY src /tmp/build/src
COPY include /tmp/build/include

WORKDIR /tmp/build/src/client
RUN mkdir -p ../../bin
RUN LDFLAGS=-largp make

WORKDIR /tmp/build/src/server
RUN mkdir -p ../../bin
RUN LDFLAGS=-largp make

FROM alpine

RUN mkdir /var/log/server
RUN adduser -H -D server
USER server
COPY --from=BUILDIMAGE /tmp/build/bin/server /opt/server
COPY --from=BUILDIMAGE /tmp/build/bin/client /opt/client
ENTRYPOINT [ "/opt/server" ] 
CMD [ "--no-daemon", "--server" ]