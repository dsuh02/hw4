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
    int8_t getBalance () const;
    void setBalance (int8_t balance);
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
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>* n, char diff);
    virtual void removeNode(AVLNode<Key, Value>* node);
    bool isLeftChild(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    bool isRightChild(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_==nullptr){ //nothing in AVL
        this->root_=new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    //else
    AVLNode<Key,Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key,Value>* temp2;
    bool addedLeft;
    while(temp != nullptr){
        if(new_item.first < temp->getKey()){
            if(temp->getLeft() == nullptr){
                AVLNode<Key,Value>* leftChild = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
                leftChild->setBalance(0);
                temp->setLeft(leftChild);
                leftChild->setParent(temp);
                temp2 = leftChild;
                addedLeft = true;
                break;
                
            } else {
                temp = temp->getLeft();
            }
        } else if(new_item.first > temp->getKey()){
            if(temp->getRight() == nullptr){
                AVLNode<Key,Value>* rightChild = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
                rightChild->setBalance(0);
                temp->setRight(rightChild);
                rightChild->setParent(temp);
                temp2 = rightChild;
                addedLeft = false;
                break;
            } else {
                temp = temp->getRight();
            }
        } else {
            temp->setValue(new_item.second);
            return;
        }
    }   
    if(addedLeft){
        if(temp->getBalance()==-1 || temp->getBalance()==1){
            temp->setBalance(0);
        }
        else if(temp->getBalance()==0){
            temp->setBalance(-1);
            insertFix(temp, temp2);
        }
    } 
    else {
        if(temp->getBalance() == -1 || temp->getBalance() == 1){
            temp->setBalance(0);
        }
        else if(temp->getBalance() == 0){
            temp->setBalance(1);
            insertFix(temp, temp2);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
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
            insertFix(g,p);
        } else if (g->getBalance() == -2){
            if(p->getBalance() <= 0){ //right rotation
                rotateRight(g);
                g->setBalance(0); p->setBalance(0);
            } else { //
                rotateLeft(p);
                rotateRight(g);
                if(n->getBalance() == -1){p->setBalance(0); g->setBalance(1); n->setBalance(0);}
                else if(n->getBalance() == 0){p->setBalance(0); g->setBalance(0); n->setBalance(0);}
                else if(n->getBalance() == 1){p->setBalance(-1); g->setBalance(0); n->setBalance(0);}
            }
        }
    } else if(g->getRight() == p) {
        //assume p is right child of g
        g->updateBalance(1);
        if(g->getBalance() == 0){
            return;
        } else if (g->getBalance() == 1){
            insertFix(g,p);
        } else if (g->getBalance() == 2){
            if(p->getBalance() >= 0){ //left rotation needed
                rotateLeft(g);
                g->setBalance(0); p->setBalance(0);
            } else { //
                rotateRight(p);
                rotateLeft(g);
                if(n->getBalance() == 1){p->setBalance(0); g->setBalance(-1); n->setBalance(0);}
                else if(n->getBalance() == 0){p->setBalance(0); g->setBalance(0); n->setBalance(0);}
                else if(n->getBalance() == -1){p->setBalance(1); g->setBalance(0); n->setBalance(0);}
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
    n->setParent(c);

    c->setParent(p);

    if(p == nullptr){ // is root
        this->root_ = c;
    } else {
        if(p->getLeft() == n){ //is left child of some parent
            p->setLeft(c);
        } else { //is right child of some parent
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
    n->setParent(c);

    c->setParent(p);

    if(p == nullptr){ // is root
        this->root_ = c;
    } else {
        if(p->getLeft() == n){ //is left child of some parent
            p->setLeft(c);
        } else { //is right child of some parent
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
    AVLNode<Key,Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(n == nullptr){
        return;
    }
    if(n->getLeft() != nullptr && n->getRight() != nullptr){
        nodeSwap(n, static_cast<AVLNode<Key, Value>*>(this->predecessor(n)));
    }
    AVLNode<Key,Value>* p = n->getParent();
    char diff = 0;
    if(p != nullptr){
        if(p->getLeft() == n){ //is a left child 
            diff = 1;
        } else { //is a right child 
            diff = -1;
        }
    }
    //delete and update
    removeNode(n);
    //check for fixes after remove
    removeFix(p, diff);
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, char diff){
    if(n == nullptr)
        return;
    AVLNode<Key,Value>* p = n->getParent();
    char ndiff = 0;
    if(p != nullptr){
        if(p->getLeft() == n){ //is left child of some parent
            ndiff = 1;
        } else { //is right child of some parent
            ndiff = -1;
        }
    }
    if(diff == -1){
        if(n->getBalance() + diff == -2){
            AVLNode<Key,Value>* c = n->getLeft();
            if(c->getBalance() == -1) { rotateRight(n); n->setBalance(0); c->setBalance(0); removeFix(p, ndiff);}
            else if (c->getBalance() == 0) { rotateRight(n); n->setBalance(-1); c->setBalance(1); return;}
            else if (c->getBalance() == 1) { 
                AVLNode<Key,Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n); 
                if(g->getBalance() == 1){n->setBalance(0); c->setBalance(-1); g->setBalance(0);}
                else if(g->getBalance() == 0){n->setBalance(0); c->setBalance(0); g->setBalance(0);}
                else if(g->getBalance() == -1){n->setBalance(1); c->setBalance(0); g->setBalance(0);}
                removeFix(p, ndiff);
            }
        } else if(n->getBalance() + diff == -1){
            n->setBalance(-1);
            return;
        } else if(n->getBalance() + diff == 0){
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    } else if(diff == 1){
        if(n->getBalance() + diff == 2){
            AVLNode<Key,Value>* c = n->getRight();
            if(c->getBalance() == 1) { rotateLeft(n); n->setBalance(0); c->setBalance(0); removeFix(p, ndiff);}
            else if (c->getBalance() == 0) { rotateLeft(n); n->setBalance(1); c->setBalance(-1); return;}
            else if (c->getBalance() == -1) { 
                AVLNode<Key,Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n); 
                if(g->getBalance() == -1){n->setBalance(0); c->setBalance(1); g->setBalance(0);}
                else if(g->getBalance() == 0){n->setBalance(0); c->setBalance(0); g->setBalance(0);}
                else if(g->getBalance() == 1){n->setBalance(-1); c->setBalance(0); g->setBalance(0);}
                removeFix(p, ndiff);
            }
        } else if(n->getBalance() + diff == 1){
            n->setBalance(1);
            return;
        } else if(n->getBalance() + diff == 0){
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::removeNode(AVLNode<Key, Value>* node){
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
        removeNode(node);
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
