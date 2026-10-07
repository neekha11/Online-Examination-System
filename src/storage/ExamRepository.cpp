#include "../../include/storage/ExamRepository.h"
#include <fstream>

using namespace std;

void ExamRepository::saveExam(Exam exam, string filename) {

    ofstream file(filename, ios::app);

    if (file.is_open()) {
        file << exam.getExamId() << ","
             << exam.getTitle() << ","
             << exam.getDuration() << ","
             << exam.getTotalQuestions() << endl;

        file.close();
    }
}