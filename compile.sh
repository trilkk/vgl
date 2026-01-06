#!/bin/sh

DNLOAD="../dnload/dnload.py"
if [ ! -f "${DNLOAD}" ] ; then
  DNLOAD="../faemiyah-demoscene/dnload/dnload.py"
  if [ ! -f "${DNLOAD}" ] ; then
    DNLOAD="/usr/local/src/dnload/dnload.py"
    if [ ! -f "${DNLOAD}" ] ; then
      DNLOAD="/usr/local/src/faemiyah-demoscene/dnload/dnload.py"
      if [ ! -f "${DNLOAD}" ] ; then
        echo "${0}: could not find dnload.py"
        exit 1
      fi
    fi
  fi
fi

if [ ! -f "dnload.h" ] ; then
  touch dnload.h
fi

python3 "${DNLOAD}" -v "test.cpp" -o "test" $*
if [ $? -ne 0 ] ; then
  echo "${0}: compilation failed"
  exit 1
fi

exit 0
