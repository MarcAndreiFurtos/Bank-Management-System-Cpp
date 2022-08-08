#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <thread>

// initializing linked list class

class Node {
public:
    int account_number;
    std::string holder_name;
    int account_sum;
    std::string account_password;
    Node* next;
public:
    Node(int acc_nr , std::string hold_name , std::string acc_pass);
    
};

// list node manipulation functions
namespace Funk {

    void add_before_head(Node*& head, int acc_nr, char* hold_name, char* acc_pass);
    bool check_existance(Node* head, int acc_nr, char* hold_name, char* acc_pass);

}