#!/bin/sh

EXTENSION=$1
if test -z $1; then
    printf "Potential error: filetype defaulting to PDF.\nPlease provide a parameter with your desired file extension.\nContinuing...\n"
    EXTENSION=pdf
fi

pandoc design_doc.md -o design_doc.$EXTENSION
pandoc server/README.md -o server/README.$EXTENSION
echo 'Docs compiled successfully.'
