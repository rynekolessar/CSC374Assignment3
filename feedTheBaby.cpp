/*-------------------------------------------------------------------------*
 *---									                                ---*
 *---		                    feedTheBaby.cpp		              		---*
 *---									                                ---*
 *---	    This program simulates a mother having to periodically  	---*
 *---	feed, and clean up after, a baby.  The food and the baby are	---*
 *---	all implemented with threads.					                ---*
 *---								                                	---*
 *---	----	----	----	----	----	----	----	----	---*
 *---								                                	---*
 *---	    Version 1.0		2019 October 17		Joseph Phillips	        ---*
 *---									                                ---*
 *-------------------------------------------------------------------------*/

//		Compile with:
//			g++ feedTheBaby.cpp -lpthread -o feedTheBaby

//---		Header inclusion and namespace specification:		---//

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <set>

using namespace std;

//---		Definition of constants:				---//

//  PURPOSE:  To tell the length of char arrays.
const int MAX_LINE = 256;

//  PURPOSE:  To tell the minimum number of meals that must be somewhere in
//	the gut of the baby before it reports that it is hungry.
const int MIN_NUM_MEALS_BEFORE_REPORT_HUNGRY = 2;

//  PURPOSE:  To tell the tell the minimum load that the diaper can hold
//	before it reports that the diaper must be changed.
const int DIAPER_LOAD_BEFORE_GUARANTEE_COMPLAINT = 4;

//  PURPOSE:  To tell the names of the foods that the baby can eat.
const char *FOOD_ARRAY[] =
    {"applesauce",
     "yogurt",
     "milk",
     "porridge",
     "carrots",
     "peas",
     "Chipotle burrito"};

//  PURPOSE:  To tell what the baby will report when it is hungry.
const char *HUNGRY_MSG_ARRAY[] =
    {"Hear that gut growling?  It's EMPTY!",
     "How do you expect me to gain weight?",
     "Lunch time!",
     "My tummy says it's bored!"};

//  PURPOSE:  To tell what the baby will report when its diaper needs to be
//	changed.
const char *DIRTY_DIAPER_MSG_ARRAY[] =
    {"Yuck! Can't you SMELL that?",
     "It was warm when it came out, but now it's nasty!",
     "Eeww!  I have a soggy bottom!",
     "I have an itchy bottom!"};

//  PURPOSE:  To tell what the mama will say when she changes the diaper.
const char *REPLACE_DIAPER_MSG_ARRAY[] =
    {"Hold your nose during, wash your hands after!",
     "Dirty work, but someone's gotta do it!",
     "A mother's work is never done!",
     "Where's Daddy at a time like this?!"};

//---		Definition of classes:				---//

//  PURPOSE:  To implement a digestive organ of the baby.
class DigestiveOrgan
{
    //  I.  Member vars:
    //  PURPOSE:  To hold the name of '*this' organ.
    std::string name_;
    pthread_mutex_t organLock_;
    pthread_cond_t notFull_;  

    //  PURPOSE:  To tell the maximum number of meals that '*this' organ can hold.
    int maxCapacity_;

    //  PURPOSE:  To tell the current number of meals in '*this' organ.
    int currentNumMeals_;

    //  II.  Disallowed methods:
    //  No default constructor:
    DigestiveOrgan();

    //  No copy constructor:
    DigestiveOrgan(const DigestiveOrgan &);

    //  No copy assignment op:
    DigestiveOrgan &
    operator=(const DigestiveOrgan &);

protected:
    //  III.  Protected methods:

public:
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' DigestiveOrgan to have name 'newName'
    //	and capacity for 'newCapacity' meals.  No return value.
    DigestiveOrgan(const std::string &newName,
                   int newCapacity) : name_(newName),
                                      maxCapacity_(newCapacity),
                                      currentNumMeals_(0)
    {
        pthread_mutex_init(&organLock_, NULL);
        pthread_cond_init(&notFull_, NULL); 
    }

    //  PURPOSE:  To release the resources of '*this'.  No parameters.  No return
    //	value.
    ~DigestiveOrgan()
    {
        pthread_mutex_destroy(&organLock_);
        pthread_cond_destroy(&notFull_);
    }

    //  V.  Accessors:
    //  PURPOSE:  To return the name of '*this' organ.  No parameters.
    const std::string &
    getName()
        const
    {
        return (name_);
    }

    //  PURPOSE:  To return the maximum number of meals that '*this' organ can
    //	hold.  No parameters.
    int getMaxCapacity()
        const
    {
        return (maxCapacity_);
    }

    //  PURPOSE:  To return the current number of meals in '*this' organ.  No
    //	parameters.
    int getCurrentNumMeals()
        const
    {
        return (currentNumMeals_);
    }

    //  PURPOSE:  To return 'true' if '*this' is full.  No parameters.
    bool isFull()
        const
    {
        return (getCurrentNumMeals() >=
                getMaxCapacity());
    }

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc work of class:
    //  PURPOSE:  To make the food named 'mealCPtr' enter '*this' DigestiveOrgan.
    //	No return value.
    void enter(const char *mealCPtr)
    {
        pthread_mutex_lock(&organLock_);
        while (getCurrentNumMeals() >= getMaxCapacity())
        {
            printf("%s is being held up in the %s\n", mealCPtr, getName().c_str());
            pthread_cond_wait(&notFull_, &organLock_);
        }

        currentNumMeals_++;

        if (getCurrentNumMeals() > getMaxCapacity())
        {
            printf("The baby's %s was overstuffed and ruptured!\n", getName().c_str());
            printf("The baby is dead!\n");
            exit(EXIT_FAILURE);
        }

        printf("The %s is entering the %s\n", mealCPtr, getName().c_str());
        pthread_mutex_unlock(&organLock_);    
    }

    //  PURPOSE:  To make the food named 'mealCPtr' leave '*this' DigestiveOrgan.
    //	No return value.
    void leave(const char *mealCPtr)
    {
        pthread_mutex_lock(&organLock_);
        currentNumMeals_--;

        if (getCurrentNumMeals() < 0)
        {
            printf("The The baby's %s was sucked dry and imploded!\n",
                   getName().c_str());
            printf("The baby is dead!\n");
            exit(EXIT_FAILURE);
        }

        printf("The %s is leaving the %s\n", mealCPtr, getName().c_str());
        pthread_mutex_unlock(&organLock_);
        pthread_cond_signal(&notFull_);
    }
};

//  PURPOSE:  To implement the diaper on the baby.
class Diaper
{
    //  I.  Member vars:
    //  PURPOSE:  To tell the total number of meals that have been "deposited"
    //	into '*this' Diaper.
    int loadCount_;
    pthread_mutex_t 	 	diaperLock_;

    //  II.  Disallowed auto-generated methods:
    //  No copy constructor:
    Diaper(const Diaper &);

    //  No copy assignment op:
    Diaper &operator=(const Diaper &);

protected:
    //  III.  Protected methods:

public:
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' Diaper to be empty (clean).
    Diaper() : loadCount_(0)
    {
        pthread_mutex_init(&diaperLock_, NULL);
    }

    //  PURPOSE:  To release the resources of '*this'.  No parameters.  No return
    //	value.
    ~Diaper()
    {
        pthread_mutex_destroy(&diaperLock_);
    }

    //  V.  Accessors:
    //  PURPOSE:  To return the current number of meal deposited into '*this'
    //	Diaper.  No parameters.
    int getLoadCount()
        const
    {
        return (loadCount_);
    }

    //  VI.  Mutators:
    //  PURPOSE:  To deposit the meal named 'mealNameCPtr' into '*this' Diaper.
    //	No return value.
    void makeDeposit(const char *mealNameCPtr)
    {
        pthread_mutex_lock(&diaperLock_);
        printf("(%s entering diaper)\n", mealNameCPtr);
        loadCount_++;
        pthread_mutex_unlock(&diaperLock_);
    }

    //  PURPOSE:  To replace '*this' Diaper with a clean one.  No parameters.
    //	No return value.
    void replace()
    {
        pthread_mutex_lock(&diaperLock_);
        loadCount_ = 0;
        pthread_mutex_unlock(&diaperLock_);
    }

    //  VII.  Methods that do main and misc work of class:
};

//---		Global functions and objects:				---//

//  PURPOSE:  To represent the stomach of the baby.
DigestiveOrgan stomach("stomach", 2);

//  PURPOSE:  To represent the small intestine of the baby.
DigestiveOrgan smallIntestine("small intestine", 3);

//  PURPOSE:  To represent the large intestine of the baby.
DigestiveOrgan largeIntestine("large intestine", 1);

//  PURPOSE:  To represent the diaper on the baby.
Diaper diaper;

//  PURPOSE:  To hold 'true' while this program should run, or 'false'
//	otherwise.
bool shouldRun = true;

//  PURPOSE:  To randomly choose and return a 'const char*' member from array
//	'array[]' of size 'arraySize'
const char *obtainRandomMessage(size_t arraySize, const char *array[])
{
    return (array[rand() % (arraySize / sizeof(const char *))]);
}

//  PURPOSE:  To randomly select and return the name of a food.  No parameters.
const char *selectFood()
{
    return (obtainRandomMessage(sizeof(FOOD_ARRAY), FOOD_ARRAY));
}

//  PURPOSE:  To change the diaper, but complain about it.  No parameters.
//	No return value.
void replaceDiaper()
{
    const char *msgCPtr = obtainRandomMessage(sizeof(REPLACE_DIAPER_MSG_ARRAY),
                                              REPLACE_DIAPER_MSG_ARRAY);
    printf("Mama: \"%s\"\n", msgCPtr);
    diaper.replace();
}

//  PURPOSE:  To process the meal whose 'const char*' name is pointed to by
//	'vPtr'.  Returns 'NULL'.
void *processMeal(void *vPtr)
{
    //  I.  Application validity check:
    if (vPtr == NULL)
    {
        fprintf(stderr, "NULL ptr to processMeal(), quitting\n");
        exit(EXIT_FAILURE);
    }

    //  II.  Handle attempting to eat the food:
    //  II.A.  Get the name of the food:
    const char *mealNamePtr = (const char *)vPtr;

    //  II.B.  Just give up if the stomach is already full:
    if (stomach.isFull())
    {
        printf("Baby \"There's no space for %s!"
               "  I'm spitting it on to the floor!\"\n",
               mealNamePtr);
        return (NULL);
    }

    //  II.C.  Let the food pass thru the gut:
    printf("Baby \"%s?  I'll give it a try . . .\"\n", mealNamePtr);

    stomach.enter(mealNamePtr);
    sleep(3 + rand() % 3);
    smallIntestine.enter(mealNamePtr);
    stomach.leave(mealNamePtr);
    sleep(3 + rand() % 3);
    largeIntestine.enter(mealNamePtr);
    smallIntestine.leave(mealNamePtr);
    sleep(3 + rand() % 3);
    largeIntestine.leave(mealNamePtr);
    diaper.makeDeposit(mealNamePtr);

    //  III.  Finished:
    return (NULL);
}

//  PURPOSE:  To be the function that reports on the status of the baby.
//	Ignores 'vPtr'.  Returns 'NULL'.
void *beTheBaby(void *vPtr)
{
    //  I.  Application validity check:

    //  II.  Be the baby:
    //  II.A.  Each iteration considers complaining about an empty gut and
    //  	     considers complaining about a soiled diaper.
    while (shouldRun)
    {
        //  II.A.1.  Consider complaining about an empty gut:
        if ((stomach.getCurrentNumMeals() +
             smallIntestine.getCurrentNumMeals() +
             largeIntestine.getCurrentNumMeals()) < MIN_NUM_MEALS_BEFORE_REPORT_HUNGRY)
        {
            const char *msgCPtr = obtainRandomMessage(sizeof(HUNGRY_MSG_ARRAY),
                                                      HUNGRY_MSG_ARRAY);
            printf("Baby: \"%s  WWWAAHH!\"\n", msgCPtr);
        }

        sleep(1 + rand() % 2);

        //  II.A.2.  Consider complaining about a soiled diaper:
        if (diaper.getLoadCount() > (rand() % DIAPER_LOAD_BEFORE_GUARANTEE_COMPLAINT))
        {
            const char *msgCPtr = obtainRandomMessage(sizeof(DIRTY_DIAPER_MSG_ARRAY),
                                                      DIRTY_DIAPER_MSG_ARRAY);
            printf("Baby: \"%s  WWWAAHH!\"\n", msgCPtr);
        }

        sleep(1 + rand() % 2);
    }

    //  III.  Finished:
    return (NULL);
}

int main(int argc, char *argv[])
{
    if (argc > 1)
        srand(strtol(argv[1], NULL, 10));
    else
        srand(getpid());

    //  YOUR CODE HERE
    pthread_t thread;
    pthread_t dtId;
    pthread_attr_t dtAttr;
    char line[MAX_LINE];
    const char *food;
    int choice;
    int shouldRun = 1;

    pthread_create(&thread, NULL, beTheBaby, NULL);
    pthread_attr_init(&dtAttr);
    do
    {
        printf("Choice is currently %d \n", choice);
        do
        {
            printf("What's a mother to do? \n"
                   "\t(1) Feed my baby, \n"
                   "\t(2) Change the diapers, \n"
                   "\t(0) Give the baby up for adoption: \n");
            fgets(line, MAX_LINE, stdin);
            choice = strtol(line, NULL, 10);
            if (choice == 0)
            {
                shouldRun = 0;
                break;
            }
            else if (choice == 1)
            {
                food = selectFood();
                pthread_attr_setdetachstate(&dtAttr, PTHREAD_CREATE_DETACHED);
                pthread_create(&thread, &dtAttr, processMeal, (void *)&food[0]);
            }
            else if (choice == 2)
            {
                replaceDiaper();
            }
            if (!(choice == 0 || choice == 1 || choice == 2))
                printf("That's not a valid option, please enter either 0, 1, or 2 this time: \n");
        } while (choice != 0 && choice != 1 && choice != 2 && shouldRun != 0);

        if (choice == 0)
        {
            shouldRun = 0;
            break;
        }
    } while (shouldRun == 1);

    pthread_join(thread, NULL);
    pthread_attr_destroy(&dtAttr);

    printf("Mama \"Now you're someone ELSE's problem!\"\n");
    fflush(stdout);
    return (EXIT_SUCCESS);
}