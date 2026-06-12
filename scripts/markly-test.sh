#!/usr/bin/env bash
# markly-test.sh — send one command to a running offscreen markly test bridge and
# print its reply. App must be launched with MARKLY_TEST_SOCKET=<path>.
#   MARKLY_TEST_SOCKET=/tmp/mk.sock markly-test.sh dialog image
# Commands: dialog <kind> | content <kind> | viewmode <mode> | insert <text>
#           | text | shot <path> | wait <ms> | quit
set -euo pipefail
SOCK="${MARKLY_TEST_SOCKET:-/tmp/markly-test.sock}"
[ -S "$SOCK" ] || { echo "no socket at $SOCK" >&2; exit 1; }
CMD="$*"
if command -v socat >/dev/null; then
  printf '%s\n' "$CMD" | socat -t3 - "UNIX-CONNECT:$SOCK"
else
  python3 - "$SOCK" "$CMD" <<'PY'
import socket, sys
s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM); s.settimeout(3); s.connect(sys.argv[1])
s.sendall((sys.argv[2] + "\n").encode()); print(s.recv(65536).decode().strip())
PY
fi
