#ifndef STUDENTREPOSITORY_H
#define STUDENTREPOSITORY_H

#include "../models/Student.h"
#include <memory>
#include <string>
#include <vector>

// File format (one student per line):
//   id,name,username,passwordHash,department,year
class StudentRepository {
public:
    // Returns false if the data is invalid, the id/username already exists,
    // or the file cannot be written.
    bool saveStudent(const Student& student, const std::string& filename) const;

    bool existsById(int id, const std::string& filename) const;
    // Returns the student, or a null pointer if not found.
    std::unique_ptr<Student> findByUsername(const std::string& username,
                                            const std::string& filename) const;
    std::vector<Student> loadAll(const std::string& filename) const;
};

#endif