#  CPPDubovSystem Changelog

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
