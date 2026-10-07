#ifndef STUDENTREPOSITORY_H
#define STUDENTREPOSITORY_H

#include "../models/Student.h"
#include <string>
using namespace std;

class StudentRepository {
public:
    void saveStudent(Student student, string filename);
};

#endif