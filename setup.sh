alias ms-random="/mnt/c/Users/tsbo/Projects/c_ipc/bin/ms-random --version 1.0.0 --name randomsvc --port 8081"
alias ms-measure="/mnt/c/Users/tsbo/Projects/c_ipc/bin/ms-measure --version 1.0.0 --name th-1 --port 8080 --type thermometer --address 01:00 --randomsvc http://127.0.0.1:8081/random"
alias server="/mnt/c/Users/tsbo/Projects/c_ipc/bin/server"
alias client="/mnt/c/Users/tsbo/Projects/c_ipc/bin/client"

alias droot="docker run -i -t --rm --privileged --pid=host debian nsenter -t 1 -m -u -n -i "
alias dils='docker image ls --filter "label=PROJECT=com.gft.techoffice.event-20211108"'
alias dcls='docker container ls --filter "label=PROJECT=com.gft.techoffice.event-20211108" '
alias dcr='docker container run --label="PROJECT=com.gft.techoffice.event-20211108" --rm -i -t '
alias dexec='docker container exec -i -t '

alias ks1='kubectl --namespace step-1'
alias ks2='kubectl --namespace step-2'
alias ks3='kubectl --namespace step-3'
