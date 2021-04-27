#!/bin/bash

tar -cjvf data.tar.bz2 $(find -size -10G -name "*soap")

