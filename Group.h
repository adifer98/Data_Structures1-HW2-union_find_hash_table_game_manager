#ifndef GROUP_H
#define GROUP_H

#include "PlayersAvlTree.h"


class Group {
    
    private:
        int id;
        int num_players;
        int num_players_with_level_zero;
        PlayersAvlTree* players_in_group_tree_by_level;
        PlayersAvlTree* players_in_group_tree_by_score;
        Group* parent;
        int* score_hist;

    public:
        Group(int group_id, int scale);
        ~Group();
        int getId();
        int getNumPlayers();
        int getNumPlayersWithLevelZero();
        PlayersAvlTree* getPlayersInGroupTreeByLevel();
        PlayersAvlTree* getPlayersInGroupTreeByScore();
        void setTrees(PlayersAvlTree* new_players_in_group_tree_by_level, PlayersAvlTree* new_players_in_group_tree_by_score);
        void reduceFromNumPlayersWithLevelZero(int score);
        void addToNumPlayersWithLevelZero(int score);
        void reduceFromNumPlayers();
        void setNumPlayersWithLevelZero(int num);
        Group* getParent();
        int* getScoreHist();
        void setScoreHist(int *new_hist);

        void setId(int new_id);
        void setNumPlayers(int new_num_players);
        void setParent(Group* new_parent);

        void addToNumPlayers();
        void decToNumPlayers();
        void addPlayer(std::shared_ptr<Player>& player);
        void removePlayerFromGroup(Player* player);
        Node* getHighestLevelPlayer();
        Node* getHighestScorePlayer();
        Node* getNodeWithRank(int rank);

        int numPlayersInBoundByLevel(int lowerLevel, int higherLevel);
        int numPlayersInBoundByWithScore(int lowerLevel, int higherLevel, int score);


};





#endif