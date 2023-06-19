#include "Group.h"


Group::Group(int group_id, int scale) : id(group_id)
{
    num_players = 0;
    num_players_with_level_zero = 0;
    parent = NULL;
    players_in_group_tree_by_level = new PlayersAvlTree();
    players_in_group_tree_by_score = new PlayersAvlTree();
    score_hist = new int[scale + 1];
    for(int i=0; i<=scale; i++)
    {
        score_hist[i] = 0;
    }
}

Group::~Group()
{
    parent = NULL;
    delete players_in_group_tree_by_level;
    delete players_in_group_tree_by_score;
    delete[] score_hist;
}

int Group::getId()
{
    return id;
}

int Group::getNumPlayers()
{
    return num_players;
}

PlayersAvlTree* Group::getPlayersInGroupTreeByLevel()
{
    return players_in_group_tree_by_level;
}

PlayersAvlTree* Group::getPlayersInGroupTreeByScore()
{
    return players_in_group_tree_by_score;
}

Group* Group::getParent()
{
    return parent;
}

int* Group::getScoreHist()
{
    return score_hist;
}

void Group::setScoreHist(int *new_hist)
{
    int* temp = score_hist;
    score_hist = new_hist;
    delete[] temp;
}


void Group::setId(int new_id)
{
    id = new_id;
}

void Group::setNumPlayers(int new_num_players)
{
    num_players = new_num_players;
}

int Group::getNumPlayersWithLevelZero()
{
    return num_players_with_level_zero;
}

void Group::setNumPlayersWithLevelZero(int num)
{
    num_players_with_level_zero = num;
}

void Group::reduceFromNumPlayers()
{
    num_players--;
}

void Group::reduceFromNumPlayersWithLevelZero(int score)
{
    num_players_with_level_zero--;
    score_hist[score]--;
}

void Group::addToNumPlayersWithLevelZero(int score)
{
    num_players_with_level_zero++;
    score_hist[score]++;
}

void Group::setParent(Group* new_parent)
{
    parent = new_parent;
}

void Group::addToNumPlayers()
{
    num_players++;
}

void Group::decToNumPlayers()
{
    num_players--;
}

void Group::addPlayer(std::shared_ptr<Player>& player)
{
    if(player == NULL)
    {
        return;
    }
    players_in_group_tree_by_level->insert(player->getLevel(), player->getScore(), player->getId(), player);
    players_in_group_tree_by_score->insert(player->getScore(), player->getLevel(), player->getId(), player);
}


void Group::removePlayerFromGroup(Player* player)
{
    players_in_group_tree_by_level->remove(player->getLevel(), player->getScore(), player->getId());
    players_in_group_tree_by_score->remove(player->getScore(), player->getLevel(), player->getId());
}


Node* Group::getHighestLevelPlayer()
{
    return players_in_group_tree_by_level->getMax();
}

Node* Group::getHighestScorePlayer()
{
    return players_in_group_tree_by_score->getMax();
}


void Group::setTrees(PlayersAvlTree* new_players_in_group_tree_by_level, PlayersAvlTree* new_players_in_group_tree_by_score)
{
    players_in_group_tree_by_score = new_players_in_group_tree_by_score;
    players_in_group_tree_by_level = new_players_in_group_tree_by_level;
}


Node* Group::getNodeWithRank(int rank)
{
    if(rank > num_players - num_players_with_level_zero || rank <= 0)
    {
        return NULL;
    }
    Node* ptr = players_in_group_tree_by_level->getRoot();
    return Select(ptr, rank);
}


int Group::numPlayersInBoundByLevel(int lowerLevel, int higherLevel)
{
    return players_in_group_tree_by_level->numPlayersInBound(lowerLevel, higherLevel);
}


int Group::numPlayersInBoundByWithScore(int lowerLevel, int higherLevel, int score)
{
    if(higherLevel < lowerLevel || higherLevel <= 0)
    {
        return 0;
    }
    Node* left_node_in_score = players_in_group_tree_by_score->searchMainSecondKey(score,lowerLevel);
    int left_node_in_score_index=0;
    if(left_node_in_score != NULL)
    {
        left_node_in_score_index = players_in_group_tree_by_score->getNodeRank(left_node_in_score);
    }
    Node* right_node_in_score = players_in_group_tree_by_score->searchMainSecondKey(score,higherLevel+1);
    int right_node_in_score_index=0;
    if(right_node_in_score != NULL)
    {
        right_node_in_score_index = players_in_group_tree_by_score->getNodeRank(right_node_in_score);
    }
    int players_in_score_bound = (right_node_in_score_index-left_node_in_score_index);
    return players_in_score_bound;
}
