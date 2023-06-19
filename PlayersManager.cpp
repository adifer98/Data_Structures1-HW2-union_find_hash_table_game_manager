#include "PlayersManager.h"

PlayersManager::PlayersManager(int k, int DS_scale) : num_groups(k), scale(DS_scale)
         {
            num_players = 0;
            num_players_with_level_zero = 0;

            //Creater players array for hash-table
            players_array_size = N;
            players_array = new std::shared_ptr<Player>*[N];
            for(int i=0;i<N;i++)
            {
               players_array[i] = NULL;
            }

            //Creater group array for union-find
            groups_array = new Group*[k];
            for(int i=0;i<k;i++)
            {
               groups_array[i] = new Group(i+1, DS_scale);
            }

            //Create player in level 0 score hist
            score_hist = new int[DS_scale + 1];
            for(int i=0; i<=DS_scale;i++)
            {
               score_hist[i] = 0;
            }
         }

PlayersManager::~PlayersManager()
{
    //Delete players array
    for(int i=0;i<players_array_size;i++)
    {
        if(players_array[i]==NULL)
        {
            continue;
        }
        else
        {
            std::shared_ptr<Player>* ptr = players_array[i];
            while(ptr!=NULL)
            {
                std::shared_ptr<Player>* temp = ptr;
                ptr=ptr->get()->getNext();
                delete temp;
            }
        }
    }

    //Delete group array
    for(int i=0;i<num_groups;i++)
    {
        delete groups_array[i];
    }
    delete[] groups_array;
    delete[] players_array;

    //Delete score hist
    delete[] score_hist;
}


StatusType PlayersManager::mergeGroups(int GroupID1, int GroupID2)
{
    //checks if the input is invalid
    if(GroupID1 <= 0 || GroupID2 <= 0 || GroupID1 > num_groups || GroupID2 > num_groups)
    {
        return INVALID_INPUT;
    }
    //gets the groups that fill GroupID1 and GroupID2
    Group* old_parent1 = groupFind(groups_array[GroupID1-1]);
    Group* old_parent2 = groupFind(groups_array[GroupID2-1]);
    //checks if those are the same group
    if(old_parent1->getId() == old_parent2->getId())
    {
        return SUCCESS;
    }
    //check which group is bigger and adds to it the trees
    if(old_parent1->getNumPlayers() < old_parent2->getNumPlayers())
    {
        StatusType status = replaceGroupsTrees(old_parent1, old_parent2);
        if(status != SUCCESS)
        {
            return status;
        }
    }
    else
    {
        StatusType status = replaceGroupsTrees(old_parent2, old_parent1);
        if(status != SUCCESS)
        {
            return status;
        }
    }
    //union the groups
    groupUnion(groups_array[GroupID1-1], groups_array[GroupID2-1], scale);
    return SUCCESS;
}


StatusType PlayersManager::RemovePlayer(int PlayerID)
{
    
    //checks if input is valid:
    if(PlayerID <= 0)
    {
        return INVALID_INPUT;
    }
    
    //checks if the player exists and deletes:
    std::shared_ptr<Player>* ptr = players_array[hashFunction(PlayerID, players_array_size)];
    if(ptr == NULL)
    {
        return FAILURE;
    }
    
    //checks if it's the first player in the list
    if(ptr->get()->getId() == PlayerID)
    {
        players_array[hashFunction(PlayerID, players_array_size)] = ptr->get()->getNext();
        int group_id = ptr->get()->getGroupId();
        Group* main_group = groupFind(groups_array[group_id-1]);
        if(ptr->get()->getLevel() > 0)
        {
            main_group->removePlayerFromGroup(ptr->get());
            this->removePlayerFromTrees(ptr->get());
        }
        else
        {
            main_group->reduceFromNumPlayersWithLevelZero(ptr->get()->getScore());
            num_players--;
            num_players_with_level_zero--;
            score_hist[ptr->get()->getScore()]--;
        }
        main_group->reduceFromNumPlayers();
        delete ptr;
        return SUCCESS;
    }

    //runs through the list
    std::shared_ptr<Player>* prev = ptr;
    ptr = ptr->get()->getNext();
    while(ptr != NULL)
    {
        if(ptr->get()->getId() == PlayerID)
        {
            prev->get()->setNext(ptr->get()->getNext());
            int group_id = ptr->get()->getGroupId();
            Group* main_group = groupFind(groups_array[group_id-1]);
            if(ptr->get()->getLevel() > 0)
            {
                main_group->removePlayerFromGroup(ptr->get());
                this->removePlayerFromTrees(ptr->get());
            }
            else
            {
                main_group->reduceFromNumPlayersWithLevelZero(ptr->get()->getScore());
                num_players--;
                num_players_with_level_zero--;
                score_hist[ptr->get()->getScore()]--;
            }
            main_group->reduceFromNumPlayers();
            delete ptr;
            return SUCCESS;
        }
        prev = ptr;
        ptr = ptr->get()->getNext();
    }

    //the player doesn't exist
    return FAILURE;
}


StatusType PlayersManager::addPlayer(int PlayerID, int GroupID, int score)
{
    //Input check
    if(GroupID>num_groups || GroupID <=0 || PlayerID<=0 || score>scale || score<=0 )
    {
        return INVALID_INPUT;
    }

    //Check if the player already exist
    int index = hashFunction(PlayerID,players_array_size);
    std::shared_ptr<Player>* ptr = players_array[index];
    while(ptr!=NULL)
    {
        if(PlayerID==ptr->get()->getId())
        {
            return FAILURE;
        }
        ptr = ptr->get()->getNext();
    }

    //If the player doesnt exist
    //Adding the player to the hash table
    checkPlayersArray(this);
    index = hashFunction(PlayerID,players_array_size);
    std::shared_ptr<Player>* new_player = new std::shared_ptr<Player>((new Player(PlayerID,GroupID,score,0)));
    if(new_player->get()==NULL)
    {
        return ALLOCATION_ERROR;
    }
    new_player->get()->setNext(players_array[index]);
    players_array[index]=new_player;

    //Add the player to score hist
    //Group score hist
    Group* group = groupFind(groups_array[GroupID-1]);
    group->addToNumPlayers();
    group->addToNumPlayersWithLevelZero(score);
    //System score hist
    score_hist[score]++;
    num_players_with_level_zero++;

    num_players++;

    return SUCCESS;

}


StatusType PlayersManager::changePlayerIDScore(int PlayerID, int NewScore)
{
    if(PlayerID <= 0 || NewScore <= 0 || NewScore > scale)
    {
        return INVALID_INPUT;
    }

    //checks if the player exists
    std::shared_ptr<Player>* ptr = players_array[hashFunction(PlayerID, players_array_size)];
    if(ptr == NULL)
    {
        return FAILURE;
    }

    //runs through the list
    while(ptr != NULL)
    {
        if(ptr->get()->getId() == PlayerID)
        {
            Player* player_ptr = ptr->get();
            Group* group_ptr = groupFind(groups_array[player_ptr->getGroupId() - 1]);
            if(player_ptr->getLevel() > 0)
            {
                group_ptr->getPlayersInGroupTreeByLevel()->remove(player_ptr->getLevel(), player_ptr->getScore(), player_ptr->getId());
                group_ptr->getPlayersInGroupTreeByScore()->remove(player_ptr->getScore(), player_ptr->getLevel(), player_ptr->getId());
                players_tree_by_level.remove(player_ptr->getLevel(), player_ptr->getScore(), player_ptr->getId());
                players_tree_by_score.remove(player_ptr->getScore(), player_ptr->getLevel(), player_ptr->getId());
            }

            int prevScore = player_ptr->getScore();
            player_ptr->setScore(NewScore);

            if(player_ptr->getLevel() > 0)
            {
                try{
                    group_ptr->getPlayersInGroupTreeByLevel()->insert(player_ptr->getLevel(), player_ptr->getScore(), player_ptr->getId(), *ptr);
                    group_ptr->getPlayersInGroupTreeByScore()->insert(player_ptr->getScore(), player_ptr->getLevel(), player_ptr->getId(), *ptr);
                    players_tree_by_level.insert(player_ptr->getLevel(), player_ptr->getScore(), player_ptr->getId(), *ptr);
                    players_tree_by_score.insert(player_ptr->getScore(), player_ptr->getLevel(), player_ptr->getId(), *ptr);
                } catch(const std::bad_alloc& e)
                {
                    return ALLOCATION_ERROR;
                }
            }
            else
            {
                score_hist[prevScore]--;
                score_hist[NewScore]++;
                group_ptr->getScoreHist()[prevScore]--;
                group_ptr->getScoreHist()[NewScore]++;
            }
            
            return SUCCESS;
        }
        ptr = ptr->get()->getNext();
    }
    //the player doesn't exist
    return FAILURE;
}


StatusType PlayersManager::increasePlayerIDLevel(int PlayerID, int LevelIncrease)
{
    //Input check
    if(PlayerID <=0 || LevelIncrease <=0)
    {
        return INVALID_INPUT;
    }

    //Find the player in the hash table
    int index = hashFunction(PlayerID,players_array_size);
    std::shared_ptr<Player>* ptr = players_array[index];
    while(ptr!=NULL)
    {
        if(PlayerID==ptr->get()->getId())
        {
            //If preveus player's level above 0
            if(ptr->get()->getLevel()>0)
            {
                //Remove the player from all the trees
                Group* player_group = groupFind(groups_array[ptr->get()->getGroupId()-1]);
                player_group->removePlayerFromGroup(ptr->get());
                removePlayerFromTrees(ptr->get());
                
                //Update level
                ptr->get()->setLevel(ptr->get()->getLevel()+LevelIncrease);

                //Add the player to all trees
                try
                {
                    player_group->addPlayer(*ptr);
                    players_tree_by_level.insert(ptr->get()->getLevel(),ptr->get()->getScore(),ptr->get()->getId(),*ptr);
                    players_tree_by_score.insert(ptr->get()->getScore(),ptr->get()->getLevel(),ptr->get()->getId(),*ptr);
                }catch(const std::bad_alloc& e)
                {
                    return ALLOCATION_ERROR;
                }
                num_players++;
                return SUCCESS;
            }
            //If the player's level was 0
            else
            {
                //Delete the player from scores hists
                Group* player_group = groupFind(groups_array[ptr->get()->getGroupId()-1]);
                player_group->reduceFromNumPlayersWithLevelZero(ptr->get()->getScore());
                score_hist[ptr->get()->getScore()]--;
                num_players_with_level_zero--;
                
                //Update level
                ptr->get()->setLevel(ptr->get()->getLevel()+LevelIncrease);

                //Add the player to all trees
                try
                {
                    player_group->addPlayer(*ptr);
                    players_tree_by_level.insert(ptr->get()->getLevel(),ptr->get()->getScore(),ptr->get()->getId(),*ptr);
                    players_tree_by_score.insert(ptr->get()->getScore(),ptr->get()->getLevel(),ptr->get()->getId(),*ptr);
                }catch(const std::bad_alloc& e)
                {
                    return ALLOCATION_ERROR;
                }
                return SUCCESS;
            }
        }
        ptr = ptr->get()->getNext();
    }

    //If the player doesnt exist
    return FAILURE;
}


StatusType PlayersManager::averageHighestPlayerLevelByGroup(int GroupID, int m, double * avgLevel)
{
    if(GroupID < 0 || GroupID > num_groups || m <= 0 || avgLevel == NULL)
    {
        return INVALID_INPUT;
    }
    if(GroupID > 0)
    {
        //when GroupID > 0
        Group* main_group = groupFind(groups_array[GroupID-1]);
        int n = main_group->getNumPlayers();
        if(n < m)
        {
            return FAILURE;
        }
        if(n == main_group->getNumPlayersWithLevelZero())
        {
            *avgLevel = 0;
            return SUCCESS;
        }
        Node* max;
        max = main_group->getPlayersInGroupTreeByLevel()->getMax();
        int level_sum1 = main_group->getPlayersInGroupTreeByLevel()->levelSumToNode(max);
        int num_players_in_tree = n - main_group->getNumPlayersWithLevelZero();
        Node* node = main_group->getNodeWithRank(num_players_in_tree - m);
        int level_sum2 = 0;
        if(node != NULL)
        {
            level_sum2 = main_group->getPlayersInGroupTreeByLevel()->levelSumToNode(node);
        }

        *avgLevel = ((double)(level_sum1 - level_sum2) /(double) m);
        return SUCCESS;
    }

    //when GroupID == 0
    int n = num_players;
    if(n < m)
    {
        return FAILURE;
    }
    if(n == num_players_with_level_zero)
    {
        *avgLevel = 0;
        return SUCCESS;
    }
    Node* max = players_tree_by_level.getMax();
    int level_sum1 = players_tree_by_level.levelSumToNode(max);
    int num_players_in_tree = n - num_players_with_level_zero;
    Node* node = this->getNodeWithRank(num_players_in_tree - m);
    int level_sum2 = 0;
    if(node != NULL)
    {
        level_sum2 = players_tree_by_level.levelSumToNode(node);
    }

    *avgLevel = ((double)(level_sum1 - level_sum2) / (double)m);
    return SUCCESS;
}


StatusType PlayersManager::getPercentOfPlayersWithScoreInBounds (int GroupID, int score, int lowerLevel, int higherLevel, double * players)
{
    if(players == NULL || GroupID > num_groups || GroupID < 0)
    {
        return INVALID_INPUT;
    }
    if(lowerLevel > higherLevel)
    {
        return FAILURE;
    }
    if(higherLevel<0)
    {
        *players =0;
        return SUCCESS;
    }

    PlayersAvlTree* levelTree;
    PlayersAvlTree* scoreTree;
    int totalPlayerInLevelZeroNum=0;
    int playerInLevelZeroWithScore=0;

    if(GroupID>0)
    {
        Group* group = groupFind(groups_array[GroupID-1]);
        levelTree = group->getPlayersInGroupTreeByLevel();
        scoreTree = group->getPlayersInGroupTreeByScore();
        if(lowerLevel <=0)
        {
            totalPlayerInLevelZeroNum = group->getNumPlayersWithLevelZero();
            playerInLevelZeroWithScore = group->getScoreHist()[score];
        }
    }
    else
    {
        levelTree = &players_tree_by_level;
        scoreTree = &players_tree_by_score;
        if(lowerLevel <=0)
        {
            totalPlayerInLevelZeroNum = num_players_with_level_zero;
            playerInLevelZeroWithScore = score_hist[score];
        }
    }

    //Find players in bound
    Node* left_node = levelTree->searchMainKey(lowerLevel);
    int left_index=0;
    if(left_node !=NULL)
    {
        left_index =levelTree->getNodeRank(left_node);
    }
    Node* right_node = levelTree->searchMainKey(higherLevel+1);
    int right_index=0;
    if(right_node !=NULL)
    {
        right_index = levelTree->getNodeRank(right_node);
    }

    int player_in_bound_num = (right_index-left_index) + totalPlayerInLevelZeroNum;
    if(player_in_bound_num==0)
    {
        return FAILURE;
    }

    //Find player in score bound
    Node* left_node_in_score = scoreTree->searchMainSecondKey(score,lowerLevel);
    int left_node_in_score_index=0;
    if(left_node_in_score != NULL)
    {
        left_node_in_score_index = scoreTree->getNodeRank(left_node_in_score);
    }
    Node* right_node_in_score = scoreTree->searchMainSecondKey(score,higherLevel+1);
    int right_node_in_score_index=0;
    if(right_node_in_score != NULL)
    {
        right_node_in_score_index = scoreTree->getNodeRank(right_node_in_score);
    }
    int player_in_score_bound = (right_node_in_score_index-left_node_in_score_index) + playerInLevelZeroWithScore;
    *players = ((double)player_in_score_bound/(double)player_in_bound_num)*100;
    return SUCCESS;
}




StatusType PlayersManager::getPlayersBound(int GroupID, int score, int m, int * LowerBoundPlayers, int * HigherBoundPlayers)
{
    if(GroupID < 0 || GroupID > num_groups || m < 0 || score <= 0 || score > scale || LowerBoundPlayers == NULL || HigherBoundPlayers == NULL)
    {
        return INVALID_INPUT;
    }
    if(m == 0)
    {
        *LowerBoundPlayers = 0;
        *HigherBoundPlayers = 0;
        return SUCCESS;
    }
    PlayersAvlTree *tree;
    int *hist;
    int n, n_zeros;
    Group *main_group;
    if(GroupID > 0)
    {
        main_group = groupFind(groups_array[GroupID - 1]);
        tree = main_group->getPlayersInGroupTreeByLevel();
        hist = main_group->getScoreHist();
        n = main_group->getNumPlayers();
        n_zeros = main_group->getNumPlayersWithLevelZero();
    }
    else
    {
        tree = &players_tree_by_level;
        hist = score_hist;
        n = num_players;
        n_zeros = num_players_with_level_zero;
    }
    if(m > n)
    {
        return FAILURE;
    }

    if(tree->getMax() == NULL || n == n_zeros)
    {
        *LowerBoundPlayers = m - (n_zeros - hist[score]);
        if(*LowerBoundPlayers < 0)
        {
            *LowerBoundPlayers = 0;
        }
        *HigherBoundPlayers = hist[score];
        if(*HigherBoundPlayers > m)
        {
            *HigherBoundPlayers = m;
        }
        return SUCCESS;
    }

    int lower_level = 0;
    if(GroupID > 0)
    {
        Node* node = main_group->getNodeWithRank(n - n_zeros - m + 1);
        if(node != NULL)
        {
            lower_level = node->getMainKey();
        }
    }
    else
    {
        Node* node = this->getNodeWithRank(n - n_zeros - m + 1);
        if(node != NULL)
        {
            lower_level = node->getMainKey();
        }
    }

    int numPlayersWithLevel;
    if(lower_level == 0)
    {
        numPlayersWithLevel = n_zeros;
    }
    else
    {
        numPlayersWithLevel = tree->numPlayersInBound(lower_level, lower_level);
    }

    int hasScoreWithLevel;
    if(lower_level == 0)
    {
        hasScoreWithLevel = hist[score];
    }
    else
    {
        if(GroupID > 0)
        {
            hasScoreWithLevel = main_group->numPlayersInBoundByWithScore(lower_level, lower_level, score);
        }
        else
        {
            hasScoreWithLevel = this->numPlayersInBoundByWithScore(lower_level, lower_level, score);
        }
    }

    int highest_level = tree->getMax()->getMainKey();
    int numPlayersWithHigherLevel = tree->numPlayersInBound(lower_level+1, highest_level);
    int hasScoreWithHigherLevel;
    if(GroupID > 0)
    {
        hasScoreWithHigherLevel = main_group->numPlayersInBoundByWithScore(lower_level+1, highest_level, score);
    }
    else hasScoreWithHigherLevel = this->numPlayersInBoundByWithScore(lower_level+1, highest_level, score);

    
    int playersToTakeWithLevel = m - numPlayersWithHigherLevel;
    if(playersToTakeWithLevel < 0)
    {
        playersToTakeWithLevel = 0;
    }

    if(lower_level == highest_level)
    {
        *LowerBoundPlayers = m - (numPlayersWithLevel - hasScoreWithLevel);
        if(*LowerBoundPlayers < 0)
        {
            *LowerBoundPlayers = 0;
        }
        *HigherBoundPlayers = hasScoreWithLevel;
        if(*HigherBoundPlayers > m)
        {
            *HigherBoundPlayers = m;
        }
        return SUCCESS;
    }

    *LowerBoundPlayers = playersToTakeWithLevel - (numPlayersWithLevel - hasScoreWithLevel);
    if(*LowerBoundPlayers < 0)
    {
        *LowerBoundPlayers = 0;
    }
    *HigherBoundPlayers = hasScoreWithLevel;
    if(*HigherBoundPlayers > playersToTakeWithLevel)
    {
        *HigherBoundPlayers = playersToTakeWithLevel;
    }

    *LowerBoundPlayers = *LowerBoundPlayers + hasScoreWithHigherLevel;
    *HigherBoundPlayers = *HigherBoundPlayers + hasScoreWithHigherLevel;

    return SUCCESS;
}

