## How to deploy a simple app with a service exposed through an ingress and multiple apps

In order to run this example you have to:
- build the container
- push the container to the registry
- adjust the namespace
- apply the deployment
- check the resulting deployment, service and ingress
- edit your /etc/hosts file adding an entry so that the cluster IP resolves to the ingress name
- check the result opening the browser at http://ingressname
- check the number provided by the app. It should change, showing that multiple instances are being used
