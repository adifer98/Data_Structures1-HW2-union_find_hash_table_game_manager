#include "Node.h"
#include "PlayersAvlTree.h"
#include "Player.h"
#include <memory>
#include "library2.h"
#include "PlayersManager.h"

void printTree(Node* root)
{
    if (root == NULL)
     return;
    printTree(root->getLeft());
    std::cout << "Id-" <<root->getInfo()->getId()<<" Level-"<<root->getInfo()->getLevel()<<" Score-" <<root->getInfo()->getScore() << std::endl;
    printTree(root->getRight()); 
}

void printHist(int* hist)
{
    for(int i=0;i<=100;i++)
    {
        std::cout << i << "-" << hist[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
    PlayersManager* DS = (PlayersManager*)Init(5,100);

    if(AddPlayer(DS,1,1,1) == SUCCESS)
    {
        std::cout << "SUCCESS" << std::endl;
    }
    if(AddPlayer(DS,2,2,10) == SUCCESS)
    {
        std::cout << "SUCCESS" << std::endl;
    }
    if(AddPlayer(DS,3,3,10) == SUCCESS)
    {
        std::cout << "SUCCESS" << std::endl;
    }
    if(AddPlayer(DS,4,4,10) == SUCCESS)
    {
        std::cout << "SUCCESS" << std::endl;
    }
    if(AddPlayer(DS,5,5,10) == SUCCESS)
    {
        std::cout << "SUCCESS" << std::endl;
    }

    IncreasePlayerIDLevel(DS,1,1);
    IncreasePlayerIDLevel(DS,2,2);
    //IncreasePlayerIDLevel(DS,3,3);
    IncreasePlayerIDLevel(DS,4,4);
    IncreasePlayerIDLevel(DS,5,5);

    MergeGroups(DS,5,2);
    MergeGroups(DS,1,2);
    MergeGroups(DS,3,4);
    MergeGroups(DS,5,3);

    printTree(DS->getGroupsArray()[0]->getPlayersInGroupTreeByLevel()->getRoot());

    double result=0;
    AverageHighestPlayerLevelByGroup(DS,5,5,&result);
    std::cout << "Avarage: " << result << std::endl;
    
    Quit((void**)&DS);

}