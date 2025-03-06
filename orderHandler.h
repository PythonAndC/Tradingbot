#ifndef ORDERHANDLER_H
#define ORDERHANDLER_H

typedef struct {
    // Order type: true for buy, false for sell
    bool type;
    unsigned long int opentimestamp;
    unsigned long int closetimestamp;
    double open;
    double close;
    double takeprofit;
    double stoploss;
    double amount;
    double profit;
    // Order status: true for success, false for failure
    bool success;
    // Order status: true for complete, false for incomplete
    bool complete;
} Order;

struct node {
    Order order;
    struct node *next;
};

typedef struct node Node;

Order createOrder(bool type, unsigned long int opentimestamp, double open, double takeprofit, double stoploss, double amount);
Node *createNode(Order order);
Node *pushNode(Node *head, Node *newNode);
Node *checkOrders(Node *headOrder, Node *headComplete, unsigned long int timestamp, double high, double low);
Node *deleteWaste(Node *head);
void deleteFullList(Node **head);
void printList(Node *head);
#endif