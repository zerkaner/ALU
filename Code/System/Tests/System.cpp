#include <stdio.h>
#include "..\Execution\Executor.h"
#include "..\..\Data\Vector.h"

int main (int argc, char** argv) {
  printf("Test System!\n");

  Executor* exec = new Executor();
  exec->RandomExecution = false;

  Vector v1 = Vector (2.0f, 3.0f);
  Vector v2 = Vector (4.0f, 4.0f);

  printf ("%s\n%s\n", v1.ToString(), v2.ToString());

  
  float dp = Vector::DotProduct(v1, v2);
  printf("\nDP is: %5.2f\n", dp);

  Vector v3 = 5*v1;
  printf("\n%s\n", (v3*5).ToString());

  //new Engine("Testfenster", 640, 480, false);

  getchar();
  return 0;
}