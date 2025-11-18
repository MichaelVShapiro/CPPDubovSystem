# CPPDubovSystem

A simple pairing engine that generates pairings via the FIDE Swiss Dubov system.

## Table of Contents

- About
- Installation
- Use by Terminal
- Use by API
- Extra Notes
- Acknowledgements
- License

## About CPPDubovSystem

CPPDubovSystem (**current version 2.1.1**) is a framework for generating pairings using the FIDE Swiss Dubov algorithm. It is coded entirely in C++.

This framework follows all the rules written [here](https://handbook.fide.com/chapter/C040401202602). I reccomend for you to [see also](http://www.rrweb.org/javafo/C04.pdf). Please note that this has **NOT YET BEEN APPROVED** by FIDE, however it will be sent to FIDE at one point for approval. Follow this repository for updates!

## Installation

The easiest way to install CPPDubovSystem is to download this github repository, and compile it. Make sure you have g++ installed (or if you are on Mac, the clang++ command that comes standard with Xcode), and run the following in a terminal below. You are essentially going to run the build.sh file that is in the same directory as this README (so make sure to cd into this project in your terminal).

```console
user@user:~$ chmod +x build.sh; ./build.sh
```

Alternatively, you can use CMake to install this framework:

```console
user@user:~$ mkdir build
user@user:~$ cd build
user@user:~$ cmake ..
user@user:~$ make -j4
```

Please note: this framework was only tested on MacOS, and no other platform. Do know however that generally speaking, this should work on any system. It doesn't require any external framework or library to work (or use anything native to MacOS in specific). I have tested this framework on my M1 Mac with Xcode 16.2.

## Use by Terminal

The best way to get the pairings is by executing the data via terminal.

### Standard Pairings

To get pairings for a round, execute the following:

```console
user@user:~$ ./CPPDubovSystem --pairings "path/to/tournament_data.trf"
```

The framework accepts a file coded in [TRF16](https://www.fide.com/FIDE/handbook/C04Annex2_TRF16.pdf) as per FIDE requirements.

**Important Note**: the pairing engine **needs to know how many rounds your tournament has**. Therefore, in your TRF file, you must include the following code **TNR _round\_count_** in order for the pairing engine to capture the number of rounds in the tournament. See one of the example files in the "tests" folder. Alternatively, you can use the code **XXR _round\_count_** to set the total number of rounds in your tournament (this works similarly to JaVaFo and bbpPairings).

If you would like to invoke [Baku acceleration](https://handbook.fide.com/chapter/C0405202507), make sure to set the "ACC" code in the TRF file, and set the value to "true" or "1" (see an example in "DubovSystem/tests/baku_test.trf"). If you do not intend to use acceleration, you don't have to include the "ACC" code.

If you would like to signal the pairing engine that a player requests a bye (or simply not to pair it for the round) just add the following code in the TRF file: **BYE [_ID_]**. Obviously you would replace the "[ID]" part with the ID (or the pairing number/starting rank) of the player you are attempting not to pair. Alternatively, you can use the code **XXZ [_ID-LIST_]** which works similarly to how JaVaFo would parse it.

To make forbidden pairs (or pairing restrictions), simply add the TRF code "FOR" followed by the two players you are trying to make the pairing restriction. For instance, assume Player 1 (with ID 1) has a pairing restriction to Player 2 (with ID 2). You would write down in the TRF file "FOR 1 2" to create a pairing restriction for both of those players. You can also use the code "XXP" which works similarly to how JaVaFo would parse a pairing restriction.

Note that "FOR" and "BYE" doesn't support a list of players. So if you have multiple players with byes for instance, you have to write down the "BYE" and/or "FOR" code multiple times. See an example of using "BYE" and "FOR" in "DubovSystem/tests/parsing_test.trf."

### Outputing Standard Pairings to File

This framework can also output the pairings generated to a file. Run the command below to achieve this.

```console
user@user:~$ ./CPPDubovSystem --pairings "path/to/tournament_data.trf" --output "path/to/output.csv"
```

The data outputted by the framework is in CSV format. There are only two columns in the file, "White" and "Black." For each column, the players ID (or starting rank passed in) is outputted respectively. Any players who recieved a PAB (pairing allocated BYE) will have their opponent ID marked as -1.

Note that any players which you signaled not to pair, will not be outputted in the file. The pairing engine is designed to only output players in the CSV file which are active for the round.

### Random Tournament Generator

You can also use this engine to generate a random tournament. FIDE defines these random tournament generators as simply an engine that creates a tournament, and simulates it with random game results. Run the command below as follows to achieve this:

```console
user@user:~$ ./CPPDubovSystem --rtg "path/to/trf.trf" --p_count 10 --rtg_rounds 5
```

The sample command above creates a random tournament with 10 players and 5 rounds. It will output this all into a trf file (TRF16 encoded). If a round cannot be completed for whatever reason (which is highly unlikely to happen), the program will output all the rounds it was able to generate until the round it could not generate pairings for.

_Please note that sometimes, a round or two could have pairings done incorrectly (when executing the random tournament generator). Make sure to always check those rounds with the free pairings checker to make sure that the pairings are correct. This is simply a bug I noticed that rarely comes up in the RTG, and I am still working out a solution to fix it._

### Free Pairings Checker

This framework can also check your pairings to see how they compare to the internal ones generated. It will output both the internal generated one and the one to compare, and show any distinctions between the two.

Execute the following command to use the pairings checker

```console
user@user:~$ ./CPPDubovSystem --fpc "path/to/trf.trf" --fpc_rounds 3
```

Once again, we pass in a TRF16 encoded file, and we pass in what round number to check. In our example, we are asking the engine to check round 3.

### Other commands

Here is a breif list of other commands you can execute:

|Command  |Description      |
|---------|-----------------|
|--help   |Standard help    |
|--version|Output version   |
|--sample |Example usage    |

## Use by API

If you do not want to use this by terminal, you can also try executing with C++ code.

### Standard Pairings

```cpp
/**
* This is a demonstration of how to get pairings by API
*/

#include <iostream>
#include <vector>
#include "Tournament.hpp" // we need this to work with tournaments
#include "Player.hpp" // we also need this to add players

int main() {
    // first make our tournament
    CPPDubovSystem::Tournament our_tournament(5); // we pass in 5 for 5 rounds
    
    // now we should add some players
    CPPDubovSystem::Player player1("Player 1", 1200, 1, 0.0); // we make a player object with a name, rating, ID, and points
    CPPDubovSystem::Player player2("Player 2", 1100, 2, 0.0);
    CPPDubovSystem::Player player3("Player 3", 1000, 3, 0.0);
    CPPDubovSystem::Player player4("Player 4", 900, 4, 0.0);
    
    our_tournament.addPlayer(player1); // this is how to add the player to the tournament
    our_tournament.addPlayer(player2);
    our_tournament.addPlayer(player3);
    our_tournament.addPlayer(player4);
    
    // now to get the pairings
    std::vector<CPPDubovSystem::Match> the_pairings = our_tournament.generatePairings(1); // we pass in the round to generate pairings for
    
    for(int i = 0; i < the_pairings.size(); i++) {
        CPPDubovSystem::Match the_match = the_pairings[i];
        std::cout << "White: " << the_match.white.getName() << std::endl;
        if(the_match.is_bye) {
            std::cout << "BYE\n";
        } else {
            std::cout << the_match.black.getName() << std::endl;
        }
    }
    
    return 0;
}
```

In some cases you may need to add more info to player data, such as opponents played. Here is a demonstration below of how to do this.

```cpp
/**
* This demonstration shows how to add info to the player such as color history
*/

#include <iostream>
#include "Player.hpp"

int main() {
    CPPDubovSystem::Player example("EXAMPLE", 2000, 1, 1.0);
    CPPDubovSystem::Player example2("EXAMPLE2", 1000, 2, 0.0);
    CPPDubovSystem::Player example3("EXAMPLE3", 900, 3, 1.0);
    
    // add opponent history
    example.addOpp(2); // we pass in the opponent id here, which in our example is 2. We basically just told the program that player EXAMPLE played whoever has the id of 2
    example2.addOpp(1); // we just marked EXAMPLE2 played EXAMPLE
    example.addOppRating(1000); // it is also VERY important to pass in the opponents ratings played
    example2.addOppRating(2000);
    
    // add color history
    example.addColor(CPPDubovSystem::Color::WHITE); // we just said that EXAMPLE already played as white
    example2.addColor(CPPDubovSystem::Color::BLACK); // same thing for EXAMPLE2
    
    // adding bye info
    example3.setByeStatus(true); // we just marked EXAMPLE3 as the player who already got a bye
    
    // you should set setByeStatus to true if the player already got the bye or won by forfeit
    
    // you can play with upfloat history as well. In fact if a player upfloated, you should work with this
    example2.incrementUpfloat(); // increments by 1, the number of times the player upfloated in the whole tournament
    example2.setUpfloatPrevStatus(true); // if the player only upfloated in the previous round, this is required to set to true
    
    // to assign pairing restrictions, use addPairingRestriction
    example3.addPairingRestriction(1); // this forbids EXAMPLE3 from ever getting paired up with EXAMPLE

    return 0;
}
```

A word on **incrementUpfloat** and **setUpfloatPrevStatus**. If you are not familiar with the term "upfloater," this simply means someone who played another person who has a higher score than them. For instance, if Bob (who has 1.0 points) plays Joe (who has 2.0 points), Bob is the upfloater. Float history is extremely important here. For each round the player upfloats, you must signal it to the framework using **incrementUpfloat**. If the player upfloated just the round before the round you are attempting to pair, you must mark **setUpfloatPrevStatus** to true. Otherwise, mark it as false.

If a player is requesting a bye, or you do not wish to pair a player, simply do not include that player in the pairing selection.

### Applying Baku Acceleration

If you want to use baku acceleration, just add one more argument to generate pairings, and set it to "true." See the example below.

```cpp
#include <iostream>
#include "Tournament.hpp"

int main() {
    CPPDubovSystem::Tournament someTournament(5);
    // assume for this example players were already added to someTournament
    
    std::vector<CPPDubovSystem::Match> games = someTournament.generatePairings(2, true); // adding 'true' signals to the pairing engine that it should use baku acceleration
    
    return 0;
}
```

## Extra Notes

It is extremely important to note that as the FIDE rules say, you are required to **make sure all players are rated**. It actually says it in rule 1.1.1 (see rule book). If the pairing engine detects that a player is unrated, it will put a warning in the console and then generate pairings normally.

It is also noted in the FIDE rule book that the number of rounds the tournament has **must** be set. So remember that when using this framework through a terminal, make sure that the **TNR** code (or XXR) is in the TRF file.

Once again, this pairing engine has not yet been approved by FIDE. But I will send it to them for approval. I will update this README file once they have approved it.

For the first round, this pairing engine will automatically assign the color white to the top player. You can use the "XXC" TRF(x) code to override this (e.g. "XXC white1" OR "XXC black1"). The _Tournament_ class also has the function _setRound1Color_ which accepts a boolean value. If the value is set to true, white will be given to the top player in round 1.

Here is a list of error codes the framework can give you:

- 0 (success)
- -1 Pairings cannot be completed for some reason (this can be for any reason, like everyone already played everyone in the tournament)
- 1 Malformed TRF file (the data cannot be read). You will actually find that an error will be thrown for this one
- 2 Missing tournament number of rounds
- 3 Tournament complete (a new round is attempted to be paired, but all the rounds are complete)

Also note that upon pairing error (error code = -1) CPPDubovSystem will not attempt to repair anybody. It will simply throw you an error

## Acknowledgements

As of CPPDubovSystem version 2.0, code was incorporated from the Minimum-Cost-Perfect-Matching repository [(https://github.com/dilsonpereira/Minimum-Cost-Perfect-Matching)](https://github.com/dilsonpereira/Minimum-Cost-Perfect-Matching), which was authored by dilsonpereira, and licensed under the MIT license. See "DubovSystem/graph util" in this project for the complete code of the repository mentioned, and a copy of the license.

I would also like to thank Roberto Ricca for helping me test this framework and validate the correctness of the pairings. This is a huge leap to hopefully getting it approved by FIDE and be used by other pairing programs! I thank you a lot for your help!

## License

CPPDubovSystem is open source! It is licensed under Apache 2.0 (see LICENSE file).
