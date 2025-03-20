#include "Cards.h"

Card::Card(CardType type)
    : type(type) {
}

Card::Card(const Card& other)
    : Card(other.type) {
}

Card& Card::operator=(const Card& other) {
    if (this != &other) {
        type = other.type;
    }
    return *this;
}

Card::~Card() {
}

std::ostream& operator<<(std::ostream& os, const Card& card) {
    // Switch case to match a card type to its position in the CardType enum
    switch (card.type) {
    case CardType::BOMB:
        os << "Bomb";
        break;
    case CardType::REINFORCEMENT:
        os << "Reinforcement";
        break;
    case CardType::BLOCKADE:
        os << "Blockade";
        break;
    case CardType::AIRLIFT:
        os << "Airlift";
        break;
    case CardType::DIPLOMACY:
        os << "Diplomacy";
        break;
    }
    return os;
}

// TODO: UNCOMMENT THIS!!!
void Card::play(OrdersList* orders) const {
    // Switch case to match the type of a card to its order.
    switch (type) {
    case CardType::BOMB:
        // orders->add(new BombOrder());
        break;
    case CardType::REINFORCEMENT:
        // TODO: Figure out what order should be added here
        break;
    case CardType::BLOCKADE:
        // orders->add(new BlockadeOrder());
        break;
    case CardType::AIRLIFT:
        // orders->add(new AirliftOrder());
        break;
    case CardType::DIPLOMACY:
        // TODO: Figure out what order should be added here
        break;
    }
}

CardType Card::getType() const {
    return this->type;
}

Deck::Deck() {
    // Push an artbitrary collection of five cards(one for each type) on the deck
    cards.push_back(new Card(CardType::BOMB));
    cards.push_back(new Card(CardType::REINFORCEMENT));
    cards.push_back(new Card(CardType::BLOCKADE));
    cards.push_back(new Card(CardType::AIRLIFT));
    cards.push_back(new Card(CardType::DIPLOMACY));
}

Deck::Deck(const Deck& other) {
    for (const auto& card : other.cards)
        cards.push_back(new Card(*card));
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        for (const auto& card : other.cards)
            cards.push_back(new Card(*card));
    }
    return *this;
}

Deck::~Deck() {
    for (auto card : cards)
        delete card;
}

Card* Deck::draw() {
    if (this->cards.size() == 0) {
        return nullptr;
    }

    // Radomize the card draw from the deck - to add(return) in the hand of the player calling it
    size_t index = std::rand() % cards.size();
    Card* drawnCard = cards[index];
    // Delete the specific card drawn from the deck
    cards.erase(cards.begin() + index);
    return drawnCard;
}

void Deck::addCard(const Card& card) {
    cards.push_back(new Card(card));
}

bool Deck::deckSize() const {
    return cards.size();
}

Hand::Hand() { }

Hand::Hand(const Hand& other) {
    for (const auto& card : other.cards)
        cards.push_back(new Card(*card));
}

Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        for (const auto& card : other.cards)
            cards.push_back(new Card(*card));
    }
    return *this;
}

Hand::~Hand() {
    for (auto card : cards)
        delete card;
}

Card* Hand::draw() {
    if (this->cards.size() == 0) {
        return nullptr;
    }
    // Draw the last added card to the deck
    size_t index = cards.size() - 1;
    Card* drawnCard = cards[index];
    cards.pop_back();
    return drawnCard;
}

void Hand::addCard(Card* card) {
    cards.push_back(card);
}

bool Hand::handSize() const {
    return cards.size();
}
