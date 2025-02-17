#!/bin/env sh
graph-easy --from vcg --as dot --input "$1" --output "$1.dot"
graphviz2drawio "$1.dot"


