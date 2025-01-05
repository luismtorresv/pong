#!/bin/sh

# Adapted from William Vinagre's "raysnake" project.
#
# URL: https://codeberg.org/whou/raysnake/src/branch/master/deploy_web.sh

PROJECT_ROOT=`pwd`
PROJECT_NAME=pong
BUILD_PATH=build
OUTPUT_PATH=$BUILD_PATH/pong

if ! type emcmake > /dev/null; then
	echo "emcmake isn't available, check if emscripten is activated correctly."
	exit 1
fi

git switch main

cd $BUILD_PATH
emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
emmake make

git switch pages

cd $PROJECT_ROOT
cp $OUTPUT_PATH/$PROJECT_NAME.html ./index.html
cp $OUTPUT_PATH/$PROJECT_NAME.js \
   $OUTPUT_PATH/$PROJECT_NAME.wasm \
   ./
   # $OUTPUT_PATH/favicon.ico \
   # $OUTPUT_PATH/icon.png \

git add --all

read -p 'Enter your commit message: ' msg

git commit -m "$msg"

read -p 'Push changes to upstream (y/n)? ' answer
if [ "$answer" != "${answer#[Yy]}" ] ;then
    git push
fi
