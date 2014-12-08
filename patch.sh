#!/bin/bash
cd /usr/src/
diff -rcP -X ignorelist.txt linux-pristine linux > project1-diff

