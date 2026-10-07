#include "../../include/storage/StudentRepository.h"
#include <fstream>

using namespace std;

void StudentRepository::saveStudent(Student student, string filename) {

    ofstream file(filename, ios::app);

    if (file.is_open()) {
        file << student.getId() << endl;
        file.close();
    }
}