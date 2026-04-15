#!/usr/bin/env bash
set -euo pipefail

# Edit these if your names differ
NS_SERVER="ns_client"
IFACE="vethA"
LOSS="25%"
CORRELATION="25%"

if [ "$EUID" -ne 0 ]; then
  echo "Run as root: sudo $0" >&2
  exit 1
fi

# Safe apply: remove any existing qdisc then add netem
ip netns exec "$NS_SERVER" tc qdisc del dev "$IFACE" root 2>/dev/null || true
ip netns exec "$NS_SERVER" tc qdisc add dev "$IFACE" root netem loss "$LOSS" "$CORRELATION"

echo "Netem applied on $NS_SERVER:$IFACE -> loss $LOSS correlation $CORRELATION"
echo "To remove netem run:"
echo "  sudo ip netns exec $NS_SERVER tc qdisc del dev $IFACE root"