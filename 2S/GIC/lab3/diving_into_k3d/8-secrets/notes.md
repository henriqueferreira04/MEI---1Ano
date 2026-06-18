## How to deploy a simple app with a service exposed through an ingress and multiple apps

In order to run this example you have to:
- build the containers
- push the containers to the registry
- adjust the namespace
- apply the deployment
- create the secret
- check the resulting deployment, service, ingress and secrets
- edit your /etc/hosts file adding an entry so that the cluster IP resolves to the ingress name
- check the result opening the browser at http://ingressname
