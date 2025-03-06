#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FEE 0.0006


//Represents an order with all its attributes
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
    // Pointer (Adress) to the next node
    struct node *next;
};

typedef struct node Node;

/**
 * @brief Creates an order with the given parameters.
 *
 * @param takeprofit That price at which the order should be closed with profit.
 * @param stoploss That price at which the order should be closed with loss.
 * @return The created order.
 * 
 * @attention The profit is calculated with the fee included.
 */
Order createOrder(bool type, unsigned long int opentimestamp, double open, double takeprofit, double stoploss, double amount) {
    Order order;
    order.type = type;
    order.opentimestamp = opentimestamp;
    order.closetimestamp = 0;
    order.open = open;
    order.close = 0;
    order.takeprofit = takeprofit;
    order.stoploss = stoploss;
    order.amount = amount;
    order.profit = -order.open * order.amount * FEE;
    order.success = false;
    order.complete = false;
    return order;
}
/**
 * @brief Checks if the order should be closed and updates the order accordingly.
 * 
 * @param order The order to be checked.
 * @param timestamp The current timestamp.
 * @param high The highest price in the given timestamp.
 * @param low The lowest price in the given timestamp.
 * @return The updated order.
 * 
 * @note If the order is closed, the profit is calculated and the order is marked as complete.
 */
Order checkOrder(Order order, unsigned long int timestamp, double high, double low) {
    // Enters if the order is a long order
    if(order.type == true) {
        // First Checks if the stoploss is hit
        if(low <= order.stoploss) {
            order.closetimestamp = timestamp;
            order.close = order.stoploss;
            order.success = false;
            order.complete = true;
            order.profit += (order.close - order.open) * order.amount;
            order.profit -= order.close * order.amount * FEE;
        } else if(high >= order.takeprofit) {
            order.closetimestamp = timestamp;
            order.close = order.takeprofit;
            order.success = true;
            order.complete = true;
            order.profit += (order.close - order.open) * order.amount;
            order.profit -= order.close * order.amount * FEE;
        }
    // Enters if the order is a short order
    } else {
        // First Checks if the stoploss is hit
        if(high >= order.stoploss) {
            order.closetimestamp = timestamp;
            order.close = order.stoploss;
            order.success = false;
            order.complete = true;
            order.profit += (order.open - order.close) * order.amount;
            order.profit -= order.close * order.amount * FEE;
        }else if(low <= order.takeprofit) {
            order.closetimestamp = timestamp;
            order.close = order.takeprofit;
            order.success = true;
            order.complete = true;
            order.profit += (order.open - order.close) * order.amount;
            order.profit -= order.close * order.amount * FEE;
        }
    }

    return order;
}

/**
 * @brief Creates a new node with the given order.
 * 
 * @param order The order to be stored in the node.
 * @return The pointer to the created node.
 * 
 * @note The created node is allocated in the heap.
 * @note The next pointer of the created node is set to NULL.
 */
Node *createNode(Order order) {
    Node *new = (Node *)malloc(sizeof(Node));
    new->order = order;
    new->next = NULL;

    return new;
}

/**
 * @brief Pushes a node to the head of the linked list.
 * 
 * @param head The head of the linked list.
 * @param newNode The node to be pushed.
 * @return The new head of the linked list.
 */
Node *pushNode(Node *head, Node *newNode) {
    newNode->next = head;
    return newNode;
}

/**
 * @brief Deletes a node from the linked list.
 * 
 * @param head The head of the linked list.
 * @param delete The node to be deleted.
 * @return The new head of the linked list.
 * 
 * @note If the head is the node to be deleted, the head is updated otherwise it returns the same head.
 * @note The deleted node is freed from the heap.
 */
Node *deleteNode(Node *head, Node *delete) {
    if(head == delete) {
        Node *next = head->next;
        free(head);
        return next;
    } else {
        Node *tmp = head;
        while(tmp->next != delete) {
            tmp = tmp->next;
        }
        tmp->next = delete->next;
        free(delete);
        return head;
    }
}

/**
 * @brief Checks all the orders in the linked list and updates them accordingly.
 * 
 * @param headOrder The head of the linked list containing the orders to be checked.
 * @param headComplete The head of the linked list containing the completed orders.
 * @param timestamp The current timestamp.
 * @param high The highest price in the given timestamp.
 * @param low The lowest price in the given timestamp.
 * @return The new head of the linked list containing the completed orders.
 * 
 * @note The orders that are checked are moved to the headComplete linked list.
 * @note The orders that are checked are removed from the headOrder linked list.
 */
Node *checkOrders(Node *headOrder, Node *headComplete, unsigned long int timestamp, double high, double low) {
    Node *temporary = headOrder;
    while(temporary != NULL) {
        temporary->order = checkOrder(temporary->order, timestamp, high, low);
        if(temporary->order.complete == true) {
            Node *newOrderNode = createNode(temporary->order);     // Creates a new node with the order
            headComplete = pushNode(headComplete, newOrderNode);  // Pushes the new node to the head of the headComplete linked list
        }
        temporary = temporary->next;
    }
    return headComplete;
}

Node *deleteWaste(Node *head) {
    Node *temporary = head;
    Node *nextNode = NULL;

    while(temporary != NULL) {
        nextNode = temporary->next;
        if(temporary->order.complete == true) {
            head = deleteNode(head, temporary);
        }
        temporary = nextNode;
    }
    return head;
}


/**
 * @brief Deletes the full linked list.
 * 
 * @param head The head of the linked list.
 * 
 * @note The nodes are freed from the heap.
 */
void deleteFullList(Node **head) {
    Node *tmp = *head;
    while(tmp != NULL) {
        Node *tmp2 = tmp->next;
        free(tmp);
        tmp = tmp2;
    }

    *head = NULL;
}

/**
 * @brief Prints the order with all relevant attributes.
 */
void printOrder(Order order) {
    printf("[%s] Opentimestamp: %ld, Closetimestamp: %ld, Profit: %.2lf, Amount: %.2lf\n", order.type ? "LONG " : "SHORT", order.opentimestamp, order.closetimestamp, order.profit, order.amount);
}

/**
 * @brief Prints all the orders in the linked list.
 */
void printList(Node *head) {
    Node *tmp = head;
    while(tmp != NULL) {
        printOrder(tmp->order);
        tmp = tmp->next;
    }
    printf("List end\n ------------------\n");
}