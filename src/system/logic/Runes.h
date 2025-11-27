#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <memory>
#include <optional>

#include "utility/Graph.h"
#include "utility/Search.h"
#include "utility/Hexagon.h"

class Runes
{
public:

    /**
     * @brief Create a new instance of Runes.
     */
    Runes() = default;

    /**
     * @brief The type of rune
     */
    enum class RuneType
    {
        VITALITY
    };

    struct AddPlayer {
        std::string name;
    };

    struct GiveRune {
        std::size_t player_id;
        Runes::RuneType rune;
        std::size_t n;
    };

    struct PlaceRune {
        std::size_t player_id;
        Runes::RuneType rune;
        Hexagon::Hexagon<int> hexagon;
    };

    struct MoveRune {
        std::size_t player_id;
        Hexagon::Hexagon<int> from;
        Hexagon::Hexagon<int> to;
    };

    std::optional<std::size_t> add_player(const AddPlayer &request);

    bool give_rune(const GiveRune &request);

    bool place_rune(const PlaceRune &request);

    bool move_rune(const MoveRune &request);

    /**
     * @brief A player in the game.
     * 
     * Each player owns a collection of runes that they are able to place.
     */
    class Player
    {
    public:

        /**
         * @brief Create a new player.
         * 
         * @param id The identifier of the player.
         * @param name The name of the player.
         */
        inline Player(std::size_t id, const std::string &name)
            : m_id(id)
            , m_name(name)
        {}

        /**
         * @brief Get the unique identifier of the player.
         * @returns The unique identifier of the player.
         */
        inline std::size_t id() const {
            return m_id;
        };

        /**
         * @brief Get the name of the player.
         * @returns The name of the player.
         */
        inline const std::string &name() const {
            return m_name;
        };

        /**
         * @brief Get the runes that this player has not yet played.
         * @return The runes this player has not played.
         */
        inline const std::unordered_map<RuneType, std::size_t> &runes() {
            return m_runes;
        }

    private:

        friend class Runes;

        /// The players unique identifier.
        std::size_t m_id;

        /// The optional name of the player.
        std::string m_name;

        /// The number of different runes this player has not yet played.
        std::unordered_map<RuneType, std::size_t> m_runes;
    };

    /**
     * @brief A rune on the board.
     */
    struct Rune {

        /// The type of rune.
        RuneType type;

        /// The owner of the rune.
        std::size_t player_id;
    };

    /// Graph of hexagons containing a stack of player runes.
    using Board = Graph<Hexagon::Hexagon<int>, Rune>;

    /**
     * @brief Get all the players that have been added to the game.
     * @return The current players.
     */
    inline const std::vector<Player> &players() const {
        return m_players;
    }

    /**
     * @brief Get the board of rune containing the pieces.
     */
    inline Board &board() {
        return m_board;
    }

    bool connected();

private:

    bool rune_moveable(Hexagon::Hexagon<int> hex);

    /**
     * @brief Returns the neighbors of a hexagon in the graph.
     * 
     * Used in the search algorithm.
     * 
     * @param hex The hexagon to get neighbors of.
     * @return The neighboring hexagons.
     */
    std::vector<Hexagon::Hexagon<int>> neighbors(Hexagon::Hexagon<int> hex);

    /// Players in the order of turns.
    std::vector<Player> m_players;

    /// The current player. Empty when game has not started.
    std::optional<std::size_t> m_current_player;

    /// The game space.
    Board m_board;
};
