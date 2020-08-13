#!/bin/bash

git submodule deinit -f .
git submodule init
git submodule update

cd esp-idf/

git checkout 4aa1058e8a8f7f3fb17d9ac1158227ad161f2996

cp ../def_config/.gitmodules  .

git submodule deinit -f .
git submodule init
git submodule update

echo -e "config OK!!!"
	
cd ../
