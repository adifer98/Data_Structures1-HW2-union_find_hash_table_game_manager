#include "Node.h"



Node::Node(std::shared_ptr<Player>& node_info, int node_main_key, int node_secondary_key, int node_third_key) :
info(node_info), main_key(node_main_key), secondary_key(node_secondary_key), third_key(node_third_key)
{
    height = 0;
    sum_levels = node_info->getLevel();
    w = 1;
    left = NULL;
    right = NULL;
    up = NULL;
}


Node::Node()
{
    main_key = 0;
    secondary_key = 0;
    third_key = 0;
    height = 0;
    w = 1;
    sum_levels = 0;
    info = NULL;
    left = NULL;
    right = NULL;
    up = NULL;
}


//Destructor 
Node::~Node()
{
    left = NULL;
    right = NULL;
    up = NULL;
}


int Node::getMainKey()
{
    return main_key;
}

int Node::getSecondaryKey()
{
    return secondary_key;
}

int Node::getThirdKey()
{
    return third_key;
}

Node* Node::getLeft()
{
    return left;
}

Node* Node::getRight()
{
    return right;
}

Node* Node::getUp()
{
    return up;
}

int Node::getHeight()
{
    return height;
}

int Node::getW()
{
    return w;
}

void Node::setW(int new_w)
{
    w=new_w;
}

void Node::setSumLevels(int new_sum)
{
    sum_levels=new_sum;
}

int Node::getSumLevels()
{
    return sum_levels;
}

std::shared_ptr<Player>& Node::getInfo()
{
    return info;
}

void Node::setHeight(int h)
{
    height = h;
}

void Node::setLeft(Node* node)
{
    left = node;
}

void Node::setRight(Node* node)
{
    right = node;
}

void Node::setUp(Node* node)
{
    up = node;
}

bool Node::nodeCmp(int node_main_key, int node_secondary_key, int node_third_key)
{
    if(main_key > node_main_key)
    {
        return true;
    }
    if(main_key < node_main_key)
    {
        return false;
    }
    if(secondary_key < node_secondary_key)
    {
        return false;
    }
    if(secondary_key > node_secondary_key)
    {
        return true;
    }
    if(third_key < node_third_key)
    {
        return false;
    }
    return true;
}


int nodeGetHighet(Node* node)
{
    if(node == NULL)
    {
       return -1;
    }
    return node->height;
}

bool Node::isLeaf()
{
    if(right == NULL && left == NULL)
    {
        return true;
    }
    return false;
}
    
//Check if the node has one child. Return true if so, false otherwise.
bool Node::hasOneChild()  
{
   if(right == NULL && left != NULL)
   {
       return true;
   }
   if(right != NULL && left == NULL)
   {
       return true;
   }
   return false;
}


//Return node's BF
int Node::getBF()
{
    return nodeGetHighet(left) - nodeGetHighet(right);
}


void Node::updateHeight()
{
    height = fmax(nodeGetHighet(left),nodeGetHighet(right)) + 1;
}


void Node::switchWithNode(Node* node)
{
    int temp1 = main_key;
    int temp2 = secondary_key;
    int temp3 = third_key;
    int temp4 = w;
    int temp5 = sum_levels;

    main_key = node->main_key;
    secondary_key = node->secondary_key;
    third_key = node->third_key;
    w = node->w;
    sum_levels = node->sum_levels - node->info->getLevel() + info->getLevel();

    node->main_key = temp1;
    node->secondary_key = temp2;
    node->third_key = temp3;
    node->w = temp4;
    node->sum_levels = temp5 - info->getLevel() + node->info->getLevel();

    info.swap(node->info);
}

void Node::AddHeight()
{
    height++;
}

void Node::nodeUpdateW()
{
    int leftw=0;
    int rightw=0;
    if(left != NULL)
    {
        leftw=left->getW();
    }
    if(right!=NULL)
    {
        rightw=right->getW();
    }

    w=leftw+rightw+1;
}

void Node::nodeUpdateSum()
{
    int leftSum=0;
    int rightSum=0;
    if(left !=NULL)
    {
        leftSum=left->getSumLevels();
    }
    if(right!=NULL)
    {
        rightSum=right->getSumLevels();
    }

    sum_levels = leftSum + rightSum + info->getLevel();
}


void Node::setInfo(std::shared_ptr<Player>& new_info)
{
    info = new_info;
}


void Node::setMainKey(int key)
{
    main_key = key;
}

void Node::setSecondaryKey(int key)
{
    secondary_key = key;
}

void Node::setThirdKey(int key)
{
    third_key = key;
}



Node* Select(Node* start, int k)
{
    if(start == NULL)
    {
        return NULL;
    }

    if(start->left != NULL)
    {
        int left_w = start->left->w;
        if(left_w == k-1)
        {
            return start;
        }
        if(left_w > k-1)
        {
            return Select(start->left, k);   
        }
        return Select(start->right, k - left_w - 1);
    }

    if(k - 1 == 0)
    {
        return start;
    }

    return Select(start->right, k - 1);
}
