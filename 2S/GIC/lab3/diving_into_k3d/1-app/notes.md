## How to deploy a simple app

In order to run this example you have to:

- adjust the namespace: kubectl create namespace NAMESPAE_NAME
- build the container: docker build -t k3d-myregistry.localhost:5000/app:v1
- push the container to the registry: docker push k3d-myregistry.localhost:5000/app:v1
- apply the deployment: kubectl apply -f deployment.yaml
- check the result: kubectl -n NAMESPACE_NAME get pods
