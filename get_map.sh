#!/usr/bin/env bash

objfile="$1"
xxd_exec="xxd"

if ! $xxd_exec -h 2>/dev/null >/dev/null || { [ -n "$USE_UTILS" ] && [ "$USE_UTILS" == "y" ]; }; then
    __script_dir="$(dirname "$(readlink -f "$0")")"
    xxd_exec="$__script_dir/utils/xxd"
fi

objdump -t "$objfile" | grep '\.text' | awk '{ print $1, $NF }' | sort | $xxd_exec -i -n sym_map
