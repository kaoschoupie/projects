#!/usr/bin/env bash
# setup.sh
set -euo pipefail

NS_A="ns_client"
NS_B="ns_server"
VETH_A="vethA"
VETH_B="vethB"
IP_A="10.200.1.1/24"
IP_B="10.200.1.2/24"

# Require root
if [ "$EUID" -ne 0 ]; then
  echo "Run as root: sudo ./setup_netns.sh"
  exit 1
fi

# Clean any previous leftovers (ignore errors)
ip netns del $NS_A 2>/dev/null || true
ip netns del $NS_B 2>/dev/null || true
ip link del $VETH_A 2>/dev/null || true

# Create namespaces
ip netns add $NS_A
ip netns add $NS_B

# Create veth pair and move into namespaces
ip link add $VETH_A type veth peer name $VETH_B
ip link set $VETH_A netns $NS_A
ip link set $VETH_B netns $NS_B

# Configure interfaces and loopback
ip netns exec $NS_A ip addr add $IP_A dev $VETH_A
ip netns exec $NS_A ip link set $VETH_A up
ip netns exec $NS_A ip link set lo up

ip netns exec $NS_B ip addr add $IP_B dev $VETH_B
ip netns exec $NS_B ip link set $VETH_B up
ip netns exec $NS_B ip link set lo up

# Bring up both ends (already done) and show status
echo "Created namespaces: $NS_A ($IP_A) <-> $NS_B ($IP_B)"
echo "Interfaces:"
ip netns exec $NS_A ip -brief addr show
ip netns exec $NS_B ip -brief addr show

echo "To remove namespaces run: sudo ./cleanup_netns.sh"