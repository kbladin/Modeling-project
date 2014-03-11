#include <iostream>
#include "gtest/gtest.h"
#include <MCS.h>

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
	
};

TEST_F(EMTest, superMegaTestet) {
	int right = 0;
	std::cout << "Running test function and shit." << std::endl;
	EXPECT_EQ(0,right);
}

TEST_F(EMTest, ParticleConnectionMCS) {
    std::cout << "Testing MCS" << std::endl;
    MCS mcs(1,2,2);
    assert(mcs.getNumberOfParticles() == 1*2*2);
}