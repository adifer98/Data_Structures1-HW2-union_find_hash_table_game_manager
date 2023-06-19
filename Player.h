#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

class Player {
    private:
       int id;
       int group_id;
       int score;
       int level;
       std::shared_ptr<Player> *next;

    public:
       Player(int player_id, int player_group_id, int player_score, int player_level);
       Player(Player& player);
       ~Player();
       int getId();
       int getLevel();
       int getGroupId();
       int getScore();
       std::shared_ptr<Player>* getNext();

       void setLevel(int new_level);
       void addToLevel(int amount);
       void setGroupId(int new_group_id);
       void setScore(int new_score);
       void setNext(std::shared_ptr<Player>* new_next);

       bool playersCompByLevel(Player* player);
       bool playersCompByScore(Player* player);

};


#endif