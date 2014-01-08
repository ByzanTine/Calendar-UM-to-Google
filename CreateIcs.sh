#!/bin/bash
g++ -o h2c h2c.cpp -std=c++0x
python calendar-um-google.py
./h2c output
mv output.ics schedule.ics
rm output
