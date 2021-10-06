FROM debian:buster-slim AS BUILDIMAGE

ARG GITHASH
ARG TIMESTAMP
ARG COMPONENT

#RUN apk --update add --no-cache build-base gcc abuild binutils make argp-standalone linux-headers git
RUN apt-get update && \
    apt-get install -y git gcc make binutils build-essential && \
    rm -rf /var/lib/apt/lists/*
RUN mkdir /tmp/build

COPY c /tmp/build/src

WORKDIR /tmp/build/src
RUN make distclean
# RUN LDFLAGS=-largp GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make dep lib
RUN GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make dep lib

WORKDIR /tmp/build/src/${COMPONENT}
# RUN LDFLAGS=-largp GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make all
RUN GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make all

WORKDIR /tmp/build

# ----------------------------------------------------------------------------

FROM debian:buster-slim

ARG GITHASH
ARG TIMESTAMP
ARG COMPONENT

# RUN apk --update add --no-cache bash
# RUN adduser -H -D "${COMPONENT}"
RUN apt-get update && \
    apt-get install -y psmisc procps lsof strace ltrace gdb && \
    rm -rf /var/lib/apt/lists/*
RUN adduser --no-create-home --disabled-password --disabled-login --gecos "Application user" "${COMPONENT}"
# RUN mkdir -p "/var/log/${COMPONENT}"
# RUN chown "${COMPONENT}:${COMPONENT}" "/var/log/${COMPONENT}"
COPY --from=BUILDIMAGE "/tmp/build/src/dist/bin/${COMPONENT}" "/opt/${COMPONENT}"
RUN chown "${COMPONENT}:${COMPONENT}" "/opt/${COMPONENT}" && \
    chmod og-rwx,u=rx "/opt/${COMPONENT}"

USER ${COMPONENT}

ENTRYPOINT [ "/opt/tail" ] 