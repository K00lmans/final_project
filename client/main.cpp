/*
 *Athena Boose, Kodiak O'Masta, Ryan Gradinaru
 *Created for CptS 122
 *Created 3-28-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 *
 *This program is dedicated to my (Kodiak's) dad, who in 1981 was working towards a degree in computer science. The
 *final project for the class he was taking at the time was to program a game of Clue™. Of course, since it was "Ye
 *Olden Times" he had no fancy languages to do it in, and instead was required to do it entirely in assembly (the
 *horror!). He was able to finish the project, but left with some less than positive feelings about the game. Of course,
 *that would not be the end of it, as his (now) ex-wife was also working towards the same degree and was a year behind
 *him. Now, she was not nearly as good as my dad and so after a full semester of him helper her, she somehow convinced
 *him to do the final project for her. He now, once again, had to program Clue™ in assembly, except this time he had to
 *do it in a completely different way to avoid plagiarizing himself. The sum total of these experiences had left him
 *with a deep disdain for Clue™ that has lasted to this day, and has become a running gag in my family. As such, I felt
 *that there could be no more appropriate choice for what to code for this final project, and as such this code is
 *dedicated to his struggles with this game.
 */

#include "main.h"

int main() {
    screen_size = sf::VideoMode::getDesktopMode().size;

    // Game setup
    Board clue_board = getBoardFromFile(); // Can this not just be a constructor? -Kodiak
    unique_ptr<Player> players[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}; // I have an irrational love of c arrays
    pick_characters(players);
    int current_player = generate_random_int(0, 5); // Picks the starting player
    int prior_player = -1;

    // Window setup
    // Creates the window with the size of the image, this makes the world coordinates of the window line of with the
    // pixels of the image for easy referencing
    auto main_game_window = sf::RenderWindow(sf::VideoMode({static_cast<unsigned>(background_size[0]),
        static_cast<unsigned>(background_size[1])}), "Clue", sf::Style::Close);
    // Then resizes the window dynamically to fit on the screen of the user
    main_game_window.setSize({get_window_x_size(), static_cast<unsigned>(screen_size.y * window_scaler)});
    // Then, since that may move the window off the screen, move it back to the center of the screen
    main_game_window.setPosition({static_cast<int>(screen_size.x / 2 - main_game_window.getSize().x / 2),
        static_cast<int>(screen_size.y / 2 - main_game_window.getSize().y / 2)});
    background_image->setSmooth(true); // Can cause weird issues but should be fine since it is the background
    const auto background = sf::Sprite(*background_image);

    std::unique_ptr<sf::Sprite> board_squares[24][25];
    setup_board_sprites(board_squares);
    for (int player = 0; player < 6; player++) {
        change_sprite_texture(*board_squares[players[player]->position.getX()][players[player]->position.getY()],
            *character_textures[player]);
    }
    auto room_containers = set_up_room_containers();

    Scratch_Pad::clear_data(); // Emptys old data
    unique_ptr<Scratch_Pad> current_users_pad;


    // Main game loop
    std::vector<Tile> tiles_in_reach; // Created here for use in full main game loop scope
    while (main_game_window.isOpen()) {

        // Game maintenance
        if (!players[current_player]->playing) { // Skips over NPC's
            prior_player = current_player; // Prevents it from freezing up if there is only one player
            current_player = (current_player + 1) % 6;
        }
        else if (current_player != prior_player) { // This means a new turn has rolled over
            prior_player = current_player;
            current_users_pad = make_unique<Scratch_Pad>(screen_size, current_player + 1);
            if (const auto current_room = get_current_room(players[current_player]->position, clue_board)) {
                tiles_in_reach = current_room->getExits();
                if (const int movement_left = generate_random_int(0 ,5)) {
                    std::vector<Tile> additional_tiles;
                    for (const auto &tile : tiles_in_reach) {
                        for (const auto &new_tile: find_reachable_tiles(tile, movement_left, clue_board)) {
                            additional_tiles.push_back(new_tile);
                        }
                    }
                    for (const auto &new_tile: additional_tiles) {
                        tiles_in_reach.push_back(new_tile);
                    }
                }
            } else {
                // This is an incredibly expensive operation so it should be run as little as possible
                tiles_in_reach = find_reachable_tiles(players[current_player]->position,
                    generate_random_int(1, 6), clue_board); // This has to be at least O(n^n)
            }
            for (const auto &tile: tiles_in_reach) {
                // Don't need to worry about overwriting player sprite since the code automatically ignores tiles with a
                // player in it
                change_sprite_texture(*board_squares[tile.getX()][tile.getY()], *selection_texture);
            }
        }

        // Handle interactivity
        while (const std::optional event = main_game_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                current_users_pad.reset();
                main_game_window.close();
            } else if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                for (const auto &tile: tiles_in_reach) {
                    if (board_squares[tile.getX()][tile.getY()]->getGlobalBounds().contains(
                        main_game_window.mapPixelToCoords(mouse->position))) {
                        if (const auto current_room = get_current_room(players[current_player]->position,
                            clue_board)) {
                            room_containers[room_numbers[current_room->getName()]]->
                            remove_player_from_seat(character_textures[current_player].get());
                        } else {
                            change_sprite_texture(*board_squares[players[current_player]->position.getX()]
                                [players[current_player]->position.getY()], *empty_texture);
                        }
                        clue_board.deleteToken(players[current_player]->characterToken,
                                players[current_player]->position);
                        players[current_player]->position = tile;

                        bool entered_room = false;
                        for (const auto &room: clue_board.getRooms()) {
                            for (const auto &door: room.getDoors()) {
                                if (players[current_player]->position == door) {
                                    entered_room = true;
                                    players[current_player]->position = room.getChair();
                                    goto searched_rooms; // The sacred goto, passed down for generations
                                }
                            }
                        }

                        searched_rooms:

                        if (entered_room) {
                            room_containers[room_numbers[get_current_room(players[current_player]->position,
                                clue_board)->getName()]]->add_player_to_seat(*character_textures[current_player]);
                        } else {
                            change_sprite_texture(*board_squares[tile.getX()][tile.getY()],
                                *character_textures[current_player]);
                            remove_item_from_vector(tiles_in_reach, tile); // Sets up for reset
                        }
                        for (const auto &unused_tile: tiles_in_reach) { // Resets textures
                            if (&board_squares[unused_tile.getX()][unused_tile.getY()]->getTexture() ==
                                selection_texture.get()) {
                                change_sprite_texture(*board_squares[unused_tile.getX()][unused_tile.getY()],
                                    *empty_texture);
                            }
                        }
                        clue_board.placeToken(players[current_player]->characterToken,
                            players[current_player]->position);
                        current_player = (current_player + 1) % 6; // Advance to next turn
                        break;
                    }
                }
            }
        }

        // Refresh and draw
        main_game_window.clear();
        main_game_window.draw(background);
        for (const auto &rows: board_squares) {
            for (const auto &squares: rows) {
                main_game_window.draw(*squares);
            }
        }
        for (const auto &room_container: room_containers) {
            room_container->draw(main_game_window);
        }
        main_game_window.display();
        if (current_users_pad != nullptr) {
            current_users_pad->update();
        }
    }

    return 0;
}
