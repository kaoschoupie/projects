#include "CardsDriver.h"
#include "Cards.h"

void testCards() {
    std::cout << "-- This programme creates a deck of Warzone cards, fill completely a hand with it, and play all cards in it --" << std::endl;

    // Pointer to Deck object
    Deck* deck = new Deck();

    // Pointer to Hand object
    Hand* hand = new Hand();

    std::cout << "\n- Drawing cards from the deck -\n"
              << std::endl;
    // Draw more cards then there is in deck
    for (size_t i = 0; i < 10; ++i) {
        // Check if there are cards left in the deck
        if (deck->deckSize()) {
            // Pointer to Card object that was in deck
            Card* drawnCard = deck->draw();

            // Add the pointer in hand
            hand->addCard(new Card(*drawnCard));
            std::cout << "Drew card: " << *drawnCard << std::endl;

            delete drawnCard;
        } else {
            std::cout << "\n*No more cards in the Warzone deck."
                      << std::endl;
            break;
        }
    }

    std::cout << "\n- Playing all cards in the hand -\n"
              << std::endl;
    // Pointer to an OrderList object that holds the result of playing a specific card type
    OrdersList* orders = new OrdersList();
    // Play all cards in hand
    while (hand->handSize()) {
        // Pointer to Card object that was in hand
        Card* card = hand->draw();

        // Hold the action performed by the card drawn from hand
        card->play(orders);
        // Add the pointer in hand
        deck->addCard(*card);

        std::cout << "Played card: " << *card << std::endl;
        std::cout << "Orders after playing:\n"
                  << *orders << std::endl;

        delete card;
    }
    std::cout << "*All cards have been played and returned to the deck" << std::endl;

    delete orders;
    delete deck;
    delete hand;
}
