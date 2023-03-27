#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n);
    void fixInsert(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void fixRemove(AVLNode<Key,Value>* n, char diff);
    virtual void deleteNode(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key,Value> &new_item)
{
    // TODO
    if(this->root_==nullptr){ //nothing in AVL
        this->root_=new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    //else
    AVLNode<Key,Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key,Value>* nextTemp;
    bool isLeft=false;
    while(temp!=nullptr){
        if(new_item.first <temp->getKey()){
            if(temp->getLeft()==nullptr){
                AVLNode<Key,Value>* leftNode =new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
                leftNode->setBalance(0);
                temp->setLeft(leftNode);
                leftNode->setParent(temp);
                nextTemp =leftNode;
                isLeft =true;
                break;
                
            }
            else{
                temp =temp->getLeft();
            }
        } else if(new_item.first > temp->getKey()){
            if(temp->getRight() == nullptr){
                AVLNode<Key,Value>* rightNode = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
                rightNode->setBalance(0);
                temp->setRight(rightNode);
                rightNode->setParent(temp);
                nextTemp = rightNode;
                isLeft = false;
                break;
            } else {
                temp = temp->getRight();
            }
        } else {
            temp->setValue(new_item.second);
            return;
        }
    }   
    if(isLeft){
        if(temp->getBalance()==-1 || temp->getBalance()==1){
            temp->setBalance(0);
        }
        else if(temp->getBalance()==0){
            temp->setBalance(-1);
            fixInsert(temp, nextTemp);
        }
    } 
    else {
        if(temp->getBalance() == -1 || temp->getBalance() == 1){
            temp->setBalance(0);
        }
        else if(temp->getBalance() == 0){
            temp->setBalance(1);
            fixInsert(temp, nextTemp);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::fixInsert(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
    if(p == nullptr || p->getParent() == nullptr)
        return;
    AVLNode<Key,Value>* g = p->getParent();
    if(g->getLeft() == p){
        //assume p is left child of g
        g->updateBalance(-1);
        if(g->getBalance() == 0){
            return;
        } else if (g->getBalance() == -1){
            fixInsert(g,p);
        } else if (g->getBalance() == -2){
            if(p->getBalance() <= 0){ //right rotation
                rotateRight(g);
                g->setBalance(0); p->setBalance(0);
            } else { //left rotation
                rotateLeft(p);
                rotateRight(g);
                if(n->getBalance() == -1){p->setBalance(0); g->setBalance(1); n->setBalance(0);}
                else if(n->getBalance() == 0){p->setBalance(0); g->setBalance(0); n->setBalance(0);}
                else if(n->getBalance() == 1){p->setBalance(-1); g->setBalance(0); n->setBalance(0);}
            }
        }
    } 
    else if(g->getRight() == p) {
        //assume p is right child of g
        g->updateBalance(1);
        if(g->getBalance() == 0){
            return;
        } else if (g->getBalance() == 1){
            fixInsert(g,p);
        } else if (g->getBalance() == 2){
            if(p->getBalance() >= 0){ //left rotation needed
                rotateLeft(g);
                g->setBalance(0); p->setBalance(0);
            } else { //
                rotateRight(p);
                rotateLeft(g);
                if(n->getBalance() == 1){
                    p->setBalance(0); 
                    g->setBalance(-1); n->setBalance(0);
                }
                else if(n->getBalance() == 0){
                    p->setBalance(0); 
                    g->setBalance(0); 
                    n->setBalance(0);
                }
                else if(n->getBalance() == -1){
                    p->setBalance(1); 
                    g->setBalance(0); 
                    n->setBalance(0);
                }
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n){
    AVLNode<Key,Value>* p = n->getParent();
    AVLNode<Key,Value>* c = n->getRight();
    //n right child to c left child
    n->setRight(c->getLeft());
    if(c->getLeft() != nullptr)
        c->getLeft()->setParent(n);
    //c left child to n
    c->setLeft(n);
    c->setParent(p);
    n->setParent(c);

    if(p == nullptr){ // is root
        this->root_ = c;
    } else {
        if(p->getLeft() == n){ //is left child of some parent
            p->setLeft(c);
        }
        else { //is right child of some parent
            p->setRight(c);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n){
    AVLNode<Key,Value>* p = n->getParent();
    AVLNode<Key,Value>* c = n->getLeft();
    //n left child to c right child
    n->setLeft(c->getRight());
    if(c->getRight() != nullptr)
        c->getRight()->setParent(n);
    //c right child to n
    c->setRight(n);
    c->setParent(p);

    n->setParent(c);

    if(p == nullptr){ // is root
        this->root_ = c;
    }
    else{
        if(p->getLeft() == n){ //is left child of some parent
            p->setLeft(c);
        }
        else { //is right child of some parent
            p->setRight(c);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key,Value>* n =static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(n ==nullptr){
        return;
    }
    if(n->getLeft() != nullptr && n->getRight() != nullptr){
        nodeSwap(n, static_cast<AVLNode<Key, Value>*>(this->predecessor(n)));
    }
    AVLNode<Key,Value>* p =n->getParent();
    char diff =0;
    if(p !=nullptr){
        if(p->getLeft() ==n){ //is a left child 
            diff =1;
        } else { //is a right child 
            diff =-1;
        }
    }
    //delete and update
    deleteNode(n);
    //check for fixes after remove
    fixRemove(p, diff);
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::fixRemove(AVLNode<Key,Value>* n, char diff){
    if(n ==nullptr)
        return;
    AVLNode<Key,Value>* p =n->getParent();
    char nextNumBal =0;
    if(p !=nullptr){
        if(p->getLeft() ==n){ //is left child of some parent
            nextNumBal =1;
        } else { //is right child of some parent
            nextNumBal =-1;
        }
    }
    if(diff ==-1){
        if(n->getBalance()+diff ==-2){
            AVLNode<Key,Value>* c = n->getLeft();
            if(c->getBalance() == -1) { rotateRight(n); n->setBalance(0); c->setBalance(0); fixRemove(p, nextNumBal);}
            else if (c->getBalance() == 0) { rotateRight(n); n->setBalance(-1); c->setBalance(1); return;}
            else if (c->getBalance() == 1) { 
                AVLNode<Key,Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n); 
                if(g->getBalance() == 1){n->setBalance(0); c->setBalance(-1); g->setBalance(0);}
                else if(g->getBalance() == 0){n->setBalance(0); c->setBalance(0); g->setBalance(0);}
                else if(g->getBalance() == -1){n->setBalance(1); c->setBalance(0); g->setBalance(0);}
                fixRemove(p, nextNumBal);
            }
        } else if(n->getBalance() + diff == -1){
            n->setBalance(-1);
            return;
        } else if(n->getBalance() + diff == 0){
            n->setBalance(0);
            fixRemove(p, nextNumBal);
        }
    } else if(diff == 1){
        if(n->getBalance() + diff == 2){
            AVLNode<Key,Value>* c = n->getRight();
            if(c->getBalance() == 1) { rotateLeft(n); n->setBalance(0); c->setBalance(0); fixRemove(p, nextNumBal);}
            else if (c->getBalance() == 0) { rotateLeft(n); n->setBalance(1); c->setBalance(-1); return;}
            else if (c->getBalance() == -1) { 
                AVLNode<Key,Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n); 
                if(g->getBalance() == -1){n->setBalance(0); c->setBalance(1); g->setBalance(0);}
                else if(g->getBalance() == 0){n->setBalance(0); c->setBalance(0); g->setBalance(0);}
                else if(g->getBalance() == 1){n->setBalance(-1); c->setBalance(0); g->setBalance(0);}
                fixRemove(p, nextNumBal);
            }
        } else if(n->getBalance() + diff == 1){
            n->setBalance(1);
            return;
        } else if(n->getBalance() + diff == 0){
            n->setBalance(0);
            fixRemove(p, nextNumBal);
        }
    }
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::deleteNode(AVLNode<Key, Value>* node){
    if((node->getLeft() == nullptr) && (node->getRight() == nullptr)){ //leaf
        if(node == this->root_){
            this->root_ = nullptr;
            delete node;
            return;
        }
        if(node->getParent()->getLeft() == node){
            node->getParent()->setLeft(nullptr);
        } else {
            node->getParent()->setRight(nullptr);
        }
        delete node;
    } else if((node->getLeft() != nullptr) && (node->getRight() != nullptr)){ //two kids
        AVLNode<Key,Value>* temp = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, temp);
        deleteNode(node);
    } else {
        AVLNode<Key,Value>* temp = node;
        if(node->getLeft() != nullptr){
            node->getLeft()->setParent(node->getParent());
            if(node->getParent() != nullptr){
                if(node->getParent()->getLeft() == node){
                    node->getParent()->setLeft(node->getLeft());
                } else {
                    node->getParent()->setRight(node->getLeft());
                }
            } else {
                this->root_ = node->getLeft();
            }
        } else {
            node->getRight()->setParent(node->getParent());
            if(node->getParent() != nullptr){
                if(node->getParent()->getLeft() == node){
                    node->getParent()->setLeft(node->getRight());
                } else {
                    node->getParent()->setRight(node->getRight());
                }
            } else {
                this->root_ = node->getRight();
            }
        }
        delete temp;
    }
}




template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
