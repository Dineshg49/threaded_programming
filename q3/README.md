# Musical Mayhem

---

## Overview
The program simulates the process of arrival , performance of the musician , collection of tshirt . Musicians arrive at the Srujana after 
a specified time and then waits for a valid stage to get free . After performing they(except singers) collect tshirt from the Coordinators of the Music Club .

## Implementation
A struct s is created which is passed through the inialization of thread functions.
```C
typedef struct s{
    int id;
} s;
```
Most of the information is stored in the global variables or arrays which can be accessed by any thread during the simulation.
All the variables are self-explanatory or are discussed in the individual function explanation.

#### The simulation is mainly consists of Four parts:
- >  Arrival of Musicians
-  > Allocating Stage if valid free
-  > Joining Performance in between (for singers)
-  > Collection of Tshirt

#### Arrival of Musicians
- Each Musician is treated as a thread which is initialized with function `performer` .
- The thread waits for the time untill the musician arrive at the Srujana
- After arrival of Musician he changes the signal array `per_status` to 1 which signals the stages the Musican has arrived.
- The Musician waits for `t` seconds to get stage free.
- If any stage gets free then value of `per_status` is changed to 2
- If no stage is allocated then the musician threads returns without performing.

#### Allocating Stage if valid free
- Both type of stages are allocated a semaphore `sem` for Accoustic and `sem2` for Electric.
- The Semaphore `sem` allows `a` performers at a time while the `sem2` allows `e` performers at a time.
- For Musician which can play both Accoustic and Electric two threads are created one for both type of stages.
- When either of the thread enters one semaphore , the other thread is returned as soon as it enters the other semaphore.
- This is achieved by using an array of semaphores named `dope[]` which is shared by both the electric and accoustic stages.

#### Joining Performance in between (for singers)
- Singers are special case . They can either perform solo or with some other musician increasing their time by 2 seconds.
- To implement this functionality , there is similar copy of the code explained above is implemented.
- To expalin the method ,assume that there is a copy of all the stages and on these stages only singers can perform.
- If the copy_stage which is allocated to singer is empty on the paralled real stage , then the singer performs solo.
- Else the singer thread just exits and signals the musician in the real stage that a singer has joined which increases it performance by 2 seconds.

#### Collection of Tshirt
- This is simply implemented by initializing a semaphore named `cord` with value `c` i.e. number of coordinators.
- When a musician leaves the stage , the thread is redirected to the tshirt function where it waits untill one of the coordinators gets free.
- After Selecting/Trying Tshirt for 2 seconds , the Musician Leaves.


## Assumptions
 - There is a valid stage available for every musician. Eg. If someone arrives with a violin , there is atleast 1 accoustic stage in the simulation.
 - The value of the time after which the Musician left the Srujana is not too large .( As the program waits for that atleast that amount of time if finished early).
