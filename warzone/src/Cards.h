#pragma once

#include "Cards.fwd.h"
#include "Orders.h"

#include <iostream>
#include <vector>

/**
 * @class Card
 * @brief Represents a card with a specific type.
 *
 * The Card class allows to creat, copy, and
 * delete a card from the game. It additionally
 * allow to display the type of the card and play
 * the card.
 */
class Card {
private:
    /** @brief Pointer to the type of the card. */
    CardType type;

public:
    /**
     * @brief Default constructs for the Card class.
     * @param type The type of card to create.
     */
    Card(CardType type);

    /**
     * @brief Copy constructor for the Card class.
     * @param other The Card object to copy from.
     */
    Card(const Card& other);

    /**
     * @brief Assignment operator for the Card class.
     * @param other The Card object to assign from.
     * @return Reference to this Card object.
     */
    Card& operator=(const Card& other);

    /**
     * @brief Destructor for the Card class.
     */
    ~Card();

    /**
     * @brief Stream insertion operator for outputting card details.
     * @param os The output stream.
     * @param card The Card object to output.
     * @return The Card type as output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Card& card);

    /**
     * @brief Plays the card and creates/add an order in player's list of order.
     * @param orders Pointer to the orders list to modify.
     */
    void play(OrdersList* orders) const;

    CardType getType() const;
};

/**
 * @class Deck
 * @brief Represents a deck of Warzone cards.
 *
 * The Deck class manages a finite collection of cards,
 * allowing for drawing cards from it, and adding cards,
 * as well as checking the current size of the deck.
 */
class Deck {
private:
    /** @brief Vector of pointers to cards in the deck. */
    std::vector<Card*> cards;

public:
    /**
     * @brief Default constructs for the Deck class.
     */
    Deck();

    /**
     * @brief Copy constructor for the Deck class.
     * @param other The Deck object to copy from.
     */
    Deck(const Deck& other);

    /**
     * @brief Assignment operator for the Deck class.
     * @param other The Deck object to assign from.
     * @return Reference to this Deck object.
     */
    Deck& operator=(const Deck& other);

    /**
     * @brief Destructor for the Deck class.
     */
    ~Deck();

    /**
     * @brief Draws a card from the deck.
     * @return The drawn Card object.
     */
    Card* draw();

    /**
     * @brief Adds a card back into the deck.
     * @param card The Card object to add back.
     */
    void addCard(const Card& card);

    /**
     * @brief Checks the size of the deck.
     * @return True if there are cards in the deck, false otherwise.
     */
    bool deckSize() const;
};

/**
 * @class Hand
 * @brief Represents a hand of cards held by a player.
 *
 * The Hand class manages a finite collection of Warzone cards,
 * allows to drawing cards from the hand and, adding new cards
 * to it, as well as checking the current size of the hand.
 */
class Hand {
private:
    /** @brief Vector of pointers to cards in the hand. */
    std::vector<Card*> cards;

public:
    /**
     * @brief Constructor for the Hand class.
     */
    Hand();

    /**
     * @brief Copy constructor for the Hand class.
     * @param other The Hand object to copy from.
     */
    Hand(const Hand& other);

    /**
     * @brief Assignment operator for the Hand class.
     * @param other The Hand object to assign from.
     * @return Reference to this Hand object.
     */
    Hand& operator=(const Hand& other);

    /**
     * @brief Destructor for the Hand class.
     */
    ~Hand();

    /**
     * @brief Draws a card from the hand.
     * @return The drawn Card object.
     */
    Card* draw();

    /**
     * @brief Adds a card to the hand.
     * @param card Pointer to the Card object to add.
     */
    void addCard(Card* card);

    /**
     * @brief Checks the size of the hand.
     * @return True if there are cards in the hand, false otherwise.
     */
    bool handSize() const;
};
