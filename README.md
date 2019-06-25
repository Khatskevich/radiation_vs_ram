Count RAM errors
================

## Motivation
THere is a [paper](https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/35162.pdf) with some authors from google which states
> For example, we observe DRAM error rates that are orders of magnitude higherthan previously reported, with 25,000 to 70,000 errors per billiondevice hours per Mbit

Here is the number converted to something more readable:
```
70000 / 1E9 / 60^2 / 1024^2 = 1.854E-14 Errors / s / bit.
```
If your machine has 16Gb of ram, how many errors will happen per day in your machine?
```
1.854E-14 * 16E9 * 8 * 60^2 * 24 = 205.1
```
That looks horrible, becouse many errors can lead to a system crush.

This program is created to make you able to track if any errors happen in your RAM.

P.S. on my machine (60Gb without SSE) during 3 months of tests I was able to catch 0 errors.
