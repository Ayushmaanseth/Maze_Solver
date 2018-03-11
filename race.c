#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "abdrive.h"
#include "simpletext.h"
#include "simpletools.h"
#include "ping.h"
#include "pathfinding.h"
#include <stdbool.h>  
#include <time.h>


int V = 16;

int irRight = 0;
int irLeft = 0;
int dist = 0;

bool visited[16];
int parent[16],shortestPath[16];

int x = 0;



void makeZero()
{
  for(int k = 0 ; k < 16 ; k++)
  {
  visited[k] = false;
  } 

  for(int i = 0;i<V;i++)
  {
    parent[i] = -1;
    shortestPath[i] = -1;
  }
}



void getLeftDist()
{
  irLeft = 0;

  for(int dacVal = 0; dacVal <= 160; dacVal += 8)  // <- add
    {                                               // <- add
      dac_ctr(26, 0, dacVal);                       // <- add
      freqout(11, 1, 38000);                        // <- add
      irLeft += input(10);
    }

        
}

void getRightDist()
{
  irRight = 0;

  for(int dacVal = 0; dacVal <= 160; dacVal += 8)  // <- add
    {                                               // <- add
      dac_ctr(26, 0, dacVal);                       // <- add
      freqout(1, 1, 38000);                        // <- add
      irRight += input(2);
    }   
}

int getWallDist()
{
  return ping_cm(8);
}

int randomnum()
{
  time_t t;
  srand((unsigned) time(&t));
  int num = rand() % (26 + 1 - 25) + 25;
  return num;
}

void driveDefault()
{
  drive_goto(127,127);
}

void turnLeft()
{
  drive_goto(-26,25);
  drive_goto(-1,0);
}

void turnRight()
{
  drive_goto(26,-25);
  drive_goto(0,-1);
}

  bool checkWall()
{
  int dist = ping_cm(8);
  if (dist < 34)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool checkLeftObstruction()
{
  getLeftDist();

  if(irLeft < 19)
    return true;
  else
    return false;

}

bool checkRightObstruction()
{
  getRightDist();

  if(irRight < 20)
    return true;
  else
    return false;

}

void getInitial()
{
  getLeftDist();
  getRightDist();

  dist = getWallDist();
 
}
/*

bool checkInit(int leftDist,int rightDist, int wallDist)
{
  getLeftDist();
  irLeftT = irLeft;
  getRightDist();
  irRightT = irRight;

  int temp = getWallDist();

  if (leftDist == irLeftT && rightDist == irRightT && temp == dist)
  {
    return true;
  }

  else
    return false;

}
*/
void followLeft()
{
  int y=0,x=0;
  int direction = 0;
  getInitial();
  makeZero();



  int V = 17;
  struct Graph* graph = createGraph(V);

  int cellCounter = 0;


  while(true)
  {
    
    //turnLeft();
    getLeftDist();
    getRightDist();

    
    if (checkLeftObstruction() == false)
    {
      turnLeft();
      direction -= 1;
      driveDefault();
    }
    else
    {
      //turnRight();
      if (checkWall() == false)
      {
        driveDefault();
      }
      else
      {
        turnRight();
        direction +=1;
        if (checkWall() == false)
        {
          driveDefault();
        }
        else
        {
          turnRight();
          direction += 1;
          driveDefault();
        }
      }
    }

    if(checkInit(irLeft,irRight,dist) == true)
      break;
    /*
    if (checkRightObstruction() == true && checkWall() == true)
    {
      break;
    }
    */
    //drive_getTicks(&irLeftTicks,&irRightTicks);
    //printf("%d %d\n",irRightTicks,irRightTicks);

    if(direction < 0)
      direction += 4;

    else if(direction > 3)
    direction -= 4;

  //printf("%d\n",direction );

  if(direction == 0)
  {
    if(visited[cellCounter + 4] == false || (visited[cellCounter + 4] == true && !checkEdge(graph,cellCounter,cellCounter + 4)))
    {
    addEdge(graph,cellCounter,cellCounter + 4,1);
    }
    cellCounter += 4;
    visited[cellCounter] = true;
  }

  else if(direction == 1 )
  {
    if(visited[cellCounter + 1] == false  || (visited[cellCounter + 1] == true && !checkEdge(graph,cellCounter,cellCounter + 1)))
    {
    addEdge(graph,cellCounter,cellCounter + 1,1);
    } 
    cellCounter += 1;
    visited[cellCounter] = true;
  }

  else if(direction == 2)
  {
    if(visited[cellCounter - 4] == false || (visited[cellCounter -4] == true && !checkEdge(graph,cellCounter,cellCounter - 4)) )
    {
    addEdge(graph,cellCounter,cellCounter - 4,1);
    }
    cellCounter -= 4;
    visited[cellCounter] = true;
  }

  else if(direction == 3)
  {
    if(visited[cellCounter - 1] == false  || (visited[cellCounter - 1] == true && !checkEdge(graph,cellCounter,cellCounter - 1)))
    {
      addEdge(graph,cellCounter,cellCounter -1,1);
    }
    cellCounter -= 1;
    visited[cellCounter] = true;
  }

  printf("%d\n",cellCounter);

  if(visited[0] == true)
    break;
  }

/*
  bool b = checkEdge(graph,9,5);
  if(b == true)
    printf("true\n");
  else
    printf("false\n");
  */
  if(direction == 3)
  {
  turnLeft();
  drive_goto(10,10);
  turnRight();
  turnRight();
  }

  else if(direction == 2)
  {
    drive_goto(10,10);
    turnRight();
    turnRight();
  }



  dijkstra(graph,0);

  drive_goto(12,12);

  int cellCount = 0,newDirection = 0;

//----------------------phase2------------------
  
while(!isE(st))
{
  int currentCell = pop(&st);

  if((currentCell - cellCount) == 1)
  {
    if(newDirection == 1)
    {
      driveDefault();
      cellCount += 1;
    }

    else if(newDirection == 2)
    {
      turnLeft();
      driveDefault();
      cellCount += 1;
      newDirection -= 1;
    }

    else if(newDirection == 0)
    {
      turnRight();
      driveDefault();
      cellCount += 1;
      newDirection += 1;
    }

    /*
    else
    {
    turnRight();
    driveDefault();
    turnLeft();
    cellCount += 1;
    }
    */
  }

  else if((currentCell - cellCount) == -1)
  {

    if(newDirection == 0)
    {
      turnLeft();
      driveDefault();
      newDirection -= 1;
      cellCount -= 1;
    }

    else if(newDirection == 2)
    {
      turnRight();
      driveDefault();
      cellCount -= 1;
      newDirection += 1;
    }

    else if(newDirection == 3)
    {
      driveDefault();
      cellCount += -1;
    }
    /*
    else if()
    turnLeft();
    driveDefault();
    turnRight();
    cellCount -= 1;
    */
  }
 //might break here if i try to remove the extra turn and leave it without keeping track of new direction for phase 2

  else if((currentCell - cellCount) == 4)
  {
    if(newDirection == 0)
    {
    driveDefault();
    cellCount += 4;
    }

    else if(newDirection == 1)
    {
      turnLeft();
      driveDefault();
      cellCount += 4;
      newDirection -= 1;
    }

    else if(newDirection == 3)
    {
      turnRight();
      driveDefault();
      cellCount += 4;
      newDirection += 1;
    }
  }

  else if((currentCell - cellCount) == -4)
  {
    if(newDirection == 1)
    {
      turnRight();
      driveDefault();
      cellCount -= 4;
      newDirection += 1;
    }

    else if(newDirection == 2)
    {
      driveDefault();
      cellCount -= 4;
    }

    else if(newDirection == 3)
    {
      turnLeft();
      driveDefault();
      cellCount -= 4;
      newDirection -= 1;
    }
    /*
    turnLeft();
    turnLeft();
    driveDefault();
    turnRight();
    turnRight();
    cellCount -= 4;
    */
  }


  if(newDirection < 0)
    newDirection += 4;

  else if(newDirection > 3)
    newDirection -= 4;


}
  
}


void main()
{
  driveDefault();
  followLeft();

}
