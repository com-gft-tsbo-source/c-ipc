FROM debian:buster-slim AS BUILDIMAGE

ARG GITHASH
ARG TIMESTAMP
ARG COMPONENT

RUN apt-get update && \
    apt-get install -y git gcc make binutils build-essential && \
    rm -rf /var/lib/apt/lists/*
RUN mkdir /tmp/build

COPY c /tmp/build/src

WORKDIR /tmp/build/src
RUN make distclean
RUN GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make dep lib

WORKDIR /tmp/build/src/${COMPONENT}
RUN GITHASH=${GITHASH} TIMESTAMP=${TIMESTAMP} make all

WORKDIR /tmp/build

# ----------------------------------------------------------------------------

FROM debian:buster-slim

ARG GITHASH
ARG TIMESTAMP
ARG COMPONENT

RUN apt-get update && \
    apt-get install -y psmisc procps lsof strace ltrace gdb && \
    rm -rf /var/lib/apt/lists/*
RUN adduser --no-create-home --disabled-password --disabled-login --gecos "Application user" "${COMPONENT}"
COPY --from=BUILDIMAGE "/tmp/build/src/dist/bin/${COMPONENT}" "/opt/${COMPONENT}"
RUN chown "${COMPONENT}:${COMPONENT}" "/opt/${COMPONENT}" && \
    chmod og-rwx,u=rx "/opt/${COMPONENT}"

USER ${COMPONENT}

ENTRYPOINT [ "/opt/tail" ] 
