#include "library2.h"
#include "PlayersManager.h"

void *Init(int k, int scale)
{
    if(k<=0 || scale>200 || scale<=0)
    {
        return NULL;
    }
    PlayersManager* DS = new PlayersManager(k,scale);
    return(void*)DS;
}

StatusType MergeGroups(void *DS, int GroupID1, int GroupID2)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->mergeGroups(GroupID1,GroupID2);
}

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int score)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->addPlayer(PlayerID,GroupID,score);
}

StatusType RemovePlayer(void *DS, int PlayerID)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->RemovePlayer(PlayerID);
}

StatusType IncreasePlayerIDLevel(void *DS, int PlayerID, int LevelIncrease)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->increasePlayerIDLevel(PlayerID,LevelIncrease);
}

StatusType ChangePlayerIDScore(void *DS, int PlayerID, int NewScore)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->changePlayerIDScore(PlayerID,NewScore);
}

StatusType GetPercentOfPlayersWithScoreInBounds(void *DS, int GroupID, int score, int lowerLevel, int higherLevel,double * players)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->getPercentOfPlayersWithScoreInBounds(GroupID,score,lowerLevel,higherLevel,players);
}

StatusType AverageHighestPlayerLevelByGroup(void *DS, int GroupID, int m, double * level)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->averageHighestPlayerLevelByGroup(GroupID,m,level);
}

//Bonus
StatusType GetPlayersBound(void *DS, int GroupID, int score, int m, int * LowerBoundPlayers, int * HigherBoundPlayers)
{
    if(DS==NULL)
    {
        return INVALID_INPUT;
    }
    return ((PlayersManager*)DS)->getPlayersBound(GroupID,score,m,LowerBoundPlayers,HigherBoundPlayers);
}

void Quit(void** DS)
{
    delete (PlayersManager*)*DS;
    *DS = NULL;
}
