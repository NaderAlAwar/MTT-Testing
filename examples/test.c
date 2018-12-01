
int main()
{
	int x;
	int y = 5;
	y = test(x,y);
}

int test(int x, double y)
{
	x = y + 3 + 6;
	int testA = 1;
	int testB = 2;
	int testF = 2 + testA;
	if(testA < testB)
	{
		testA += 1;
	}
	else if(testA > testB)
	{
		testB = 3;
	}
	else
	{
		testA = testB;
	}
	int testD = 0;

	if(testD)
	{

	}
	int testArray[] = {testA, 1, testB};
	for(int i = 0; i < 4; i++)
	{
		testArray[i] = testA + 1;
	}
	int testC = testA + testB;
	return x+y;
}
