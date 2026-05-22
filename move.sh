#!/bin/sh

if [ ! -d bin/module ]
then
    mkdir bin/module
else
    unlink bin/flz_agent
    unlink bin/module/libflz_agent.so
fi

cp flz_server/bin/flz bin/flz_agent
cp lib/libflz_agent.so bin/module/
