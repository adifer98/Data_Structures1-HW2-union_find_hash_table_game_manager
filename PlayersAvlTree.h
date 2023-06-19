#ifndef AVLTREE_H
#define AVLTREE_H

#include "Node.h"
#include <iostream>


class PlayersAvlTree {
    private:
        Node* root;
        Node* max;


        static void inOrderForPlayer(Node* node, std::shared_ptr<Player>** array, int* index)
        {
            if (node == NULL)
                return;
            inOrderForPlayer(node->getLeft(), array, index);
            array[*index] = &(node->getInfo());
            *index = *index + 1;
            inOrderForPlayer(node->getRight(), array, index); 
        }

        //Calculating the max of the int
        static int maximum(int a, int b)
        {
            if(a>b)
            {
                return a;
            }
            return b;
        }

        //Doing a right rotation
        static void rightRotation(Node* B)
        {
            assert(B != NULL && B->getLeft() != NULL);
            Node* A = B->getLeft();
            Node* ar = A->getRight();
            B->setLeft(ar);
            if(ar != NULL)
            {
                ar->setUp(B);
            }
            A->setUp(B->getUp());
            A->setRight(B);
            B->setUp(A);

            //If A is not the root, update his father pointer to A
            if(A->getUp() != NULL)
            {
                if(A->getUp()->getRight() == B)
                {
                    A->getUp()->setRight(A);
                }
                else A->getUp()->setLeft(A);
            }

            //Updating A and B height
            B->updateHeight();
            A->updateHeight();

            //update B w
            B->nodeUpdateW();
            //update A w
            A->nodeUpdateW();

            //update B sum
            B->nodeUpdateSum();
            //update A sum
            A->nodeUpdateSum();
        }

        //Doing a left rotation
        static void leftRotation(Node* A)
        {
            assert(A != NULL && A->getRight() != NULL);
            Node* B = A->getRight();
            Node* bl = B->getLeft();
            A->setRight(bl);
            if(bl != NULL)
            {
                bl->setUp(A);
            }
            B->setUp(A->getUp());
            B->setLeft(A);
            A->setUp(B);
            //If Bis not the root, update his father pointer to B
            if(B->getUp() != NULL)
            {
                if(B->getUp()->getRight() == A)
                {
                    B->getUp()->setRight(B);
                }
                else B->getUp()->setLeft(B);
            }
            //Updating A and B height
            A->updateHeight();
            B->updateHeight();

            //update A w
            A->nodeUpdateW();
            //update w B
            B->nodeUpdateW();

            //update A sum
            A->nodeUpdateSum();
            //update B sum
            B->nodeUpdateSum();
        }

        //Arranges the tree
        static void MakeOrder(PlayersAvlTree* tree, Node* start)
        {
            if(start == NULL)
            {
                return;
            }
            if(start->getBF() == -2)
            {
                if(start->getRight()->getBF() <= 0)
                {
                    //RR rotation
                    leftRotation(start);
                } 
                else
                { 
                    //RL rotation
                    rightRotation(start->getRight());
                    leftRotation(start);
                }
            }
            else if(start->getBF() == 2)
            {
                if(start->getLeft()->getBF() >= 0)
                {
                    //LL rotation
                    rightRotation(start);
                }
                else
                { 
                    //LR rotation
                    leftRotation(start->getLeft());
                    rightRotation(start);
                }
            }

            start->nodeUpdateW();
            start->nodeUpdateSum();
            start->updateHeight();

            //Update to root if needed
            if(start->getUp() == NULL)
            {
                tree->setRoot(start);
                return;
            }
            MakeOrder(tree, start->getUp());
        }

        //Update the W for each node
        static void UpdateW(Node* start)
        {
            while(start !=NULL)
            {
                start->nodeUpdateW();
                start=start->getUp();
            }
        }

        //Getting the next node in order
        static Node* getNext(Node* node)
        {
            assert(node != NULL && node->getLeft() != NULL && node->getRight() != NULL);
            Node* ptr = node->getRight();
            while(ptr->getLeft() != NULL)
            {
                ptr = ptr->getLeft();
            }
            return ptr;
        }

        //Remove leaf from tree
        static void removeLeaf(PlayersAvlTree* tree, Node* V)
        {   
               Node* father = V->getUp();

               //Update the max if V is max
               if(tree->getMax()->getMainKey() == V->getMainKey() && tree->getMax()->getSecondaryKey() == V->getSecondaryKey() && tree->getMax()->getThirdKey() == V->getThirdKey())
               {
                   tree->setMax(father);
               }

               V->setUp(NULL);
               if(father !=NULL)
               {
                   if(father->getRight() == V)
                   {
                       father->setRight(NULL);
                   }
                   else if(father->getLeft() == V)
                   {
                       father->setLeft(NULL);
                   }
                   //Update father's w and sum
                   father->setW(father->getW()-1);
                   father->setSumLevels(father->getSumLevels() - V->getInfo()->getLevel());
               }
               //V is leaf and root
               else
               {
                   tree->setRoot(NULL);
               }
               delete V;
               MakeOrder(tree, father);
               return;
        }

        //Remove node with one child
        static void removeNodeWithOneChild(PlayersAvlTree* tree, Node* V)
        {
               //Get V child
               Node* son;
               if(V->getRight() != NULL)
               {
                   son = V->getRight();
               }
               else 
               {
                   son = V->getLeft();
               }

               //Update tree max if V is the max
               if(tree->getMax()->getMainKey() == V->getMainKey() && tree->getMax()->getSecondaryKey() == V->getSecondaryKey() && tree->getMax()->getThirdKey() == V->getThirdKey())
               {
                   tree->setMax(son);
               }

               //Get V father
               Node* father = V->getUp();
               if(father == NULL)
               {
                   tree->setRoot(son);
                   son->setUp(NULL);
                   delete V;
                   return;
               }
                if(V == father->getRight())
                {
                   father->setRight(son);
                }
                else
                {
                    father->setLeft(son);
                }
                son->setUp(father);
                //Update father's w and sum
                father->setW(father->getW()-1);
                father->setSumLevels(father->getSumLevels() - V->getInfo()->getLevel());

                delete V;
                MakeOrder(tree, father);
                return;
        }


        static void updatePostOrder(Node* start)
        {
            if(start == NULL)
            {
                return;
            }
            updatePostOrder(start->getLeft());
            updatePostOrder(start->getRight());

            start->updateHeight();
            start->nodeUpdateW();
            start->nodeUpdateSum();
        }


        //Destroy the tree
        static void DestroyTree(Node* node)
        {
            if(node==NULL)
            {
                return;
            }
            DestroyTree(node->getLeft());
            //node->setLeft(NULL);
            DestroyTree(node->getRight());
            //node->setRight(NULL);

            delete node;
        }

    public:

       //Constuctor
       PlayersAvlTree();

       //Defult distructor
       ~PlayersAvlTree();

       //Getting tree root
       Node* getRoot();
       
       //Getting max node of tree
       Node* getMax();

       //Setting tree's root to new_root
       void setRoot(Node* new_root);
       
       //Set tree's max to new_max
       void setMax(Node* new_max);

       PlayersAvlTree& operator=(const PlayersAvlTree& tree);

       //Insert a new node to tree
       void insert(int main_key, int secondary_key,int third_key, std::shared_ptr<Player>& val);

       //Search for a node
       Node* find(int main_key, int secondary_key, int third_key);

       //Search for closest node that smaller then searched (if there is no item smaller then searched return null)
       Node* searchMainKey(int main_key);
       Node* searchMainSecondKey(int main_key, int secondary_key);

       //Remove node from tree
       void remove(int main_key, int secondary_key, int third_key);

       std::shared_ptr<Player>** makeArrayForPlayers(int num);

       int getNodeRank(Node* node);

       int levelSumToNode(Node* node);

       void updateTree();

       int numPlayersInBound(int lower_main_key, int higher_main_key);
};

#endif