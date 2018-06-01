#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> // getpagesize(), ftruncate()
#include <sys/types.h> // mmap()
#include <sys/mman.h> // mmap()
#include <string.h> // memcpy()
#include <math.h> // pow()

struct TestData {
	size_t a;
	size_t b;
};

void ppData(struct TestData * testData)
{
	printf("TestData {\n");
	fprintf(stdout, "a: %zu, b: %zu\n", testData->a, testData->b);
	printf("}\n");
}

struct TestData * writeTestData(size_t a, size_t b)
{
	fprintf(stdout, "========== pagesize: %d\n", getpagesize());
	printf("========== writing\n");
	FILE * fp;
	if ((fp = fopen("./mmaptestfile", "w+")) == 0) {
		perror("open");
		exit(1);
	}
	int fd = fileno(fp);
	if ((ftruncate(fd, sizeof (struct TestData))) == -1) {
		perror("ftruncate");
		close(fd);
		exit(1);
	}

	char * data;

	data = mmap(0, sizeof (struct TestData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	fprintf(stdout, "mmap addr: %p\n", data);
	if (data == (void *)-1) {
		perror("mmap");
		close(fd);
		exit(1);
	}

	struct TestData testStruct = {a,b};
	memcpy(data, &testStruct, sizeof (struct TestData));

	msync((void *) data, sizeof (struct TestData), MS_SYNC);
	printf("========== initial values\n");
	ppData ((struct TestData *) data);
	fclose(fp);

	return (struct TestData *) data;
}

struct TestData * readAndOverwrite(size_t newA, size_t newB, const char * fopen_mode)
{
	FILE * fp;
	if ((fp = fopen("./mmaptestfile", fopen_mode)) == 0) {
		perror("open");
		exit(1);
	}
	int fd = fileno(fp);
	if ((ftruncate(fd, sizeof (struct TestData))) == -1) {
		perror("ftruncate");
		close(fd);
		exit(1);
	}

	struct TestData * testData;
	
	testData = mmap(0, sizeof (struct TestData), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	fclose(fp);
	if (testData == (void *)-1) {
		perror("mmap");
		close(fd);
		exit(1);
	}
	fprintf(stdout, "mmap addr: %p\n", testData);
	testData->a = newA;
	testData->b = newB;
	printf("========== local change\n");
	ppData (testData);

	return testData;
}

int main(void)
{
	printf("========== start\n");

	struct TestData * originalMmap = writeTestData(33, 69);

	struct TestData * secondMmap = readAndOverwrite(1, 2, "r+"); // simulate read
	printf("========== read second mapping\n");
	ppData (secondMmap);
	struct TestData * thirdMmap = readAndOverwrite(3, 4, "w+"); // simulate truncate
	printf("========== read third mapping\n");
	ppData (thirdMmap);

	printf("========== re-read original mapping\n");
	ppData (originalMmap);
	printf("========== re-read second mapping\n");
	ppData (secondMmap);
	printf("========== re-read third mapping\n");
	ppData (thirdMmap);

	printf("========== end\n");
	exit(EXIT_SUCCESS);
}
