# i-have-no-clue Design Document

This file contains a basic outline of how the code will function to serve as a reference for those working on it.
Nothing in this document is fixed, static, or unchangeable by anyone working on this project.

## Project structure:

This project is broken up into two main parts - client and server.
As such, it is separated in two subfolders, client and server.
These can be both built with CMake.

## Unix build instructions

If you're on a Unix system, run `cmake -B build` to configure the build system.
Then, run `cmake --build build` to build all targets, or `cmake --build build --target <target>` to build only some.

### Available targets:

`client` - builds just client
`server` - builds just server

Both the client and server can be run independently.
The client executable is located in build/client-bin, and the server executable is located in build/server-bin.

## Client Architecture

### UI:

The UI will have two parts, the main play area which will represent the board and all the information usually on the
board in the physical game, and a separate window which will have a notepad which the player can keep track of their
information on. The button for making the final guess will be on the notepad window while the one for making an
information gathering guess will be in the main window.

## Data Structures:

The layout of the rooms will be stored as a tree data structure will all adjacent rooms being linked together.

## Server Architecture

The server listens on port 55555 and uses a protocol described below.

Each message in this protocol is delimited by a newline (CRLF).

## Protocol:

### On connection

Server will send to client all of the following messages in this exact order. Client may respond to messages at any point with its selected option.

```
comma-separated list of available characters to select from
list of cards in hand
turn index (first turn has 0 index),total number of players
```

Client will respond with its selected character.

When ready to start the game, a client must send a message with solely the text `START`.
When there are at least 3 players and all players have sent the `START` message, the game will begin.

Example messages (replace names of characters with the actual names, ofc):

Server:

```
Character1,Character2,Character3
Character5,Weapon1,Weapon6
2,5
```

Client:

```
Character3
```

### On player's turn

Server will send a message to client containing solely the text `TURN,YourCharacter`, with the character replaced with your character's name.

Client must then send a message containing a comma-separated list of the cards they want to ask for, followed by a message contianing position information.
The position information message will then be sent to all other clients, prefixed with the client's character name and a single comma.

Server will then respond with a message containing the player the card was taken from, a comma, followed by and the specific card that was found, or a message containing just the text `NONE` if none of the cards were found in anyone else's hand.

Client can then send either an `ACCUSE` message or a message containing just the text `ENDTURN`.

A client's guesses and accusations aren't checked to ensure that one card is a character, one is a weapon, and one is a location. That is done clientside (for now). Input validation may come later, but in any case the order in which characters, weapons, and locations are sent is unspecified.

Example:

Server:

```
YOURTURN
```

Client:

```
Character1,Weapon2,Location10\
somesortofpositioninformationidk
```

Server:

```
Character5,Location10
```

Client:

```
ACCUSE,Character2,Weapon1,Location2
```
or
```
ENDTURN
```

### On another player's turn

Server will send a message to client containing solely the text `TURN,Character1`, where Character1 is the name of the character whose turn it currently is.

Server will then send the client the updated position information of another player in one message.
Server will then send the client a message of either of the following forms:

```
CARDREQUEST,card1,card2,card3
```

or

```
CARDNOTICE,card1,card2,card3
```

The three cards listed after each message are the ones that the other player asked for.

If the client receives a `CARDREQUEST` message, they are expected to respond with one card of theirs that they wish to show to the person who asked for it. This message should be a single message solely containing the name of the card they wish to show.

If the client receives a `CARDNOTICE` message, it's possible that one of their cards was shown to the person who asked for it. They must manually check to see if they have any cards from the `CARDNOTICE` message if they want to display this information to the user. However, they SHALL NOT send a message containing the name of the card they want to show. The server already knows this information and it is pointless to ask for it.

If there's anything I forgot in this protocol please do let me know!
