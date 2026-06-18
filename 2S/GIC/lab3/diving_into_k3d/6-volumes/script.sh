#!/bin/bash
total=0
success=0
while true; do
total=$((total+1))
# Replace <SERVICE_NAME> with the actual K3D Ingress
if curl -s -o /dev/null -w "%{http_code}" http://app-namespace.k3d | grep -q "200"; then
success=$((success+1))
echo -ne "Request OK | "
else
echo -ne "Request FAILED | "
fi
sli=$(( success * 100 / total ))
echo "Current SLI (Success Rate): $sli% (Target: >90%)"
sleep 1
done