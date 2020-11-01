#bin/bash

cd folder1

if [[ ! -f file1 ]]
then
touch file1
fi

if [[ ! -f file2 ]]
then
touch file2
fi

if [[ ! -f file3 ]]
then
touch file3
fi

if [[ ! -d testdir1 ]]
then
mkdir testdir1
fi
if [[ ! -d testdir2 ]]
then
mkdir testdir2
fi
if [[ ! -d testdir3 ]]
then
mkdir testdir3
fi

cd testdir1
if [[ ! -f file11 ]]
then
touch file11
fi
if [[ ! -f file12 ]]
then
touch file12
fi
if [[ ! -d testdir11 ]]
then
mkdir testdir11
fi
cd testdir11
if [[ ! -f file111 ]]
then
touch file111
fi
cd ..
cd ..

cd testdir3
if [[ ! -f file31 ]]
then
touch file31
fi
if [[ ! -d testdir31 ]]
then
mkdir testdir31
fi
if [[ ! -d testdir32 ]]
then
mkdir testdir32
fi
cd testdir31
if [[ ! -d testdir311 ]]
then
mkdir testdir311
fi
if [[ ! -f file311 ]]
then
touch file311
fi
cd testdir311
if [[ ! -f file3111 ]]
then
touch file3111
fi
