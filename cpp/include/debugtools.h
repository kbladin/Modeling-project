#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H value

#include <iostream>
#include <glm/glm.hpp>



static std::ostream& operator<<(std::ostream& os, const glm::vec3& vec){
    return os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

static std::ostream& operator<<(std::ostream& os, const glm::vec4& vec){
    return os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " << vec[3] << ")";
}


#endif