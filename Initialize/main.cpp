#include <iostream>
#include <fstream>
#include "test.h"
#include "../src/lib/inc/sys.h"
using namespace std;

int main()
{
	test();

	Format("../dev/SecondaryFS.img", 1000, 65536);

	return 0;
}