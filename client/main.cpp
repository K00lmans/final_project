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

    auto background_image = sf::Texture("client/graphics/clue_board.jpg");
    background_image.setSmooth(true); // Can cause weird issues but should be fine since it is the background
    const auto background = sf::Sprite(background_image);

    // This was a function, but the smart pointers where making that hard
    const unique_ptr<sf::Texture> character_textures[6] = {
        make_unique<sf::Texture>(sf::Texture("client/graphics/colonel_mustard.jpg")),
        make_unique<sf::Texture>(sf::Texture("client/graphics/miss_scarlet.jpg")),
        make_unique<sf::Texture>(sf::Texture("client/graphics/professor_plum.jpg")),
        make_unique<sf::Texture>(sf::Texture("client/graphics/mr_green.jpg")),
        make_unique<sf::Texture>(sf::Texture("client/graphics/mrs_white.jpg")),
        make_unique<sf::Texture>(sf::Texture("client/graphics/mrs_peacock.jpg"))};
    const auto empty_texture = make_unique<sf::Texture>(sf::Texture("client/graphics/blank.png"));
    const auto selection_texture = make_unique<sf::Texture>(sf::Texture("client/graphics/selection.png"));
    std::unique_ptr<sf::Sprite> board_squares[24][25];
    for (int x = 0; x < 24; x++) {
        for (int y = 0; y < 25; y++) {
            board_squares[x][y] = make_unique<sf::Sprite>(sf::Sprite(*empty_texture));
            board_squares[x][y]->setScale({static_cast<float>(square_size /
                board_squares[x][y]->getGlobalBounds().size.x), static_cast<float>(square_size /
                    board_squares[x][y]->getGlobalBounds().size.y)});
            board_squares[x][y]->setPosition({static_cast<float>(upper_board_corner[0] + x * square_size),
                static_cast<float>(upper_board_corner[1] + y * square_size)});
        }
    }
    for (int player = 0; player < 6; player++) {
        change_sprite_texture(*board_squares[players[player]->position.getX()][players[player]->position.getY()],
            *character_textures[player]);
    }

    Scratch_Pad::clear_data(); // Emptys old data
    unique_ptr<Scratch_Pad> current_users_pad;


    // Main game loop
    std::vector<Tile> tiles_in_reach; // Created here for use in full main game loop scope
    while (main_game_window.isOpen()) {

        // Game maintenance
        if (!players[current_player]->playing) { // Skips over NPC's
            current_player = (current_player + 1) % 6;
        }
        else if (current_player != prior_player) { // This means a new turn has rolled over
            prior_player = current_player;
            current_users_pad = make_unique<Scratch_Pad>(screen_size, current_player + 1);
            // This is an incredibly expensive operation so it should be run as little as possible
            tiles_in_reach = find_reachable_tiles(players[current_player]->position,
                generate_random_int(1, 6), clue_board); // This has to be at least O(n^n)
            for (const auto &tile: tiles_in_reach) {
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
                        clue_board.deleteToken(players[current_player]->characterToken,
                            players[current_player]->position);
                        change_sprite_texture(*board_squares[players[current_player]->position.getX()]
                            [players[current_player]->position.getY()], *empty_texture);
                        players[current_player]->position = tile;
                        // Add room code here
                        change_sprite_texture(*board_squares[tile.getX()][tile.getY()],
                            *character_textures[current_player]);
                        remove_item_from_vector(tiles_in_reach, tile); // Sets up for next line
                        for (const auto &unused_tile: tiles_in_reach) { // Resets textures
                            change_sprite_texture(*board_squares[unused_tile.getX()][unused_tile.getY()],
                                *empty_texture);
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
        for (auto &rows: board_squares) {
            for (auto &squares: rows) {
                main_game_window.draw(*squares);
            }
        }
        main_game_window.display();
        if (current_users_pad != nullptr) {
            current_users_pad->update();
        }
    }

    return 0;
}
