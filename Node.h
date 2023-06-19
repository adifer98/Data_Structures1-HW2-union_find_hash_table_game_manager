#ifndef NODE_H
#define NODE_H

#include <memory>
#include <cassert>
#include <iostream>
#include <math.h> 
#include "Player.h"

class Node {
    private:
        std::shared_ptr<Player> info;
        Node* up;
        Node* left;
        Node* right;
        int main_key;
        int secondary_key;
        int third_key;
        int w;
        int sum_levels;
        int height;
    
    public:
        Node(std::shared_ptr<Player>& node_info, int node_main_key, int node_secondary_key, int node_third_key);
        Node();
        ~Node();
        int getMainKey();
        int getSecondaryKey();
        int getThirdKey();
        void setMainKey(int key);
        void setSecondaryKey(int key);
        void setThirdKey(int key);
        Node* getLeft();
        Node* getRight();
        Node* getUp();
        int getHeight();
        int getW();
        void setW(int new_w);
        void nodeUpdateW();
        int getSumLevels();
        void setSumLevels(int new_sum);
        void nodeUpdateSum();
        void setHeight(int h);
        void setLeft(Node* node);
        void setRight(Node* node);
        void setUp(Node* node);
        std::shared_ptr<Player>& getInfo();
        void setInfo(std::shared_ptr<Player>& new_info);
        bool nodeCmp(int node_main_key, int node_secondary_key, int node_third_key);
        int getBF();
        bool isLeaf();
        bool hasOneChild();
        void switchWithNode(Node* node);
        void updateHeight();
        friend int nodeGetHighet(Node* node);
        void AddHeight();
        friend Node* Select(Node* start, int k);
};

#endif