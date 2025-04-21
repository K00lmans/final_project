#!/bin/sh

EXTENSION=$1
if test -z $1; then
    printf "Potential error: filetype defaulting to PDF.\nPlease provide a parameter with your desired file extension.\nContinuing...\n"
    EXTENSION=pdf
fi

mkdir -p docs/server
pandoc design_doc.md -o docs/design_doc.$EXTENSION
pandoc server/README.md -o docs/server/README.$EXTENSION
pandoc README.md -o docs/README.$EXTENSION
echo 'Docs compiled successfully.'
