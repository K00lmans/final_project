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

The server listens on port 12345 and uses a protocol described below.

Each message in this protocol is delimited by a newline (CRLF).

## Protocol:

The following section really *should* be written up as a BNF-style grammar, but I couldn't be bothered.

### Connection handshake

When the server is ready for a newly-connected player to select a player, it will send a message of the following form:

```
PLAYERS-TAKEN,player1name,player2name
```

where `player1name` and `player2name` are names of players.
This message is terminated with a CRLF-style newline, and the list may contain anywhere from 0-5 players.

The client should then respond by picking a valid player not present in this list.
Once they have, they can reply with a message as follows:

```
PLAYER-SELECT,playername
```

This message contains just one player.

The client should then wait for enough others to connect.
Once all necessary players have connected, the server will send the following messages in sequential order:

```
PLAYER-CARDS,card1,card2,...
PLAYERS-LIST,player1,player2,...
```

The `PLAYER-CARDS` message is followed by a comma-separated list of the specific client's cards.
The `PLAYERS-LIST` message is followed by a comma-separated list of all the players in the game.

After this, the game loop begins.

### Game loop

The game loop will loop until any of the following conditions occur:

- A player makes a successful accusation
- All players make unsuccessful accusations
- An error condition occurs (at which point all connections are terminated)

To start, the server will send the following to all clients:

```
TURN-START,playername
```

where `playername` is the name of the player whose turn it is.
If your name is not `playername`, responding is an error state and will likely terminate the connection.
If you are `playername`, you have several options.

At any point during your turn, you may choose to accuse, or to request to see a card. 
This can be done in any order.

#### Accusations

To accuse, send the following:

```
ACCUSE,card1,card2,card3
```

If your accusation was correct, all players will receive a message as follows:

```
GAME-END,yourname,Successful accusation!
```

The connection will then terminate.

If your accusation was incorrect, and everyone else has also made a false accusation, all players will receive the following:

```
GAME-END,All players have made a false accusation.
```

All connections will then terminate.


If your accusation was incorrect, but others can still make accusations, all players will receive the following:

```
ACCUSE-FAIL,yourname,card1,card2,card3
```

From this point forward, you will never receive a TURN-START message with your playername.
However, you may receive card requests, so keep your connection open.

#### Card requests

To request cards from another player, send the following message:

```
CARD-REQUEST,card1,card2,card3
```

This message will then be sent back to you and all other players, with some added info about who the cards were requested by and who must show their cards. That message will look as follows:

```
CARD-REQUEST,yourname,source-player,card1,card2,card3
```

Unless, of course, nobody else had any of your cards. In this case, you'll receive the following:

```
CARD-REQUEST-EMPTY,yourname,card1,card2,card3
```

You can request your own cards, however these will be ignored by the server; the server will never prompt you to show cards to yourself.
Once the other player has decided which card of theirs to show, the server will send you the following:

```
HAVE-CARD,source-player,card
```

where source-player is the player who had the card.

Finally, you must end your turn.
You must do this even if your accusation failed!
Send the following message:

```
END-TURN
```

After this, the loop will restart.

#### If it is not your turn

If it is not your turn, you will receive the `TURN-START`, `CARD-REQUEST`, `CARD-REQUEST-EMPTY`, `END-TURN`, `GAME-END`, and `ACCUSE-FAIL` messages described earlier.
If someone else set your name as `source-player` in the `CARD-REQUEST`, you must select one of your cards and respond with the following:

```
HAVE-CARD,dest-player,card
```

### Error conditions

If an error condition occurs, you may receive a message as follows:

```
ERR,error info string
```

If you receive this, a fatal error has occurred.
You may not receive this; the socket might just close.
This is because I haven't implemented these everywhere yet and don't really have time to before this is due.
Error coverage might get better in future!
