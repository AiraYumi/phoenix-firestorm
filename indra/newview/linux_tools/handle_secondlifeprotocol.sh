#!/bin/bash

# Send a URL of the form secondlife://... to the viewer.
#

URL="$1"

if [ -z "$URL" ]; then
    echo Usage: $0 secondlife://...
    exit
fi

RUN_PATH=`dirname "$0" || echo .`
cd "${RUN_PATH}/.."

exec ./firestorm -url \'"${URL}"\'

