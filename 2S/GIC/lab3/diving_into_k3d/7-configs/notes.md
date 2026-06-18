## How to deploy a simple app with a service exposed through an ingress and multiple apps

In order to run this example you have to:
- build the containers
- push the containers to the registry
- adjust the namespace
- apply the deployment
- check the resulting deployment, service, ingress and config maps
- edit your /etc/hosts file adding an entry so that the cluster IP resolves to the ingress name
- check the result opening the browser at http://ingressname
- check the logs of the different pods
- check the config of the nginx pod. You can enter the pod shell to check it
