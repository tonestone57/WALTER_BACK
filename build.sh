#!/bin/bash
set -ex
cd /app
if [ ! -d "generated" ]; then
	mkdir generated
fi
cd generated
/app/configure --build-cross-tools x86_64 --cross-tools-source /app/buildtools
