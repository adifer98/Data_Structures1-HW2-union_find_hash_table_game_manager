#ifndef PLAYERSMANAGER_H
#define PLAYERSMANAGER_H

#include "Group.h"
#include "library2.h"

class PlayersManager {

    private:
        int num_players;
        int num_players_with_level_zero;
        int num_groups;
        int scale;
        Group** groups_array;
        std::shared_ptr<Player>** players_array;
        int players_array_size;
        PlayersAvlTree players_tree_by_level;
        PlayersAvlTree players_tree_by_score;
        int *score_hist;

        static const int N = 10;


      static void buildWholeTree(Node* start, int n)
      {
         assert(start != NULL && n >= 0);
         start->setHeight(n);
         if(n == 0)
         {
            return;
         }
         
         start->setLeft(new Node());
         start->getLeft()->setUp(start);
         buildWholeTree(start->getLeft(), n-1);
         start->setRight(new Node());
         start->getRight()->setUp(start);
         buildWholeTree(start->getRight(), n-1);
      }

      
      static void makeAlmostWhole(Node* start, int* num)
      {
         assert(start != NULL && num != NULL && *num >= 0);
         if(*num == 0)
         {
            return;
         }
         if(start->getHeight() == 0)
         {
            *num = *num - 1;
            if(start->getUp() != NULL)
            {
               if(start->getUp()->getRight() == start)
               {
                  start->getUp()->setRight(NULL);
               }
               else
               {
                  start->getUp()->setLeft(NULL);
                  start->getUp()->setHeight(0);
               }
            }
            delete start;
         }
         else
         {
            makeAlmostWhole(start->getRight(), num); 

            makeAlmostWhole(start->getLeft(), num);
            
            start->updateHeight();
         }
      }

      
      static void buildAlmostWholeTree(Node* start, int n, int num)
      {
         assert(start != NULL && n >= 0 && num >= 0);

         try{
            buildWholeTree(start, n);
            makeAlmostWhole(start, &num);
         }catch(const std::bad_alloc& e)
         {
            throw e;
         }
      }


      static int hashFunction(int player_id, int array_length)
      {
         return player_id % array_length;
      }
   

      static Group* groupFind(Group* g1)
      {
         if(g1 == NULL)
         {
               return NULL;
         }
         Group* ptr1 = g1;
         while(ptr1->getParent() != NULL)
         {
               ptr1 = ptr1->getParent();
         }
         
         Group* ptr2 = g1;
         while(ptr2->getParent() != NULL)
         {
               Group* temp = ptr2;
               ptr2 = ptr2->getParent();
               temp->setParent(ptr1);
         }
         return ptr1;
      }


      static void groupUnion(Group* g1, Group* g2, int scale)
      {
         Group* main_group1 = groupFind(g1);
         Group* main_group2 = groupFind(g2);
         if(main_group1->getId() == main_group2->getId())
         {
            return;
         }
         int *hist = new int[scale+1];
         for(int i=0; i <= scale; i++)
         {
            hist[i] = main_group1->getScoreHist()[i] + main_group2->getScoreHist()[i];
         }
         if(main_group1->getNumPlayers() >= main_group2->getNumPlayers())
         {
            main_group2->setParent(main_group1);
            main_group1->setNumPlayers(main_group1->getNumPlayers() + main_group2->getNumPlayers());
            main_group1->setNumPlayersWithLevelZero(main_group1->getNumPlayersWithLevelZero() + main_group2->getNumPlayersWithLevelZero());
            main_group1->setScoreHist(hist);
         }
         else
         {
            main_group1->setParent(main_group2);
            main_group2->setNumPlayers(main_group1->getNumPlayers() + main_group2->getNumPlayers());
            main_group2->setNumPlayersWithLevelZero(main_group1->getNumPlayersWithLevelZero() + main_group2->getNumPlayersWithLevelZero());
            main_group2->setScoreHist(hist);
         }
      }


      static void checkPlayersArray(PlayersManager* DS)
      {
         if(DS->players_array_size > DS->num_players)
         {
               return;
         }
         std::shared_ptr<Player>** new_array = new std::shared_ptr<Player>*[2 * DS->players_array_size];
         int i=0;
         while(i < 2 * DS->players_array_size)
         {
               new_array[i] = NULL;
               i++;
         }
         i=0;
         int old_size = DS->players_array_size;
         DS->players_array_size *= 2;
         while(i < old_size)
         {
            std::shared_ptr<Player>* ptr = DS->players_array[i];
            while(ptr!=NULL)
            {
               std::shared_ptr<Player>* next = ptr->get()->getNext();
               int index = hashFunction(ptr->get()->getId(),DS->players_array_size);
               if(new_array[index]!=NULL)
               {
                  ptr->get()->setNext(new_array[index]);
                  new_array[index] = ptr;
               }
               else
               {
                  ptr->get()->setNext(NULL);
                  new_array[index] = ptr;

               }
               ptr = next;
            }
            i++;
         }
         

         std::shared_ptr<Player>** temp = DS->players_array;
         DS->players_array = new_array;
         delete[] temp;
      }  


      static std::shared_ptr<Player>** mergeSortByLevel(std::shared_ptr<Player>** array1, int len1, std::shared_ptr<Player>** array2, int len2)
      {
         int length = len1 + len2;
         std::shared_ptr<Player>** new_array;
         try
         {
            new_array = (std::shared_ptr<Player>**)malloc(length * sizeof(std::shared_ptr<Player>*));
         }
         catch(const std::bad_alloc& e)
         {
            throw e;
         }
         
         int index = 0, i1 = 0, i2 = 0;
         while(i1 < len1 && i2 < len2)
         {
            Player* player1_ptr = array1[i1]->get(); 
            Player* player2_ptr = array2[i2]->get();

            if(player1_ptr->playersCompByLevel(player2_ptr))
            {
               new_array[index] = array2[i2];
               i2++;
            }
            else
            {
               new_array[index] = array1[i1];
               i1++;
            }
            index++;
         }

         if(i1 == len1)
         {
            while(i2 < len2)
            {
               new_array[index] = array2[i2];
               i2++;
               index++;
            }
         }
         else //i2 == len2
         {
            while(i1 < len1)
            {
               new_array[index] = array1[i1];
               i1++;
               index++;
            }
         }

         return new_array;
      }

      static std::shared_ptr<Player>** mergeSortByScore(std::shared_ptr<Player>** array1, int len1, std::shared_ptr<Player>** array2, int len2)
      {
         int length = len1 + len2;
         std::shared_ptr<Player>** new_array;
         try
         {
            new_array = (std::shared_ptr<Player>**)malloc(length * sizeof(std::shared_ptr<Player>*));
         }
         catch(const std::bad_alloc& e)
         {
            throw e;
         }
         
         int index = 0, i1 = 0, i2 = 0;
         while(i1 < len1 && i2 < len2)
         {
            Player* player1_ptr = array1[i1]->get(); 
            Player* player2_ptr = array2[i2]->get();
            
            if(player1_ptr->playersCompByScore(player2_ptr))
            {
               new_array[index] = array2[i2];
               i2++;
            }
            else
            {
               new_array[index] = array1[i1];
               i1++;
            }
            index++;
         }

         if(i1 == len1)
         {
            while(i2 < len2)
            {
               new_array[index] = array2[i2];
               i2++;
               index++;
            }
         }
         else //i2 == len2
         {
            while(i1 < len1)
            {
               new_array[index] = array1[i1];
               i1++;
               index++;
            }
         }

         return new_array;
      }


      static void addArrayByLevel(Node* start, std::shared_ptr<Player>** array, int GroupID, int* index)
      {
         assert(index != NULL);
         if(start == NULL)
         {
            return;
         }
         addArrayByLevel(start->getLeft(), array, GroupID, index);

         array[*index]->get()->setGroupId(GroupID);
         start->setInfo(*(array[*index]));
         start->setMainKey(array[*index]->get()->getLevel());
         start->setSecondaryKey(array[*index]->get()->getScore());
         start->setThirdKey(array[*index]->get()->getId());

         *index = *index + 1;

         addArrayByLevel(start->getRight(), array, GroupID, index);
      }


      static void addArrayByScore(Node* start, std::shared_ptr<Player>** array, int GroupID, int* index)
      {
         assert(index != NULL);
         if(start == NULL)
         {
            return;
         }
         addArrayByScore(start->getLeft(), array, GroupID, index);

         array[*index]->get()->setGroupId(GroupID);
         start->setInfo(*(array[*index]));
         start->setMainKey(array[*index]->get()->getScore());
         start->setSecondaryKey(array[*index]->get()->getLevel());
         start->setThirdKey(array[*index]->get()->getId());

         *index = *index + 1;

         addArrayByScore(start->getRight(), array, GroupID, index);
      }



      static StatusType replaceGroupsTrees(Group *group, Group *replacement)
      {
         int n_group = group->getNumPlayers() - group->getNumPlayersWithLevelZero();
         int n_replacement = replacement->getNumPlayers() - replacement->getNumPlayersWithLevelZero();
         //checks if group is empty, then there's no need for a new tree
         if(n_group == 0)
         {
            return SUCCESS;
         }

         //makes four arrays, two for one group
         std::shared_ptr<Player>** array_group_by_level;
         std::shared_ptr<Player>** array_replacement_by_level;
         std::shared_ptr<Player>** array_group_by_score;
         std::shared_ptr<Player>** array_replacement_by_score;
         try{
            array_group_by_level = group->getPlayersInGroupTreeByLevel()->makeArrayForPlayers(n_group);
         } catch(const std::bad_alloc& e)
         {
            return ALLOCATION_ERROR;
         }
         try{
            array_replacement_by_level = replacement->getPlayersInGroupTreeByLevel()->makeArrayForPlayers(n_replacement);
         } catch(const std::bad_alloc& e)
         {
            free(array_group_by_level);
            return ALLOCATION_ERROR;
         }
         try{
            array_group_by_score = group->getPlayersInGroupTreeByScore()->makeArrayForPlayers(n_group);
         } catch(const std::bad_alloc& e)
         {
            free(array_group_by_level);
            if(n_replacement > 0)
            {
               free(array_replacement_by_level);
            } 
            return ALLOCATION_ERROR;
         }
         try{
            array_replacement_by_score = replacement->getPlayersInGroupTreeByScore()->makeArrayForPlayers(n_replacement);
         } catch(const std::bad_alloc& e)
         {
            free(array_group_by_level);
            free(array_group_by_score);
            if(n_replacement > 0)
            {
               free(array_replacement_by_level);
            } 
            return ALLOCATION_ERROR;
         }

         //merges the arrays
         //for levels tree:
         std::shared_ptr<Player>** merged_array_by_level;
         try{
            merged_array_by_level = mergeSortByLevel(array_group_by_level, n_group, array_replacement_by_level, n_replacement);
         } catch(const std::bad_alloc& e)
         {
            free(array_group_by_level);
            free(array_group_by_score);
            if(n_replacement > 0)
            {
               free(array_replacement_by_level);
               free(array_replacement_by_score);
            } 
            return ALLOCATION_ERROR;
         }
         
         //for scores tree
         std::shared_ptr<Player>** merged_array_by_score;
         try{
            merged_array_by_score = mergeSortByScore(array_group_by_score, n_group, array_replacement_by_score, n_replacement);
         } catch(const std::bad_alloc& e)
         {
            free(array_group_by_level);
            free(array_group_by_score);
            if(n_replacement > 0)
            {
               free(array_replacement_by_level);
               free(array_replacement_by_score);
            } 
            free(merged_array_by_level);
            return ALLOCATION_ERROR;
         }

         //makes new trees:

         //adresses the trees:

         //for levels tree:
         PlayersAvlTree* mix_tree_by_level;
         Node* root_by_level;
         mix_tree_by_level = new PlayersAvlTree();
         root_by_level = new Node();
         mix_tree_by_level->setRoot(root_by_level);

         //for scores tree:
         PlayersAvlTree* mix_tree_by_score;
         Node* root_by_score;
         mix_tree_by_score = new PlayersAvlTree();
         root_by_score = new Node();
         mix_tree_by_score->setRoot(root_by_score);



         
         //gets the potential size of the trees:
         int n = 0, num = 0;
         while(num < (n_group + n_replacement))
         {
            num += pow(2,n);
            n++;
         }

         //makes the trees
         int length = num - n_group - n_replacement;
         buildAlmostWholeTree(root_by_level, n - 1, length);
         buildAlmostWholeTree(root_by_score, n - 1, length);
         
         int index1 = 0, index2 = 0;
         addArrayByLevel(root_by_level, merged_array_by_level, replacement->getId(), &index1);
         addArrayByScore(root_by_score, merged_array_by_score, replacement->getId(), &index2);
         
         //sets the max for the trees
         Node *max_by_level, *max_by_score;
         Node* group_max_by_level = group->getHighestLevelPlayer();
         Node* group_max_by_score = group->getHighestScorePlayer();
         if(n_replacement == 0)
         {
            max_by_level = group_max_by_level;
            max_by_score = group_max_by_score;
         }
         else
         {
            Node* replacement_max_by_level = replacement->getHighestLevelPlayer();
            Node* replacement_max_by_score = replacement->getHighestScorePlayer();
            if(replacement_max_by_level->nodeCmp(group_max_by_level->getMainKey(), group_max_by_level->getSecondaryKey(), group_max_by_level->getThirdKey()))
            {
               max_by_level = replacement_max_by_level;
            }
            else
            {
               max_by_level = group_max_by_level;
            }

            if(replacement_max_by_score->nodeCmp(group_max_by_score->getMainKey(), group_max_by_score->getSecondaryKey(), group_max_by_score->getThirdKey()))
            {
               max_by_score = replacement_max_by_score;
            }
            else
            {
               max_by_score = group_max_by_score;
            }
         }

         //find the max node in the trees
         Node* max_node_for_level = mix_tree_by_level->find(max_by_level->getMainKey(), max_by_level->getSecondaryKey(), max_by_level->getThirdKey());
         Node* max_node_for_score = mix_tree_by_score->find(max_by_score->getMainKey(), max_by_score->getSecondaryKey(), max_by_score->getThirdKey());
           //!=NULL O(log(n_group+n_replacement))
         //set the trees' max
         mix_tree_by_level->setMax(max_node_for_level);
         mix_tree_by_score->setMax(max_node_for_score);


         //updates the trees' variables (w, sum_score, height)
         mix_tree_by_level->updateTree(); //O(n_group + n_replacement)
         mix_tree_by_score->updateTree(); //O(n_group + n_replacement)
         
         //adresses and adds the new trees to the replacement group and deletes its previous trees
         PlayersAvlTree* temp1 = replacement->getPlayersInGroupTreeByLevel();
         PlayersAvlTree* temp2 = replacement->getPlayersInGroupTreeByScore(); 
         replacement->setTrees(mix_tree_by_level, mix_tree_by_score);
         
         delete temp1;
         delete temp2;
      
         free(array_group_by_level);
         free(array_group_by_score);

         if(n_replacement > 0)
         {
            free(array_replacement_by_level);
            free(array_replacement_by_score);
         }

         free(merged_array_by_level);
         free(merged_array_by_score);

         return SUCCESS;
      }


    public:
         PlayersManager(int k, int DS_scale);
         ~PlayersManager();
         int getNumPlayers() { return num_players; }
         int getNumPlayersWithLevelZero() {return num_players_with_level_zero; }
         int getNumGroups() { return num_groups; }
         int getScale() { return scale; }
         Group** getGroupsArray() { return groups_array; }
         std::shared_ptr<Player>** getPlayersArray() { return players_array; }
         int getPlayersArraysize() { return players_array_size; }
         void setPlayersArraySize(int new_size) { players_array_size = new_size; }
         PlayersAvlTree& getPlayersTreeByLevel() { return players_tree_by_level; }
         PlayersAvlTree& getPlayersTreeByScore() { return players_tree_by_score; }

         void removePlayerFromTrees(Player* player)
         {
            players_tree_by_level.remove(player->getLevel(), player->getScore(), player->getId());
            players_tree_by_score.remove(player->getScore(), player->getLevel(), player->getId());
            num_players--;
         }

         Node* getNodeWithRank(int rank)
         {
            if(rank > num_players - num_players_with_level_zero || rank <= 0)
            {
               return NULL;
            }
            Node* ptr = players_tree_by_level.getRoot();
            return Select(ptr, rank);
         }


         int numPlayersInBoundByLevel(int lowerLevel, int higherLevel)
         {
            return players_tree_by_level.numPlayersInBound(lowerLevel, higherLevel);
         }

         int numPlayersInBoundByWithScore(int lowerLevel, int higherLevel, int score)
         {
            if(higherLevel < lowerLevel || higherLevel <= 0)
            {
               return 0;
            }
            Node* left_node_in_score = players_tree_by_score.searchMainSecondKey(score,lowerLevel);
            int left_node_in_score_index=0;
            if(left_node_in_score != NULL)
            {
               left_node_in_score_index = players_tree_by_score.getNodeRank(left_node_in_score);
            }
            Node* right_node_in_score = players_tree_by_score.searchMainSecondKey(score,higherLevel+1);
            int right_node_in_score_index=0;
            if(right_node_in_score != NULL)
            {
               right_node_in_score_index = players_tree_by_score.getNodeRank(right_node_in_score);
            }
            int players_in_score_bound = (right_node_in_score_index-left_node_in_score_index);
            return players_in_score_bound;
         }

         

         StatusType mergeGroups(int GroupID1, int GroupID2);

         StatusType addPlayer(int PlayerID, int GroupID, int score);

         StatusType RemovePlayer(int PlayerID);

         StatusType increasePlayerIDLevel(int PlayerID, int LevelIncrease);

         StatusType changePlayerIDScore(int PlayerID, int NewScore);

         StatusType averageHighestPlayerLevelByGroup(int GroupID, int m, double * avgLevel);

         StatusType getPercentOfPlayersWithScoreInBounds (int GroupID, int score, int lowerLevel, int higherLevel, double * players);

         StatusType getPlayersBound(int GroupID, int score, int m, int * LowerBoundPlayers, int * HigherBoundPlayers);
};



#endif