#!/bin/bash

cd ../..

tar -cvzf ../jyut-dict_0.19.0614.tar.gz * --overwrite

cd ../

rm -rf jyut-dict_0.19.0614
mkdir jyut-dict_0.19.0614

cd jyut-dict_0.19.0614

tar -xvzf ../jyut-dict_0.19.0614.tar.gz --overwrite

cp -r ../jyut-dict/platform/linux/debian ./debian

rm ./eng.db
rm ./jyut-dict

dh_make -c mit -s -f ../jyut-dict_0.19.0614.tar.gz -p jyut-dict_0.19.0614

debuild