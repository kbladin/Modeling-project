#include "test.h"


//This test function should implement some
//nice test cases when MSC-class is done 
void testMCS(){
    std::cout << "Testing MCS" << std::endl;
    MCS mcs(1,2,2);
    assert(mcs.getNumberOfParticles() == 1*2*2);
    // uncomment this when class is implemented
    //assert(6 == mcs.numberOfConnections()); 
}
