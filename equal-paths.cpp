#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here



bool equalPaths(Node * root)
{
    if(root==nullptr){ //tree is empty
        return true;
    }
    else if(root->left==nullptr&&root->right!=nullptr){
        root=root->right;
        equalPaths(root);//scoot if needed
    }
    else if(root->left!=nullptr &&root->right==nullptr){
        root=root->left;
        equalPaths(root); //scoot
    }
    else if(branchHeight(root->left)==branchHeight(root->right)){
        return true; //both sides have same path lengths
    }
    else{
        return false;
    }
    return false;
}

int branchHeight(Node * root){
	if(root==nullptr){
		return 0;
	}
	else if(branchHeight(root->left) >= branchHeight(root->right)){
	    return branchHeight(root->left)+1; 
	}
	else{
		return branchHeight(root->right)+1;
	}
}