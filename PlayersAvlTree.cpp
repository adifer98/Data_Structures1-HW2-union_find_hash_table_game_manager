#include "PlayersAvlTree.h"

PlayersAvlTree::PlayersAvlTree()
{
    root=NULL;
    max=NULL;
}

PlayersAvlTree::~PlayersAvlTree()
{
    DestroyTree(root);
}

Node* PlayersAvlTree::getRoot()
{
    return root;
}

Node* PlayersAvlTree::getMax()
{
    return max;
}

void PlayersAvlTree::setRoot(Node* new_root)
{
    root = new_root;
}

void PlayersAvlTree::setMax(Node* new_max)
{
    max = new_max;
}

PlayersAvlTree& PlayersAvlTree::operator=(const PlayersAvlTree& tree)
{
    if(this == &tree)
    {
        return *this;
    }
    root = tree.root;
    max = tree.max;
    return *this;
}

void PlayersAvlTree::insert(int main_key, int secondary_key,int third_key, std::shared_ptr<Player>& val)
       { 
           //If the tree is empty
           if(root == NULL)
           {
                try
                {
                    root = new Node(val, main_key, secondary_key,third_key);
                }catch(const std::bad_alloc& e)
                {
                    throw e;
                }
               max = root;
               return;
           }

           //If tree is not empty
           Node* ptr = root;
           bool is_right = false;
           while(ptr != NULL)
           {
               ptr->AddHeight();
               ptr->setSumLevels(ptr->getSumLevels()+val->getLevel());
               if(ptr->nodeCmp(main_key, secondary_key,third_key))
               {
                   
                   if(ptr->getLeft()==NULL)
                   {
                       is_right = false;
                       break;
                   }
                   ptr = ptr->getLeft();
               }
               else
               {
                   if(ptr->getRight()==NULL)
                   {
                       is_right = true;
                       break;
                   }
                   ptr = ptr->getRight();
               }
           }
           //Set the new node
           if(is_right)
           {
               try
                {
                    ptr->setRight(new Node(val,main_key,secondary_key,third_key));
                }catch(const std::bad_alloc& e)
                {
                    throw e;
                }
                ptr->getRight()->setUp(ptr);
                ptr = ptr->getRight();
           }
           else 
           {
               try
                {
                    ptr->setLeft(new Node(val,main_key,secondary_key,third_key));
                }catch(const std::bad_alloc& e)
                {
                    throw e;
                }
                ptr->getLeft()->setUp(ptr);
                ptr = ptr->getLeft();
           }

           //Update Max
           if(max->nodeCmp(main_key, secondary_key,third_key) == false)
           {
               max = ptr;
           }
           UpdateW(ptr->getUp());
           MakeOrder(this, ptr);
       }

Node* PlayersAvlTree::find(int main_key, int secondary_key, int third_key)
{
    Node* ptr = root;
    while(ptr != NULL)
    {
        if(ptr->getMainKey() == main_key && ptr->getSecondaryKey() == secondary_key && ptr->getThirdKey() == third_key)
        {
            return ptr;
        }

        if(ptr->nodeCmp(main_key, secondary_key, third_key))
        {
            ptr = ptr->getLeft();
        }
        else
        {
            ptr = ptr->getRight();
        }
    }
    return NULL;
}

Node* PlayersAvlTree::searchMainKey(int main_key)
{
    Node* ptr = root;
    Node* result = NULL;
    while(ptr != NULL)
    {
        //If ptr is grater then searched item
        if(ptr->getMainKey() >= main_key)
        {
            ptr = ptr->getLeft();
        }
        //If ptr is smaller then searched item
        else
        {
            //If there is no item grater then ptr
            if(ptr->getRight()==NULL)
            {
                result = ptr;
            }
            else
            {
                //If ptr right son is greater then searched item
                if(ptr->getRight()->getMainKey() >= main_key)
                {
                    result = ptr;
                }
            }
            ptr = ptr->getRight();
        }
    }
    return result;
}

Node* PlayersAvlTree::searchMainSecondKey(int main_key, int secondary_key)
{
    Node* ptr = root;
    Node* result = NULL;
    while(ptr != NULL)
    {
        //If ptr is grater then searched item
        if(ptr->getMainKey() > main_key)
        {
            ptr = ptr->getLeft();
        }

        else if(ptr->getMainKey() < main_key)
        {
            if(ptr->getRight()==NULL || ptr->getRight()->getMainKey() > main_key || (ptr->getRight()->getMainKey() == main_key && ptr->getRight()->getSecondaryKey() >= secondary_key))
            {
                result = ptr;
            }          
            ptr = ptr->getRight();
        }

        else if(ptr->getMainKey() == main_key)
        {
            if(ptr->getSecondaryKey() >= secondary_key)
            {
                ptr = ptr->getLeft();
            }
            else
            {
                if(ptr->getRight()==NULL || ptr->getRight()->getMainKey() > main_key || (ptr->getRight()->getMainKey() == main_key && ptr->getRight()->getSecondaryKey() >= secondary_key))
                {
                    result = ptr;
                }
                ptr = ptr->getRight();
            }
        }
    }
    return result;
}

void PlayersAvlTree::remove(int main_key, int secondary_key, int third_key)
       {
           //Search if the node exist
           Node* V = this->find(main_key, secondary_key,third_key);
           if(V == NULL)
           {
               return;
           }

           //If V is leaf - remove V
           if(V->isLeaf())
           {
               removeLeaf(this,V);
               return;
           }

           //If V has one child - replace V with his son
           if(V->hasOneChild())
           {
               removeNodeWithOneChild(this,V);
               return;
           }
            
            //Replace V with the next node in order (next must be a leaf of has one child)
            Node* next = getNext(V);
            V->switchWithNode(next);
            //If next is leaf - remove next
           if(next->isLeaf())
           {
               removeLeaf(this,next);
               return;
           }

           //If next has one child - replace next with his son
           if(next->hasOneChild())
           {
               removeNodeWithOneChild(this,next);
               return;
           }
       }

std::shared_ptr<Player>** PlayersAvlTree::makeArrayForPlayers(int num)
{
    if(num <= 0)
    {
        return NULL;
    }
    std::shared_ptr<Player>** array;
    try{
        array = (std::shared_ptr<Player>**)malloc(num * sizeof(std::shared_ptr<Player>*));
    } catch(const std::bad_alloc& e)
    {
        throw e;
    }
    int index = 0;
    inOrderForPlayer(root, array, &index);
    return array;
}

int PlayersAvlTree::getNodeRank(Node* node)
{
    int r=0;
    Node* ptr=root;

    //If the node doesnt exist in tree
    if(node==NULL)
    {
        return -1;
    }
    
    while(ptr!=NULL)
    {
        if(node->getMainKey()==ptr->getMainKey() && node->getSecondaryKey()==ptr->getSecondaryKey() && node->getThirdKey()==ptr->getThirdKey())
        {
            int leftw=0;
            if(ptr->getLeft()!=NULL)
            {
                leftw = ptr->getLeft()->getW();
            }
            return r+leftw+1;
        }
        if(node->nodeCmp(ptr->getMainKey(),ptr->getSecondaryKey(),ptr->getThirdKey()))
        {
            int leftw=0;
            if(ptr->getLeft()!=NULL)
            {
                leftw = ptr->getLeft()->getW();
            }
            r=r+leftw+1;

            ptr=ptr->getRight();
        }
        else
        {
            ptr=ptr->getLeft();
        }
    }
    return -1;
}

int PlayersAvlTree::levelSumToNode(Node* node)
{
    int sum=0;
    Node* ptr=root;

    //If the node doesnt exist in tree
    if(node==NULL)
    {
        return -1;
    }

    while(ptr!=NULL)
    {
        if(node->getMainKey()==ptr->getMainKey() && node->getSecondaryKey()==ptr->getSecondaryKey() && node->getThirdKey()==ptr->getThirdKey())
        {
            int leftsum=0;
            if(ptr->getLeft()!=NULL)
            {
                leftsum = ptr->getLeft()->getSumLevels();
            }
            return sum + ptr->getInfo()->getLevel() + leftsum;
        }
        if(node->nodeCmp(ptr->getMainKey(),ptr->getSecondaryKey(),ptr->getThirdKey()))
        {
            int leftsum=0;
            if(ptr->getLeft()!=NULL)
            {
                leftsum = ptr->getLeft()->getSumLevels();
            }
            sum = sum + ptr->getInfo()->getLevel() + leftsum;

            ptr=ptr->getRight();
        }
        else
        {
            ptr=ptr->getLeft();
        }
    }
    return -1;
}



void PlayersAvlTree::updateTree()
{
    if(root == NULL)
    {
        return;
    }
    Node* ptr = root;
    updatePostOrder(ptr);
}


int PlayersAvlTree::numPlayersInBound(int lower_main_key, int higher_main_key)
{
    if(higher_main_key <= 0 || lower_main_key > higher_main_key)
    {
        return 0;
    }
    Node* left_node = this->searchMainKey(lower_main_key);
    int left_index=0;
    if(left_node !=NULL)
    {
        left_index = this->getNodeRank(left_node);
    }
    Node* right_node = this->searchMainKey(higher_main_key+1);
    int right_index=0;
    if(right_node !=NULL)
    {
        right_index = this->getNodeRank(right_node);
    }

    int player_in_bound_num = (right_index-left_index);
    return player_in_bound_num;
}


