#include <iostream>
#include "gtest/gtest.h"

#include "test.h"

/* *
* Test class for Chromosome
*/
class EMTest : public ::testing::Test {
protected:
	EMTest() {

	}

	virtual ~EMTest() {

	}

	virtual void SetUp() {

	}

	virtual void TearDown() {

	}
	
	//Chromosome c;
};

TEST_F(EMTest, superMegaTestet) {
	int right = 0;
	std::cout << "Running test function and shit." << std::endl;
	EXPECT_EQ(0,right);
}

TEST_F(EMTest, ParticleConnectionMCS) {
    testMCS();
}