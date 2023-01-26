 dcr server:simple -t  mytopic
 dcr server:wrapped mytopic
 dcr --ipc=host server:wrapped mytopic server1
 dcr --ipc=host client:simple -t  mytopic
 dcr --ipc=host server:wrapped mytopic server2
