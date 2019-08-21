# machowriter

[![Linux and macOS Build Status](https://travis-ci.com/paulhuggett/machowriter.svg?branch=master)](https://travis-ci.com/paulhuggett/machowriter) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=paulhuggett_machowriter&metric=alert_status)](https://sonarcloud.io/dashboard?id=paulhuggett_machowriter)

A tool for writing a Mach-O binary which can be executed by macOS.

At the moment the program simply writes a tiny executable program (which does nothing at all) to the supplied path. Tested on macOS 10.14.4.

~~~~bash
$ machowriter a.out
$ ./a.out
$
~~~~
