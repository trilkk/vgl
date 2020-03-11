#!/bin/sh

DNLOAD="../dnload/dnload.py"
if [ ! -f "${DNLOAD}" ] ; then
  DNLOAD="../dnload/dnload.py"
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

python "${DNLOAD}" -v "test.cpp" -E $*
if [ $? -ne 0 ] ; then
  echo "${0}: regenerating symbols failed"
  exit 1
fi

exit 0
