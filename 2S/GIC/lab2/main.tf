terraform {
  required_providers {
    vagrant = {
      source  = "bmatcuk/vagrant"
      version = "4.1.0"
    }
    k3d = {
      source  = "sneakybugs/k3d"   # or "nikhilsbhat/k3d"
      version = "1.0.1"            # check latest compatible version
    }
  }
}

# Define the Vagrant Resource
resource "vagrant_vm" "docker_node" {
  vagrantfile_dir = "." # Looks for a Vagrantfile in the same folder
  # Optional: Pass environment variables to the Vagrantfile if needed
  env = {
    VAGRANT_DEFAULT_PROVIDER = "virtualbox"
  }
}

# Use a Provisioner to install Docker once the VM is up
resource "null_resource" "install_docker" {
  depends_on = [vagrant_vm.docker_node]

  provisioner "remote-exec" {
    inline = [
      "sudo apt-get update",
      "sudo apt-get install -y apt-transport-https ca-certificates curl software-properties-common",
      "curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -",
      "sudo add-apt-repository -y 'deb [arch=amd64] https://download.docker.com/linux/ubuntu jammy stable'",
      "sudo apt-get update",
      "sudo apt-get install -y docker-ce",
      "sudo usermod -aG docker $USER",
      "sudo cp /vagrant/etc/daemon.json /etc/docker/daemon.json",
    ]

    connection {
      type        = "ssh"
      user        = "vagrant"
      # Terraform needs the specific SSH key Vagrant generates
      private_key = file(".vagrant/machines/default/virtualbox/private_key")
      host        = "127.0.0.1"
      port        = 2222 # Standard Vagrant SSH port
    }
  }
}


resource "k3d_cluster" "example" {
  name       = "gic-cluster"
  k3d_config = <<EOF
apiVersion: k3d.io/v1alpha4
kind: Simple

# Expose ports 80 via 8080 and 443 via 8443.
ports:
  - port: 3080:80
    nodeFilters:
      - loadbalancer
  - port: 3443:443
    nodeFilters:
      - loadbalancer

registries:
  create:
    name: dev
    hostPort: "5000"
EOF
}