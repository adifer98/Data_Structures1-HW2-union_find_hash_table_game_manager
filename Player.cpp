#include "Player.h"


Player::Player(int player_id, int player_group_id, int player_score, int player_level) :
id(player_id), group_id(player_group_id), score(player_score), level(player_level)
{
    next = NULL;
}

Player::~Player()
{
    next = NULL;
}

int Player::getId()
{
    return id;
}

int Player::getLevel()
{
    return level;
}

int Player::getGroupId()
{
    return group_id;
}

int Player::getScore()
{
    return score;
}

std::shared_ptr<Player>* Player::getNext()
{
    return next;
}

void Player::setLevel(int new_level)
{
    level = new_level;
}

void Player::setGroupId(int new_group_id)
{
    group_id = new_group_id;
}

void Player::addToLevel(int amount)
{
    level += amount;
}

void Player::setScore(int new_score)
{
    score = new_score;
}

void Player::setNext(std::shared_ptr<Player>* new_next)
{
    next = new_next;
}



bool Player::playersCompByLevel(Player* player)
{
    if(player == NULL)
    {
        return false;
    }

    if(level > player->level)
    {
        return true;
    }
    if(level < player->level)
    {
        return false;
    }
    if(score > player->score)
    {
        return true;
    }
    if(score < player->score)
    {
        return false;
    }
    if(id > player->id)
    {
        return true;
    }
    return false;
}


bool Player::playersCompByScore(Player* player)
{
    if(player == NULL)
    {
        return false;
    }

    if(score > player->score)
    {
        return true;
    }
    if(score < player->score)
    {
        return false;
    }    
    if(level > player->level)
    {
        return true;
    }
    if(level < player->level)
    {
        return false;
    }
    if(id > player->id)
    {
        return true;
    }
    return false;
}
