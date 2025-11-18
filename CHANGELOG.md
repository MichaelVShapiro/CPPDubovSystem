#  CPPDubovSystem Changelog

## 2.1.1
### November 18, 2025

- Fixed the order of how pairings were generated (issue #4). Updated to follow FIDE guidelines on how pairings should be sorted as given in article 3.6 here: [https://handbook.fide.com/chapter/GeneralHandlingRulesForSwissTournaments202602](https://handbook.fide.com/chapter/GeneralHandlingRulesForSwissTournaments202602)

## 2.1
### October 29, 2025

- Fixed a few compilation issues on Linux (issue #1, Add CMake, fix Linux build)
- Added CMake as another way to build the project (issue #1, Add CMake, fix Linux build)
- Added support for TRF(x) "XXR" code (issue #2, Compatibility with TRF(x))
- Added support for TRF(x) "XXC" code
- Added support for TRF(x) "XXZ" code
- Added support for TRF(x) "XXP" code

Thank you to [@pierotofy](https://github.com/pierotofy) for making compilation fixes for the Linux build (issue #1) along with including a CMakeLists file, and [@Moritz72](https://github.com/Moritz72) for the suggestion of adding XXR code in the TRF(x) file (issue #2)!

## 2.0
### February 22, 2025

- Bug fix: the random tournament generator was not outputting the data into the TRF file correctly
- Bug fix: forfeit wins/losses were not being recorded properly
- Bug fix: fix when dealing with upfloaters. Some upfloaters were not being removed properly
- Bug fix: A few memory leaks were fixed
- New feature: it is now possible to add pairing restrictions to opponents
- New feature: it is now possible to mark which players not to pair in the TRF files
- New feature: Baku acceleration made available (see [https://handbook.fide.com/chapter/C0405202507](https://handbook.fide.com/chapter/C0405202507))!
- New feature: it is now possible to output pairings generated to a file
- New feature: free pairings checker can now output which pairings don't match up
- Time complexity improvement: The rule book suggests that a few places use a brute-force approach when some pairings don't work out (this occurs in some rare cases). All difficult pairing situations now make use of minimum weight perfect matching
- Time complexity improvement: Sometimes, player due color and due color strength need to be frequently called. The functions were updated to act in a way where the first time it is being called, the time complexity is O(n) (where n is the number of colors in the color history), after that, it will be O(1)
- Time complexity improvement: Sometimes, the ARO (average rating of opponent) of a player needs to be called frequently. Like the fix mentioned above, instead of executing this multiple times (at O(n) time where n is the number of opponents played), the value of the ARO is only computed once, and from there, getting the ARO takes O(1) time.
- General improvement: parts of the framework were updated to reflect rules more accurately (some pairings were incorrect before)
- General improvement: documentation has been highly improved
- General improvement: the output table pairings for a round generated looks nicer in the terminal
- General improvement: put Tournament, Player, baku, and LinkedList related classes under the namespace _CPPDubovSystem_
- General improvement: removed the MatchColor namespace, and made Color and ColorPreference enums under the CPPDubovSystem namespace

## 1.0
### May 27, 2024
- Initial Release
