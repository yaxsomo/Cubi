#!/bin/sh

if [ -d "RhobanMaple" ]; then
    cd Maple &&
    git pull
else
    git clone https://github.com/Rhoban/Maple.git --depth=1
fi
