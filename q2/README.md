# BACK TO COLLEGE
---
## Overview
The program simulates the process of production, distribution and testing of vaccines on students . The companies produces the vaccines which are then transported to the vaccination zones .Students are allocated a vaccination zone where they are injected vaccines. If they passed the antibody test , then they are sent to college else they have to get vaccinated again . If any student fails the test 3 times , he is sent back home.The simulation is synchronized using mutex locks.



## Implementation

A struct s is created which is passed during the inialization of thread functions.
```C
typedef struct s{
    int id;
} s;
```
Most of the information is stored in the global variables or arrays which can be accessed by any thread during the simulation.
All the variables are self-explanatory or are discussed in the individual function explanation.


#### The code is divided into three sections which deals separately with :
 - Pharmaceutical Companies
 - Vaccination Zones
 - Students

##### Pharmaceutical Companies
 - Each Pharmaceutical Company is treated as a separate thread. 
 - The Company takes a random amount of time to prepare the vaccines .
 - It produces r(also random) batches of vaccines.
 - It checks if any of the zones has ran out of vaccines by iterating over the signal array of the zones (i.e `zones_signal[]`).
 - The mutex named `mutex` ensures that no two companies provide vaccines to the same zone at the same time. 
 - After distributing all the vaccines to the zones , the company threads busy -waits for its vaccines to get exhausted on students in vaccination zones.
 - After the vaccines are exhausted the company again starts manufacturing vaccines.

 ##### Vaccination Zones
 -  Each Vaccination Zone is treated as separate thread 
 -  Initially in the main function  `zones_signal` array is all set to 1 and the zone thread busy waits for the companies to deliever the vaccines.
 -  After the zones receive the vaccines ,it creates k slots. The value of k is set so that it does not exceed the students waiting and the vaccines remaining in the zones.
 -  The mutex named `mutex4` ensures that the same students is not allocated to two different zone at the same time. It also ensures that the value of `students_waiting` do not change while zone is allocating students. 
 -  After allocating k students the zone , the zone enters the vaccination zones and all students are vaccinated and afterwards are tested for antibodies.
 - If the zone ran out of vaccines it changes its signal array and thus informing companies that it needs vaccines.

##### Students 
- Each student is also treated as a separate thread.
- An array named `students_vaccine_cnt` stores the number of times student has came for vaccinations. If the value reaches 3 , student is sent back to home.
- After arrival students signals the vaccination zones through the `student_waiting` array .
- The student busy waits on the `student_waiting` array the zone is allocated .
- Another busy-waiting loop waits untill the the student is vaccinated.
- If the student tests positve for antibodies then he is sent to college else he has to join the queue.

## Assumptions

- The value of n,m,o cannot be equal to zero .
- The values of probabilty are rounded off to 2 decimal places.