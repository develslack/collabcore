#!/bin/bash

clear

fecha=`date +%d-%m-%Y`

echo "========================================================================"
echo "Ultimo commit realizado..."
git log -1

echo "========================================================================"
echo "Rama en la que se está parado..."
git branch

echo "========================================================================"
echo "Ramas existentes..."
git branch -a

echo "========================================================================"
echo "Seleccione una Rama..."
read branch_selected 

echo "========================================================================"
echo "Ingrese el nro de commit a realizar: "
read commit_number
echo "========================================================================"
echo "Ahora ingrese el texto para el commit NRO: $commit_number : "
read commit
echo "========================================================================"
echo "Realizando Dump de la base de datos..."
./dump_data_base.sh

git add .
git commit -m "commit #$commit_number [ $commit ] - [ $fecha ]"
git push -u origin $branch_selected
