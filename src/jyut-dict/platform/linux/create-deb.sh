#!/bin/bash

cd ../..

tar -cvzf ../jyut-dict_0.20.0525.tar.gz * --overwrite

cd ../

rm -rf jyut-dict_0.20.0525
mkdir jyut-dict_0.20.0525

cd jyut-dict_0.20.0525

tar -xvzf ../jyut-dict_0.20.0525.tar.gz --overwrite

cp -r ../jyut-dict/platform/linux/debian ./debian

rm ./dict.db
rm ./jyut-dict

dh_make -c mit -s -f ../jyut-dict_0.20.0525.tar.gz -p jyut-dict_0.20.0525

debuild