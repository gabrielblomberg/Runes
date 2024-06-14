#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <memory>
#include <optional>

#include "util/Graph.h"
#include "util/Search.h"
#include "util/Hexagon.h"

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

    /**
     * @brief The actions that can be performed in the game.
     */
    enum class ActionType
    {
        ADD_PLAYER,
        GIVE_PLAYER_RUNE,
        MOVE_PLAYER_RUNE,
        PLACE_PLAYER_RUNE
    };

    using enum RuneType;
    using enum ActionType;

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
     * @brief Generic structure containing actions performed in the game,
     * specialised for each type of action.
     * 
     * Contains all information about an action performed in the game, including
     * any consequences of the action and arguments, depending on how it is
     * defined and used in the corresponding Action() call.
     */
    template<ActionType>
    struct ActionData;

    /**
     * @brief An action pointer is a pair of action type and a pointer to the
     * data of the action type.
     */
    struct Action {
        ActionType type;
        std::shared_ptr<void> data;
    };

    /**
     * @brief Perform an action in the game.
     * 
     * Requires the arguments to be able to construct an instance of ActionType.
     * 
     * @tparam A The type of action to perform.
     * @param args The arguments to the action.
     * 
     * @returns If the action was successful and a pointer to information
     * about the action.
     */
    template<ActionType A, typename... Args>
    std::tuple<bool, Action> perform(Args&&... args);

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

    /**
     * @brief Function overloaded to define what happens when an action is
     * performed.
     * 
     * @tparam A The type of action being performed.
     * @param data The data related to the action.
     * 
     * @returns If the action was successful.
     */
    template<ActionType A>
    bool action(ActionData<A> &data);

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

    /// History of actions performed in the game.
    std::vector<Action> m_history;
};

template<Runes::ActionType A, typename... Args>
std::tuple<bool, Runes::Action> Runes::perform(Args&&... args)
{
    auto data = std::make_shared<ActionData<A>>(std::forward<Args>(args)...);
    Action action = {
        .type = A,
        .data = std::make_shared<ActionData<A>>(std::forward<Args>(args)...)
    };

    bool success = Runes::action<A>(*(ActionData<A>*)action.data.get());
    if (success)
        m_history.push_back(action);

    return std::make_tuple(success, action);
}

template<>
struct Runes::ActionData<Runes::ADD_PLAYER> {
    std::string name;
    std::size_t player_id;
};

template<>
struct Runes::ActionData<Runes::GIVE_PLAYER_RUNE> {
    std::size_t player_id;
    Runes::RuneType rune;
    std::size_t n;
};

template<>
struct Runes::ActionData<Runes::PLACE_PLAYER_RUNE> {
    std::size_t player_id;
    Runes::RuneType rune;
    Hexagon::Hexagon<int> hexagon;
};

template<>
struct Runes::ActionData<Runes::MOVE_PLAYER_RUNE> {
    std::size_t player_id;
    Hexagon::Hexagon<int> from;
    Hexagon::Hexagon<int> to;
};
