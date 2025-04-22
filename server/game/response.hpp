#pragma once

//
// Maintainer: Athena Boose
//
// Description: An enum encapsulating different possible response types.
//

enum class Response {
    Ack,
    TurnEnd,
    GetCards,
    Accuse,
    None,
};
