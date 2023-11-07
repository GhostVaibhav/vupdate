FROM alpine:latest

RUN apk update && apk add --no-cache
RUN apk add cmake make gcc g++ git

WORKDIR /app

COPY . /app

ENTRYPOINT [ "sh" ]
