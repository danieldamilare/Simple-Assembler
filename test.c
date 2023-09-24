#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the customer structure
struct Customer {
    char name[51];
    int num_tickets;
    int arrival_time;
    int checkout_time;
};

// Define the node structure for the linked list
struct Node {
    struct Customer *customer;
    struct Node *next;
};

// Define the queue structure
struct Queue {
    struct Node *front;
    struct Node *rear;
    int size;
};

// Function to create a new customer
struct Customer *createCustomer(char name[], int num_tickets, int arrival_time) {
    struct Customer *newCustomer = (struct Customer *)malloc(sizeof(struct Customer));
    strcpy(newCustomer->name, name);
    newCustomer->num_tickets = num_tickets;
    newCustomer->arrival_time = arrival_time;
    newCustomer->checkout_time = 0; // Initialize checkout time to 0
    return newCustomer;
}

// Function to create a new node for the linked list
struct Node *createNode(struct Customer *customer) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->customer = customer;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize a queue
struct Queue *createQueue() {
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    queue->front = queue->rear = NULL;
    queue->size = 0;
    return queue;
}

// Function to enqueue a customer
void enqueue(struct Queue *queue, struct Customer *customer) {
    struct Node *newNode = createNode(customer);
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

// Function to dequeue a customer
struct Customer *dequeue(struct Queue *queue) {
    if (queue->front == NULL) {
        return NULL; // Queue is empty
    }
    struct Node *temp = queue->front;
    struct Customer *customer = temp->customer;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL; // If the last customer is dequeued
    }
    free(temp);
    queue->size--;
    return customer;
}

// Function to check if the queue is empty
int isEmpty(struct Queue *queue) {
    return (queue->front == NULL);
}

// Function to get the front customer without dequeuing
struct Customer *peek(struct Queue *queue) {
    if (queue->front == NULL) {
        return NULL; // Queue is empty
    }
    return queue->front->customer;
}

int main() {
    int n, b;
    scanf("%d %d", &n, &b);

    struct Queue *queues[13]; // 12 queues, indexed from 1 to 12
    for (int i = 1; i <= 12; i++) {
        queues[i] = createQueue();
    }

    for (int i = 0; i < n; i++) {
        char name[51];
        int num_tickets, arrival_time;
        scanf("%s %d %d", name, &num_tickets, &arrival_time);

        struct Customer *customer = createCustomer(name, num_tickets, arrival_time);

        // Calculate the queue number based on the rules you've described
        int p = name[0] - 'A' + 1; // Convert character to queue number
        int k = 0; // Number of non-empty queues

        // Iterate through all queues to find non-empty ones
        for (int j = 1; j <= 12; j++) {
            if (!isEmpty(queues[j])) {
                k++;
            }
        }

        // Calculate the queue number for the customer
        int queue_number;
        if (k <= b) {
            queue_number = p; // Assign based on initial letter
        } else {
            int customers_per_booth = k / b;
            int booths_with_extra_customer = k % b;

            if (p <= (booths_with_extra_customer * (customers_per_booth + 1))) {
                queue_number = (p - 1) / (customers_per_booth + 1) + 1;
            } else {
                queue_number = (p - (booths_with_extra_customer * (customers_per_booth + 1))) / customers_per_booth + booths_with_extra_customer + 1;
            }
        }

        // Enqueue the customer into the appropriate queue
        enqueue(queues[queue_number], customer);
    }

    int current_time = 0;

    for (int booth_number = 1; booth_number <= b; booth_number++) {
        printf("Booth %d\n", booth_number);

        while (!isEmpty(queues[booth_number])) {
            struct Customer *customer = dequeue(queues[booth_number]);
            int processing_time = 30 + customer->num_tickets * 5;

            if (current_time < customer->arrival_time) {
                current_time = customer->arrival_time;
            }

            customer->checkout_time = current_time + processing_time;
            current_time = customer->checkout_time;

            printf("%s from line %d checks out at time %d.\n", customer->name, booth_number, customer->checkout_time);
        }

        printf("\n");
    }

    return 0;
}