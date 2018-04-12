# include <iostream>
# include <stdlib.h>
# include <limits>

using namespace std;

// struct definitions

enum COURSE {CHE102, MATH115, MATH117, ECE105, ECE150, ECE190, Null};

struct Assignment
{
    COURSE course; // CHE102, MATH117, etc.
    int assnNum; // Assignment Number
    int dueMonth; // Month due (1-12)
    int dueDay; // Day due (1-31)
    char* description; // Assignment description
};

struct HomeworkQueue
{
    Assignment* assn;
    HomeworkQueue* nextInQueue;
};

// Function declarations; do not remove

bool enqueue(HomeworkQueue*& queue, const Assignment& assignment);
const Assignment* dequeue(HomeworkQueue*& queue);
int daysTillDue(const HomeworkQueue* q, const COURSE course);
const Assignment* dueIn(const HomeworkQueue* q, const int numDays);

//Helper function(s)
bool isEarlier(const Assignment&, const Assignment&);

//Enqueue notes

//A Homework Queue pointer has been passed in by reference... therefore, I'll point that to the head of the list
//This function will simply be responsible for putting the linked list together... will need another function to sort
//Need to perform a check... ie. (if HomeworkQueue pointer == NULL...
//create a new head pointer, point it to the assignment, and point HomeworkQueue to head)

bool enqueue(HomeworkQueue*& queue, const Assignment& assignment)
{
    bool enqueueSuccess = false;
    //create a new instance of the assignment pointer; copy the values from the passed in assignment to it
    //cannot simply be of the struct type because it'll go out of scope at the end of the function if done so
    Assignment* myAssgn = new Assignment;
    myAssgn->assnNum = assignment.assnNum;
    myAssgn->course = assignment.course;
    myAssgn->dueMonth = assignment.dueMonth;
    myAssgn->dueDay = assignment.dueDay;
    myAssgn->description = assignment.description;

    //need to check whether or not queue has a single element in it or not
    if (queue->assn == NULL) {
        //create a head-pointer, and have it's assignment be myAssign;
        HomeworkQueue* headPtr = new HomeworkQueue;
        headPtr->assn = myAssgn;
        //set the next to NULL... because it's the first element
        headPtr->nextInQueue = NULL;

        //queue must point to the head of the list; so set it equal to the head we created
        queue = headPtr;
        //function is done; return true because enqueuing was done successfully
        //free up the memory allocated from the heap
        delete headPtr;
        enqueueSuccess = true;
    }

        //if there is already a linked list... lets add the item to the very back
    else {
        //need to create a pointer to hold myAssign in either case... refactor this outside the if else logic
        HomeworkQueue* nextPtr = new HomeworkQueue;
        nextPtr->assn = myAssgn;
        nextPtr->nextInQueue = NULL;

        //note that queue is always at the head of the linked list; thus, let the lastElem pointer transverse the list from the beginning
        //this is because we don't really know when the linked list is going to end
        HomeworkQueue* lastElem = new HomeworkQueue;
        lastElem = queue;

        //need to put in a loop here for the nextPtr to chase the headPointer until the next is null...
        //re-point the lastElem pointer to the new end of the list
        bool found = false;
        while (!found) {
            lastElem = lastElem->nextInQueue;

            if (lastElem->nextInQueue == NULL) {
                found = true;
            }
        }

        //add the new element to the end of the queue; lastElem is on the current last element
        lastElem->nextInQueue = nextPtr;

        //lastElem now goes to the new last element
        //next two lines of code are essentially useless... I just need the head of the list.
        lastElem = lastElem->nextInQueue;
        nextPtr = nextPtr->nextInQueue;


        //clean up the memory here... because queue is the only pointer required by the end
        delete lastElem;
        delete nextPtr;

        enqueueSuccess = true;
    }

    /*
     * checking that enqueuing was done correctly
    cout << queue->assn->course << endl;
    cout << queue->assn->dueDay << endl;
    cout << queue->assn->dueMonth << endl;
    cout << queue->assn->assnNum << endl;
    cout << queue->assn->description << endl;
     */


    if (!enqueueSuccess)
        return false;

    else
        return true;
}


//this function is responsible for dequeuing the first assignment from the linked list
//the HomeworkQueue pointer is passed in by reference... essentially I'll need to change where the head of the list is located
//the deleted assignment needs to be returned via an assignment pointer
const Assignment* dequeue(HomeworkQueue*& queue)
{
    //boolean for sequential logic execution
    bool assignFound;

    //if the queue has nothing in it
    if (queue == NULL) {
        assignFound = false;


        //from main method.. should be returning NULL (for the bool if else)
        return NULL;
    }

    //if there is something to dequeue
    else {
        Assignment* assignReturned = new Assignment;

        //need the copy the header's (ie. first element in the queue into this pointer)
        assignReturned->course = queue->assn->course;
        assignReturned->description = queue->assn->description;
        assignReturned->dueDay = queue->assn->dueDay;
        assignReturned->dueMonth = queue->assn->dueMonth;
        assignReturned->assnNum = queue->assn->assnNum;

        //need a temporary new head pointer; point it at queue
        HomeworkQueue* newHead = new HomeworkQueue;
        newHead = queue;

        //let it point to the next element in the queue
        newHead = newHead->nextInQueue;
        //delete the assignment aka the first task
        //delete queue->assn;

        queue = newHead;
        delete newHead;
        return assignReturned;
    }
}

int daysTillDue(const HomeworkQueue* q, const COURSE course)
{
    bool found = false;

    const HomeworkQueue* traverse = new HomeworkQueue;
    traverse = q;

    //return value
    int daysLeft;

    //data for the assignment the user wants to locate
    int assgnDueDay;
    int assgnDueMonth;
    int assgnTotalDueTime;

    //essentially while there is no next assignment to parse
    while (traverse != NULL) {
        if (traverse->assn->course == course) {
            assgnDueDay = traverse->assn->dueDay;
            assgnDueMonth = traverse->assn->dueMonth;
            found = true;
        }

        traverse = traverse->nextInQueue;
    }

    //convert the time found to number of days... operating in the assumption that a month contains ~30 days (could replace easily with a struct here)
    assgnTotalDueTime = assgnDueMonth*30 + assgnDueDay;

    //now that I've found the time of the assignment... I need the current time (in days of the year)
    time_t currentTime;
    struct tm *myTime = localtime(&currentTime);
    int myTotalTime = myTime->tm_yday;


    //need to calculate the days left to when the assignment is due
    //logic needs to take into account if the due date goes into the next year
    //general logic will be assignDuedate - myCurrentDate;
    if ((assgnTotalDueTime - myTotalTime) < 0) {
        daysLeft = assgnTotalDueTime - myTotalTime + 365;
    }

    else {
        daysLeft = assgnTotalDueTime - myTotalTime;
    }


    //returning the number of days left
    if (found) {
        return daysLeft;
    }

    else {
        return INT_MAX;
    }
}


const Assignment* dueIn(const HomeworkQueue* q, const int numDays)
{
    return NULL;
}

bool isEarlier(const Assignment& a1, const Assignment& a2) {
    //error checking goes here (ie. is date within the valid 31 days... and is the month value between 1 and 12)
    if (a1.dueMonth > 12 || a1.dueMonth <= 0 || a2.dueMonth > 12 || a2.dueMonth <= 0) {
        cerr << "assignment month value is invalid" << endl;
        return false;
    }

    if (a1.dueDay <= 0 || a1.dueDay > 31 || a2.dueDay <= 0 || a2.dueDay > 31) {
        cerr << "assignment date value is invalid" << endl;
        return false;
    }


    //proper comparison logic implemented
    if (a1.dueMonth < a2.dueMonth) {
        return true;
    }

    else if (a1.dueMonth > a2.dueMonth) {
        return false;
    }

    else if (a1.dueMonth == a2.dueMonth) {
        if (a1.dueDay < a2.dueDay)
            return true;

        else if (a1.dueDay > a2.dueDay)
            return false;

        else if (a1.dueDay == a2.dueDay) {
            if (a1.course == ECE105)
                return true;

            else
                return false;
        }
    }

    //essentially only triggered if nothing else above is triggered... equivalent to else {return false}
    return false;

}

int main(const int argc, const char* const argv[]) {
    // Some test driver code here ....

    std::cout << std::endl << "Implement some more appropriate tests in main()" << std::endl << std::endl;

    HomeworkQueue* p_queue = new HomeworkQueue;
    p_queue->nextInQueue = NULL;
    p_queue->assn = NULL;

    HomeworkQueue* p_copy = new HomeworkQueue;

    char sampleDescription[] = "Sequential Execution";
    Assignment assn1 =
            {
                    .course = ECE150,
                    .assnNum = 0,
                    .dueMonth = 9,
                    .dueDay = 15,
                    .description = sampleDescription
            };

    bool enqueueSuccess = enqueue(p_queue, assn1);
    p_copy = p_queue;

    if(enqueueSuccess)
    {
        std::cout << "assn1 enqueued successfully" << std::endl << std::endl;

    }

    else
    {
        std::cout << "enqueue() failed" << std::endl << std::endl;
    }

    //finding how many days are left implemented
    int daysLeft = daysTillDue(p_copy, ECE150);

    if (daysLeft == INT_MAX) {
        cout << "could not find assignment" << std::endl;
    }

    else {
        cout << "days remaining: " << daysLeft << " days" << std::endl<< std::endl;
    }


    //dequeue function is invoked here... after that, the queue essentially becomes null
    const Assignment* p_firstAssignmentInQueue = dequeue(p_queue);


    if (p_firstAssignmentInQueue)
    {
        std::cout << "Dequeue successful..." << std::endl;
        std::cout << "Course: " << p_firstAssignmentInQueue->course << std::endl;
        std::cout << "Assignment #: " << p_firstAssignmentInQueue->assnNum << std::endl;
        std::cout << "Day due: " << p_firstAssignmentInQueue->dueDay << std::endl;
        std::cout << "Month due: " << p_firstAssignmentInQueue->dueMonth << std::endl;
        std::cout << "Description: " << p_firstAssignmentInQueue->description << std::endl;
    }

    else
    {
        std::cout << "dequeue() failed" << std::endl;
    }

    delete p_queue;

    return 0;
}