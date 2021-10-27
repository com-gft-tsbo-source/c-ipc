FROM golang:alpine AS BUILDIMAGE

ARG GITHASH
ARG TIMESTAMP
ARG COMPONENT

RUN apk --update add  build-base gcc abuild binutils make argp-standalone linux-headers git
RUN mkdir /tmp/build

COPY go /tmp/build/src

WORKDIR /tmp/build/src
RUN make distclean
WORKDIR /tmp/build/src/${COMPONENT}
RUN GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make dep
RUN GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make all

WORKDIR /tmp/build

FROM alpine

ARG GITHASH
ARG TIMESTAMP
ARG COMPONENT

RUN apk --update add curl
RUN mkdir /var/log/${COMPONENT}
RUN adduser -H -D ${COMPONENT}
COPY --from=BUILDIMAGE /tmp/build/src/dist/bin/${COMPONENT} /opt/${COMPONENT}
RUN chown ${COMPONENT}:${COMPONENT} /opt/${COMPONENT} && \
    chmod og-rwx,u=rx /opt/${COMPONENT}

USER ${COMPONENT}
ENTRYPOINT [ "/opt/ms-measure", "--version 1.0.0", "--name", "ms-measure", "--type", "thermometer", "--address", "01:00"  ] 
