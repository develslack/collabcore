#!/bin/bash
fecha=`date +%d-%m-%Y`
archivo="queries/dumps/collab_core-$fecha.sql"
mysqldump --user=root --password=slack142 --host=localhost --routines --triggers collab_core > $archivo
chmod 777 $archivo



