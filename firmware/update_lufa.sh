#!/bin/bash

#Delete existing LUFA library if it exists.
#Delete lufa clone if it exists.
rm -rf lib/LUFA lufa

#Clone entire repo.
git clone https://github.com/abcminiuser/lufa/

#Create the lib directory if it doesn't exist.
if [ ! -d lib ]; then mkdir lib; fi

#Only need the second-level LUFA directory.
mv lufa/LUFA lib/.

#Delete the remainder.
rm -rf lufa