#pragma once

/**
 * @enum CardType
 * @brief Enumeration of card types possible in the Warzone game.
 */
enum class CardType : char {
    BOMB,
    REINFORCEMENT,
    BLOCKADE,
    AIRLIFT,
    DIPLOMACY
};

class Card;
class Deck;
class Hand;
