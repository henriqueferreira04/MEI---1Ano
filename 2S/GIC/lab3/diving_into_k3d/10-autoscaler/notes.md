## How to deploy a simple app with a service exposed through an ingress and multiple apps

In order to run this example you have to:
- build the containers
- push the containers to the registry
- adjust the namespace
- apply the deployment
- check the resulting deployment, service, ingress and pods
- edit your /etc/hosts file adding an entry so that the cluster IP resolves to the ingress name
- check the result opening the browser at http://ingressname
- check where the pods are actually located and how many pods are created
- open several windows
- check where the pods are actually located and how many pods are created

