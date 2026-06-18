#!/bin/bash

docker ps > /dev/null 2>&1
if [ $? -gt 0 ]; then
	echo "Docker not found! Please install it using your package manager"
	exit
fi

if [ ! -f /usr/local/bin/kubectl ]; then
	echo "Downloading kubectl"
        curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"
	if [ ! -f kubectl ]; then
		echo "Error downloading kubectl"
		exit
	fi
	echo "Your credentials are required to move kubectl to /usr/local/bin"
	sudo mv kubectl /usr/local/bin/
	sudo chmod +x /usr/local/bin/kubectl
fi


echo "This script will delete ALL k3d clusters!"
read -p "Press ENTER to continue, or CTRL-C to exit"

k3d cluster rm --all && k3d registry rm --all

echo "$HOME/.k3d/volumes will be used to store persistent volumes"
if [ ! -d $HOME/.k3d ]; then
	mkdir $HOME/.k3d
	mkdir  $HOME/.k3d/volumes
fi

echo "Creating Cluster Registry"
k3d registry  create myregistry.localhost --port 5000
echo "Creating Cluster"
k3d cluster create local --servers 1 --agents 3  --registry-use myregistry.localhost:5000 -p "80:80@loadbalancer" --volume "$HOME/.k3d/volumes":/volumes

echo "WARNING: Before you can push new images to docker, you need to:"

echo '- Add "127.0.1.1 k3d-myregistry.localhost" to /etc/hosts'
echo '- Add "insecure-registries" : [ "k3d-myregistry.localhost:5000" ] to /etc/docker/daemon.json'
echo '- Restart Docker with systemctl restart docker'
echo ''
echo 'You will be able to use "k3d-myregistry.localhost:5000" as your local registry'

read -p 'Press ENTER when ready'

echo "Installing the Kubernetes HeadLamp"
kubectl apply -f https://raw.githubusercontent.com/kubernetes-sigs/headlamp/main/kubernetes-headlamp.yaml
sleep 20
pkill -9 -f kubectl
kubectl port-forward -n kube-system service/headlamp 8080:80 &

kubectl -n kube-system create serviceaccount headlamp-admin
kubectl create clusterrolebinding headlamp-admin --serviceaccount=kube-system:headlamp-admin --clusterrole=cluster-admin
echo "Your Token for Headlamp is:"
kubectl create token headlamp-admin -n kube-system

echo "Go Here: http://127.0.0.1:8080"
echo "\nAll Done"
