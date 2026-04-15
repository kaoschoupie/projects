#!/usr/bin/env bash
# cleanup_netns.sh
set -euo pipefail

NS_A="ns_client"
NS_B="ns_server"

if [ "$EUID" -ne 0 ]; then
  echo "Run as root: sudo ./cleanup_netns.sh"
  exit 1
fi

ip netns del $NS_A 2>/dev/null || true
ip netns del $NS_B 2>/dev/null || true

echo "Namespaces removed."