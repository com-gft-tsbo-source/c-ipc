alias ms-random="/mnt/c/Users/tsbo/Projects/c_ipc/bin/ms-random --version 1.0.0 --name randomsvc --port 8081"
alias ms-measure="/mnt/c/Users/tsbo/Projects/c_ipc/bin/ms-measure --version 1.0.0 --name th-1 --port 8080 --type thermometer --address 01:00 --randomsvc http://127.0.0.1:8081/random"
alias server="/mnt/c/Users/tsbo/Projects/c_ipc/bin/server"
alias client="/mnt/c/Users/tsbo/Projects/c_ipc/bin/client"

alias droot="docker run -i -t --rm --privileged --pid=host debian nsenter -t 1 -m -u -n -i "
alias dils='docker image ls --filter "label=PROJECT=com.gft.techoffice.event-20211104"'
alias dcls='docker container ls --filter "label=PROJECT=com.gft.techoffice.event-20211104" '
alias dcr='docker container run --label="PROJECT=com.gft.techoffice.event-20211104" -i -t '
alias dexec='docker container exec -i -t '

alias ks1='kubectl --namespace step-1'
alias ks2='kubectl --namespace step-2'
alias ks3='kubectl --namespace step-3'

export PS1='\[\e]0;\u@\h: \w\]${debian_chroot:+($debian_chroot)}\[\033[00;32m\]y\u@\h\[\033[00m\] $(if [[ $? == 0 ]]; then echo "✓"; else echo "✗"; fi) : \[\033[00;34m\]$(realpath --relative-to=/mnt/c/Users/tsbo/Projects "\w")\[\033[00m\]\$ '
