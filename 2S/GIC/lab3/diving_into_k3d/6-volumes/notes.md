## How to deploy a simple app with a service exposed through an ingress and multiple apps

In order to run this example you have to:
- build the containers
- push the containers to the registry
- adjust the namespace
- apply the deployment
- check the resulting deployment, service and ingress
- edit your /etc/hosts file adding an entry so that the cluster IP resolves to the ingress name
- access the webpage
- apply the storage definition file
- use `kubectl cp origin pod:/volume_path` to copy files to the volume inside the pod
- access the webpage
