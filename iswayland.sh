#!/usr/bin/bash
if [ "$(loginctl show-session $(loginctl user-status $USER | grep -E -m 1 'session-[0-9]+\.scope' | sed -E 's/^.*?session-([0-9]+)\.scope.*$/\1/') -p Type | grep -ic "wayland")" -ge 1 ]; then
    exit 1
else
	exit 0
fi
