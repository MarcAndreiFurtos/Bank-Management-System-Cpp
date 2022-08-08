#include "b_header.h"
#include "banking_interface.h"

//constructing linked list and checking for an existing account

Node::Node(int acc_nr , std::string hold_name , std::string acc_pass) {
    account_number = acc_nr;
    holder_name = hold_name;
    account_sum = 50000;
    account_password = acc_pass;
    next = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Funk::add_before_head(Node*& head , int acc_nr , char* hold_name , char* acc_pass) {
    Node* newnode = new Node(acc_nr , hold_name , acc_pass);
    newnode->next = head;
    head = newnode;
}

bool Funk::check_existance(Node* head , int acc_nr , char* hold_name , char* acc_pass) {
    Node* n = new Node(acc_nr, hold_name , acc_pass);
    for (n = head; n != NULL; n = n->next) {
        if (n->holder_name == hold_name) {
            return true; 
        }
        else {
            return false;
        }

    }
}

