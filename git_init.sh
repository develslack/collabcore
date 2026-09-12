#!/bin/bash

echo "# collabcore" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin https://github.com/develslack/collabcore.git
git push -u origin main
