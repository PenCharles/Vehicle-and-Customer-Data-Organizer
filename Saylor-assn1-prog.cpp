//------------------------------------------------------------------------------
// CODE FILENAME: SaylorAssn1.cpp
// DESCRIPTION: Stack and queues are used to sort and store the vehicles and
//				customers in the car rental program.
//				This program reads in a list of vehicles and sorts them by size 
//				into two lists. The vehicles will be used for rentals
//				The program then reads in data from another file which are either 
//				customers looking to rent a vehicle or a vehicle returning to the
//				fleet. The program will match the customers to a vehicle until 
//				there are no more customers or there are no more vehicles. If the 
//				latter is true then the program will place the customers into a 
//				waiting list. The program will display to the user what vehicles 
//				are left in the stack and what customers have been assigned to 
//				vehicles. The program will also display what vehicles have been 
//				returned to the stack and how many customers are waiting in the 
//				queues.
// CLASS & TERM: CS372 Summer 8-week 1
// DESIGNER: James Saylor
// FUNCTIONS:	createEmptyStack - creates and initializes a vehicle stack
//				isEmptyStack - checks to see if stack is empty
//				isFullStack - checks to see if stack is full
//				getStackCount - returns the length of the list
//				push - adds a node to the stack
//				pop - removes a node from the stack
//				createEmptyQueue - creates and initializes a waiting queue
//				isEmptyQueue - checks to see if the queue is empty
//				isFullQueue - checks to see if the queue is full
//				enQueue - adds a node to the queue
//				deQueue - deletes a node from the queue
//				errorMessage - displays an error message to the user
//				checkoutCar - checks out a car
//				checkoutVan - checks out a van
//				returnCar - places the car back into the stack
//				returnVan - places the van back into the stack
//				generalCheckout - assigns the vehicle to a customer
//------------------------------------------------------------------------------

#include <iostream>						// I/O purposes	
#include <iomanip>						// manipulating I/O 
#include <cstdlib>						// general utilities
#include <string>						// using string data types
#include <fstream>						// used to relate to data from files
#include <cstddef>						// defines the NULL function
using namespace std;

//global constants
const string LOTSTATUS = "LotStatus.txt";	//filename that holds data after program runs

//global structure definition
struct stackRec
{
	string vehicleType;		//a single letter that denotes Auto or Van
	string license;			//license plate AAA123
	int capacity;			//amount of room for people in each vehicle
	stackRec *nextVehicle;	//pointer to the next vehicle
};

struct stack
{
	int count;		//holds the count of each vehicle stack
	stackRec *top;	//poiints to the top of the stack
};

struct queueRec
{
	int numWaiting;		//holds the count of people waiting in each node
	queueRec *nextNode;	//pointer to the next node
};

struct queue
{
	int count;			//holds the count of nodes in each stack
	queueRec *front;	//pointer to the next vehicle
	queueRec *back;		//pointer to the previous vehicle
};

//function protoypes
stack* createEmptyStack ();
bool isEmptyStack (stack* s);
bool isFullStack ();
int getStackCount (int nodeCount);
void push (stackRec *newNode, stack *s);
stackRec* pop (stack *s);
queue* createEmptyQueue ();
bool isEmptyQueue (queue *q);
bool isFullQueue ();
void enQueue (queueRec *newNode, queue *q);
void deQueue (queue *q);
int errorMessage (string error);
void checkoutCar (stack *cars, int& personCount);
void checkoutVan (stack *vans, int& personCount);
bool returnCar (string licensePlate, int passengerCount, stack *cars);
bool returnVan (string licensePlate, int passengerCount, stack *vans);
bool generalCheckout (stack *cars, stack *vans, queue *line, int& personCount);

//------------------------------------------------------------------------------
// FUNCTION: main
// DESCRIPTION: Displays the amount of employees waiting for a vehicle. The 
//				program then assigns a group of employees to as many vehicles as 
//				needed until the groups have been taken care of or the rental
//				vehicles run out of choices. Also, the program will take returns
//				and assign the left over groups of employees to vehicles as they
//				are returned. The program will then display the parking lot status
//				with automobiles available and amount of people waiting for a ride
// OUTPUT:
//		Returns: 0 on success and 1 when checkout of a vehicle is not successful
// CALLS TO: createEmptyStack, createEmptyQueue, errorMessage, push, generalCheckout,
//			 returnCar, returnVan, isEmptyQueue, pop
//------------------------------------------------------------------------------
int main (int argc, char *argv[])
{
	string currentLine;
	string vehicle;							//holds the vehicle type read from the file
	string licensePlate;					//holds the license plate read from the file
	string tempVehicleType;					//holds the vehicle type before writing to file
	string tempLicense;						//holds the license plate before writing to file
	int number;								//holds the capacity of each vehicle
	int personCount;						//holds the amount of customers in each group
	int passengerCount;						//holds the passenger capacity in each returning vehicle
	int tempCapacity;						//holds the amount of people a vehicle can carry
	int nodeCountCars;						//holds the amount of cars in the stack
	int nodeCountVans;						//holds the amount of vans in the stack
	string transaction;						//holds whether the data is a return or checkout
	bool returnCarSuccessful;				//returns true if car returned successfully
	bool returnVanSuccessful;				//returns true if van returned successfully
	bool checkOutSuccessful;				//returns true if vehicle checkout was successful
	bool employeesWaiting;					//returns true if employees are waiting
	stackRec *newNode;						//empty new node
	stackRec *tempCarNode;					//temporary node for cars stack
	stackRec *tempVanNode;					//temporary node for vans stack
	queueRec *newQueueNode;					//new node for queue wait lines
	ifstream inFile (argv[1]);				//stream variable associated with input file
	ifstream inTransactionData (argv[2]);	//stream variable associated with input file
	ofstream outFile;						//stream associated with output file

	if (!inFile.is_open() || !inTransactionData.is_open()) //test to see if files exist
	{
		return errorMessage("Could not open one of the files!");
	}
	stack *cars = createEmptyStack(); //create a car stack
	stack *vans = createEmptyStack(); //create a van stack
	queue *line = createEmptyQueue(); //create a line queue
	
	inFile >> vehicle;
	while (inFile)	//enter while loop after reading first piece of data
	{
		inFile >> licensePlate >> number;
		newNode = new (nothrow) stackRec();
		if (newNode == NULL)
		{
			return errorMessage("Could not allocate memory from the heap!");
		}
		if (vehicle == "A")//load file data into newly created node and push on applicable stack
		{
			newNode->vehicleType = vehicle;
			newNode->license = licensePlate;
			newNode->capacity = number;
			push (newNode, cars);
			cout << " Placing " << licensePlate << " into automobile area." << endl << endl;
		}
		else //push on van stack
		{
			newNode->vehicleType = vehicle;
			newNode->license = licensePlate;
			newNode->capacity = number;
			push (newNode, vans);
			cout << " Placing " << licensePlate << " into van area." << endl << endl;
		}
		inFile >> vehicle; //read next piece of data until the end of file
	}
	inFile.close(); //when finished with file be sure to close it
	
	nodeCountCars = cars->count; //placing the amount of cars and vans each into a variable
	nodeCountVans = vans->count;
	
	cout << " Parking lot status:" << endl;//display parking lot status after vehicle data has been read
	cout << "      " << nodeCountCars << " automobiles available" << endl;
	cout << "      " << nodeCountVans << " vans available" << endl;
 	cout << endl << endl;
 	system ("pause"); 
	cout << endl << endl;
	
	inTransactionData >> transaction; //begin reading data from checkout return file
	checkOutSuccessful = false;
	
	while (inTransactionData)
	{
		//Checkout transaction
		if (transaction == "C")
		{
			
			inTransactionData >> personCount;
			cout << " For " << personCount << " employees in a group, vehicles checked out are:" << endl;
		
			while(personCount > 0)
			{
				//People are waiting, do a checkout
				checkOutSuccessful = generalCheckout (cars, vans, line, personCount);
				if (!checkOutSuccessful)
				{
					return 1;
				}
			}
			cout << endl;	
		}
		
		//Return transaction
		else if (transaction == "R")
		{
			inTransactionData >> vehicle;
			
			//Vehicle is a car
			if (vehicle == "A")
			{
				inTransactionData >> licensePlate >> passengerCount;
				
				returnCarSuccessful = returnCar (licensePlate, passengerCount, cars);
			}
			
			//Vehicle is a van
			else
			{
				inTransactionData >> licensePlate >> passengerCount;
				
				returnVanSuccessful = returnVan (licensePlate, passengerCount, vans);
			}	
			
			//If employees are waiting, do a checkout with newly returned vehicle
			employeesWaiting = !isEmptyQueue (line);
			if (employeesWaiting) 
			{
				personCount = line->front->numWaiting;
				cout << "    " << personCount << " employees still waiting for a vehicle." << endl << endl;
				
				checkOutSuccessful = generalCheckout (cars, vans, line, personCount);
				if (!checkOutSuccessful)
				{
					return 1;
				}
			}
		}
		cout << "    " << line->count << " employees waiting for vehicle" << endl << endl;
		inTransactionData >> transaction;	
	}
	inTransactionData.close ();
	
	outFile.open (LOTSTATUS.c_str()); //open the write to file LOTSTATUS
	nodeCountCars = cars->count;
	tempCarNode = pop (cars); //removes top car and stores in temporary node
	while (tempCarNode != NULL)
	{
		//assigns data from tempCarNode into variables and then is written to the file
		tempVehicleType = tempCarNode->vehicleType;
		tempLicense = tempCarNode->license;
		tempCapacity = tempCarNode->capacity;
		outFile << tempVehicleType << " " << tempLicense << " " << tempCapacity << endl; 
		delete tempCarNode;
		tempCarNode = pop (cars);
	}
	nodeCountVans = vans->count;
	tempVanNode = pop (vans);
	while (tempVanNode != NULL)
	{
		tempVehicleType = tempVanNode->vehicleType;
		tempLicense = tempVanNode->license;
		tempCapacity = tempVanNode->capacity;
		outFile << tempVehicleType << " " << tempLicense << " " << tempCapacity << endl;
		delete tempVanNode;
		tempVanNode = pop (vans);
	}
	outFile.close ();
	//final display of parking lot with van, car and customer count
	cout << " Parking lot status:" << endl;
	cout << "      " << nodeCountCars << " automobiles available" << endl;
	cout << "      " << nodeCountVans << " vans available" << endl;
	cout << "      " << line->count << " employees waiting for vehicle" << endl << endl;
 	cout << endl << endl;
 	return 0;
}
//function definitions
//------------------------------------------------------------------------------
// FUNCTION: createEmptyStack
// DESCRIPTION: create a stack and intialize the elements and return it to main
// OUTPUT: 
//		Returns: a newly intitalized stack 
//------------------------------------------------------------------------------
stack* createEmptyStack ()
{
	stack* newStack = new (nothrow) stack();
	newStack->count = 0;
	newStack->top = NULL;

	return newStack;
}

//------------------------------------------------------------------------------
// FUNCTION: isEmptyStack
// DESCRIPTION: checks to see if the stack of cars or vans is empty
// OUTPUT:
//		Returns: true or false
//------------------------------------------------------------------------------
bool isEmptyStack (stack* s)
{	
	return (s->top == NULL);	
}

//------------------------------------------------------------------------------
// FUNCTION: isFullStack
// DESCRIPTION: checks to see if the stack of cars or vans is full
// OUTPUT:
//		Returns: true or false
//------------------------------------------------------------------------------
bool isFullStack ()
{
	bool fullStack = false;						  //tells program if stack is full
	stackRec *newNode = new (nothrow) stackRec(); //a new empty node
	
	if (newNode == NULL)
	{
		fullStack =  true;
	}
	
	return fullStack;
}

//------------------------------------------------------------------------------
// FUNCTION: getStackCount
// DESCRIPTION: gets the stack count and returns it to main
// INPUT: 
//		Parameters: nodeCount - holds the count of cars in the stack
// OUTPUT:
//		Returns: stackCount - amount of vehicles in each stack
//------------------------------------------------------------------------------
int getStackCount (int nodeCount)
{
	int stackCount = nodeCount;
	
	return stackCount;
}

//------------------------------------------------------------------------------
// FUNCTION: push
// DESCRIPTION: adds a new vehicle node to the top of the stack
// INPUT:
//		Parameters: newNode - node that contains new vehicle info
//					s - stack that applies to that particluar node
//------------------------------------------------------------------------------
void push (stackRec *newNode, stack *s)
{
	newNode->nextVehicle = s->top;
	s->top = newNode;
	s->count++;
}

//------------------------------------------------------------------------------
// FUNCTION: pop
// DESCRIPTION: removes a vehicle from the stack
// INPUT:
//		Paramters: s - stack that applies to that particular node
//------------------------------------------------------------------------------
stackRec* pop (stack *s)
{
	if(!isEmptyStack(s))
	{
		stackRec* oldTop;
		oldTop = s->top;
		s->top = oldTop->nextVehicle;
		s->count--;
		return oldTop;
	}
}

//------------------------------------------------------------------------------
// FUNCTION: createEmptyQueue
// DESCRIPTION: creates a new empty queue with pointers and a queue count initialized
// OUTPUT:
//		Returns: newQueue - a new intitalized queue
//------------------------------------------------------------------------------
queue* createEmptyQueue ()
{
	queue *newQueue = new (nothrow) queue();
	newQueue->count = 0;
	newQueue->front = NULL;
	newQueue->back = NULL;
	
	return newQueue;
}

//------------------------------------------------------------------------------
// FUNCTION: isEmptyQueue
// DESCRIPTION: determines if the queue is empty
// OUTPUT:
//		Returns: true or false
//------------------------------------------------------------------------------
bool isEmptyQueue (queue *q)
{
	return (q->front == NULL);
}

//------------------------------------------------------------------------------
// FUNCTION: isFullQueue
// DESCRIPTION: determines if the queue is full
// OUTPUT:
//		Returns: true or false
//------------------------------------------------------------------------------
bool isFullQueue ()
{
	bool fullQueue = false;						  //tells program if stack is full
	queueRec *newNode = new (nothrow) queueRec(); //new queueRec node
	
	if (newNode == NULL)
	{
		fullQueue = true;
	}
	return fullQueue;
}

//------------------------------------------------------------------------------
// FUNCTION: enQueue
// DESCRIPTION: adds a node to queue
// INPUT:
//		Parameters: newNode - a node that contains new data
//					q - the queue that applies to this list
//------------------------------------------------------------------------------
void enQueue (queueRec *newNode, queue *q)
{
	if (!isEmptyQueue (q))
	{
		q->back->nextNode = newNode;
	}
	q->back = newNode;
	q->count += newNode->numWaiting;
}

//------------------------------------------------------------------------------
// FUNCTION: deQueue
// DESCRIPTION: removes a node from the queue
// INPUT:
//		Parameters: q - the queue that applies to this list
//------------------------------------------------------------------------------
void deQueue (queue *q)
{
	if (!isEmptyQueue (q))
	{
		queueRec *newFirst = q->front->nextNode;
		q->count -= q->front->numWaiting;
		delete q->front;
		q->front = newFirst;
	}
}

//------------------------------------------------------------------------------
// FUNCTION: errorMessage
// DESCRIPTION: displays an error message
// INPUT:
//		Parameter: error - user defined error message
// OUTPUT:
//		Returns: 1 when error occurs
//------------------------------------------------------------------------------
int errorMessage (string error)
{
	cout << " Error!! " << error;
	return 1;
}

//------------------------------------------------------------------------------
// FUNCTION: checkoutCar
// DESCRIPTION: calls pop function to remove node and returns info to this function
//				then displays that info for the user and updates the personCount
// INPUT:
//		Parameters: cars - pointer to the stack of cars
//					personCount - amount of people wating on cars in that node
//------------------------------------------------------------------------------
void checkoutCar (stack *cars, int& personCount)
{
	stackRec *tempCar; //holds the info for the car that was removed
	
	tempCar = pop (cars);
	cout << "    Automobile " << tempCar->license << " holds " << tempCar->capacity 
		 << " passengers." << endl;
	personCount = personCount - tempCar->capacity;
	delete tempCar;
}

//------------------------------------------------------------------------------
// FUNCTION: checkoutVan
// DESCRIPTION: calls pop function to remove node and returns info to this function
//				then displays that info for the user and updates the personCount
// INPUT:
//		Parameters: vans - pointer to the stack of cars
//					personCount - amount of people wating on cars in that node
//------------------------------------------------------------------------------
void checkoutVan (stack *vans, int& personCount)
{
	stackRec *tempVan; //holds the info for the car that was removed
	
	tempVan = pop (vans);
	cout << "    Van " << tempVan->license << " holds " << tempVan->capacity 
		 << " passengers." << endl;
	personCount = personCount - tempVan->capacity;
	delete tempVan;
}

//------------------------------------------------------------------------------
// FUNCTION: returnCar
// DESCRIPTION: takes the vehicle read in from the file ands returns it the stack
// INPUT:
//		Parameters: licensePlate - holds license plate read from file in main
//					passengerCount - the amount of people that vehicle can hold
//					cars - the stack the vehicle will be placed in
// OUTPUT:
//		Returns: true if return successful and false if not
//------------------------------------------------------------------------------
bool returnCar (string licensePlate, int passengerCount, stack *cars)
{
	bool returnCar = true;						//memory full is switched to false
	stackRec *tempCar = new (nothrow) stackRec; //a new empty node for car stack
	
	if (tempCar == NULL)
	{
		cout << " Heap error - could not allocate memory." << endl;
		returnCar = false;
	}
	tempCar->vehicleType = "A";
	tempCar->license = licensePlate;
	tempCar->capacity = passengerCount;
	//push the car on the stack
	push (tempCar, cars);
	cout << " Returning " << tempCar->license << " to automobile area." << endl;
	
	return returnCar;
}

//------------------------------------------------------------------------------
// FUNCTION: returnVan
// DESCRIPTION: takes the vehicle read in from the file ands returns it the stack
// INPUT:
//		Parameters: licensePlate - holds license plate read from file in main
//					passengerCount - the amount of people that vehicle can hold
//					vans - the stack the vehicle will be placed in
// OUTPUT:
//		Returns: true if return successful and false if not
//------------------------------------------------------------------------------
bool returnVan (string licensePlate, int passengerCount, stack *vans)
{
	bool returnVan = true;						//memory full if switched to false
	stackRec *tempVan = new (nothrow) stackRec; //allocate new node if room in stack
	
	if (tempVan == NULL)
	{
		cout << " Heap error - could not allocate memory." << endl;
		returnVan = false;
	}
	tempVan->vehicleType = "V";
	tempVan->license = licensePlate;
	tempVan->capacity = passengerCount;
	//push the car on the stack
	push (tempVan, vans);
	cout << " Returning " << tempVan->license << " to van area." << endl;
	return returnVan;
}

//------------------------------------------------------------------------------
// FUNCTION: generalCheckout
// DESCRIPTION: determines if there are cars and vans available to checkout and
//				calls the appropriate functions to checkout, remove vehicles and
//				update counts
// INPUT:
//		Parameters: cars - pointer to the car stack
//					vans - pointer to the van stack
//					line - pointer to the line of passengers waiting for a vehicle
//					personCount - amount of people waiting in the queue for a vehicle
// OUTPUT:
//		Returns: true if checkout is successful
//------------------------------------------------------------------------------
bool generalCheckout (stack *cars, stack *vans, queue *line, int& personCount)
{
	bool generalCheckout = true; //tells user memory is full if switched to false
	queueRec *newQueueNode; 	 //allocates space for a new queue node

	if (isEmptyStack (cars) && isEmptyStack (vans))
	{	
		cout << "    No vehicle has been assigned to this group." << endl;
		newQueueNode = new (nothrow) queueRec();
		if (newQueueNode == NULL)
		{
		cout << " Heap error - could not allocate memory." << endl;
		generalCheckout = false;
		}
		newQueueNode->numWaiting = personCount;
		personCount = 0;
		enQueue (newQueueNode, line);
	}
	else if ((!isEmptyStack (cars) && personCount <= 4)
	 	  || (!isEmptyStack (cars) && isEmptyStack (vans)))
	{
		checkoutCar (cars, personCount);
	}
	else
	{
		checkoutVan (vans, personCount);
	}
	return generalCheckout;	
}

