bin/ms-random --port 8081
bin/ms-measure --type thermometer --address TH:01 --port 8080 --randomsvc http://127.0.0.1:8081/random


curl http://127.0.0.1:8080/measure


bin/server --topic my-topic-1 server-1

tail -f '/var/log/server/server-1#my-topic-1.log'


bin/client --topic my-topic-1 client-1


