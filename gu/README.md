# gu
The Gu grading utility.

A mini application that uses the some experimental ideas:

* a very simple three-stage csv parser, *shrdlu*, that
parses fields, records or entire csv documents.

* *Sheet*, a class for manipulating tables of strings. Sheets
come with an algebra. They can be stacked horizontally and
vertically. This makes it easy to build up complex text-based 
reports.

*Gu* compiles with g++5.1 and Boost 1.55. Just run ```make``` at
the command line.
