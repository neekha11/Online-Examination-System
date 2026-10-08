#ifndef EXAMREPOSITORY_H
#define EXAMREPOSITORY_H

#include "../models/Exam.h"
#include <string>
using namespace std;

class ExamRepository {
public:
    void saveExam(Exam exam, string filename);
};

#endif