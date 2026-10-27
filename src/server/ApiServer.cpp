#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include "../../include/third_party/httplib.h"
#include "../../include/server/ApiServer.h"
#include "../../include/models/Admin.h"
#include "../../include/models/AnswerSheet.h"
#include "../../include/services/AuthService.h"
#include "../../include/services/EvaluationService.h"
#include "../../include/services/GradingService.h"
#include "../../include/services/QuestionService.h"
#include "../../include/services/SubmissionService.h"
#include "../../include/storage/AnswerRepository.h"
#include "../../include/storage/ExamRepository.h"
#include "../../include/storage/ResultRepository.h"
#include "../../include/storage/StudentRepository.h"
#include "../../include/utils/StringUtils.h"
#include "../../include/utils/Validator.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <random>
#include <sstream>

using namespace std;
using httplib::Request;
using httplib::Response;
using httplib::Server;

namespace {

const string STUDENT_FILE = "data/students.txt";
const string EXAM_FILE = "data/exams.txt";
const string QUESTION_FILE = "data/questions.txt";
const string RESULT_FILE = "data/results.txt";
const string ANSWER_FILE = "data/answers.txt";
const int GRACE_SECONDS = 15;           // allowance for network delay on submit

struct Session {
    string role;                        // "student" or "admin"
    int studentId;
    string name;
};

mutex mu;                               // one request at a time: the data files are plain text
map<string, Session> sessions;
map<pair<int, int>, long long> attempts; // (studentId, examId) -> start time (seconds)

long long nowSec() {
    return chrono::duration_cast<chrono::seconds>(
               chrono::steady_clock::now().time_since_epoch()).count();
}

string js(const string& s) {            // JSON string literal
    string o = "\"";
    for (size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (c == '"') o += "\\\"";
        else if (c == '\\') o += "\\\\";
        else if (c == '\n') o += "\\n";
        else if (static_cast<unsigned char>(c) >= 32) o += c;
    }
    return o + "\"";
}

string pct(double v) {
    ostringstream o;
    o << fixed << setprecision(1) << v;
    return o.str();
}

void reply(Response& res, int status, const string& body) {
    res.status = status;
    res.set_content(body, "application/json");
}
void fail(Response& res, int status, const string& message) {
    reply(res, status, "{\"error\":" + js(message) + "}");
}

string newToken() {
    static mt19937_64 rng((random_device()()));
    ostringstream o;
    o << hex << rng() << rng() << rng();
    return o.str();
}

bool requireRole(const Request& req, Response& res, const string& role, Session& out) {
    const string header = req.get_header_value("Authorization");
    const string token = header.rfind("Bearer ", 0) == 0 ? header.substr(7) : "";
    map<string, Session>::const_iterator it = sessions.find(token);

    if (it == sessions.end() || it->second.role != role) {
        fail(res, 401, "Please sign in again.");
        return false;
    }
    out = it->second;
    return true;
}

string questionJson(const Question& q) {
    string o = "{\"id\":" + to_string(q.getId()) + ",\"type\":" + js(q.getType()) +
               ",\"text\":" + js(q.getText()) + ",\"marks\":" + to_string(q.getMarks()) +
               ",\"options\":[";
    const vector<string> options = q.getOptions();
    for (size_t i = 0; i < options.size(); ++i)
        o += (i ? "," : "") + js(options[i]);
    return o + "]}";
}

string resultJson(const Result& r, const string& title) {
    return "{\"examId\":" + to_string(r.getExamId()) + ",\"examTitle\":" + js(title) +
           ",\"score\":" + to_string(r.getScore()) + ",\"totalMarks\":" + to_string(r.getTotalMarks()) +
           ",\"percentage\":" + pct(r.getPercentage()) + ",\"grade\":" + js(r.getGrade()) +
           ",\"passed\":" + (r.isPassed() ? "true" : "false") +
           ",\"correct\":" + to_string(r.getCorrectCount()) + ",\"attempted\":" + to_string(r.getAttempted()) +
           ",\"totalQuestions\":" + to_string(r.getTotalQuestions()) +
           ",\"submittedAt\":" + js(r.getSubmittedAt()) + ",\"pending\":" + to_string(r.getPending()) + "}";
}

string titleOf(int examId) {
    ExamRepository repo;
    unique_ptr<Exam> e = repo.findById(examId, EXAM_FILE);
    return e ? e->getTitle() : "(deleted exam)";
}

Result finish(int studentId, const Exam& exam, const vector<unique_ptr<Question> >& questions,
              const AnswerSheet& sheet) {
    return SubmissionService(ANSWER_FILE, RESULT_FILE).submit(studentId, exam, questions, sheet);
}

} // namespace

int runApiServer(int port, const string& webRoot) {
    Server svr;
    AuthService auth(STUDENT_FILE);
    Admin admin(1, "Administrator", "admin", "Admin@123");   // same demo admin as the console app

    // ---------- sign in / out ----------
    svr.Post("/api/student/login", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        unique_ptr<Student> s = auth.loginStudent(req.get_param_value("username"),
                                                  req.get_param_value("password"));
        if (!s) return fail(res, 401, "Wrong username or password.");

        const string token = newToken();
        Session session = {"student", s->getId(), s->getName()};
        sessions[token] = session;
        reply(res, 200, "{\"token\":" + js(token) + ",\"student\":{\"id\":" + to_string(s->getId()) +
                        ",\"name\":" + js(s->getName()) + ",\"department\":" + js(s->getDepartment()) +
                        ",\"year\":" + to_string(s->getYear()) + "}}");
    });

    svr.Post("/api/admin/login", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        if (!auth.authenticate(&admin, req.get_param_value("username"), req.get_param_value("password")))
            return fail(res, 401, "Wrong username or password.");

        const string token = newToken();
        Session session = {"admin", 0, admin.getName()};
        sessions[token] = session;
        reply(res, 200, "{\"token\":" + js(token) + ",\"admin\":{\"name\":" + js(admin.getName()) + "}}");
    });

    svr.Post("/api/logout", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        const string header = req.get_header_value("Authorization");
        if (header.rfind("Bearer ", 0) == 0)
            sessions.erase(header.substr(7));
        reply(res, 200, "{\"ok\":true}");
    });

    svr.Post("/api/student/register", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        const vector<Student> all = StudentRepository().loadAll(STUDENT_FILE);
        int id = 1;
        for (size_t i = 0; i < all.size(); ++i)
            id = max(id, all[i].getId() + 1);

        string error;
        if (!auth.registerStudent(id, req.get_param_value("name"), req.get_param_value("username"),
                                  req.get_param_value("password"), req.get_param_value("department"),
                                  atoi(req.get_param_value("year").c_str()), error))
            return fail(res, 400, error);
        reply(res, 200, "{\"ok\":true,\"id\":" + to_string(id) + "}");
    });

    // ---------- student: exams and results ----------
    svr.Get("/api/exams", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "student", s)) return;

        const vector<Exam> exams = ExamRepository().loadAll(EXAM_FILE);
        QuestionService questions(QUESTION_FILE);
        ResultRepository results;

        string o = "[";
        for (size_t i = 0; i < exams.size(); ++i) {
            const Exam& e = exams[i];
            const bool done = results.hasAttempted(s.studentId, e.getExamId(), RESULT_FILE);
            const bool running = attempts.count(make_pair(s.studentId, e.getExamId())) > 0;
            o += string(i ? "," : "") + "{\"id\":" + to_string(e.getExamId()) + ",\"title\":" + js(e.getTitle()) +
                 ",\"duration\":" + to_string(e.getDuration()) +
                 ",\"questions\":" + to_string(questions.getQuestions(e.getExamId()).size()) +
                 ",\"attempted\":" + (done ? "true" : "false") +
                 ",\"inProgress\":" + (running && !done ? "true" : "false") + "}";
        }
        reply(res, 200, o + "]");
    });

    svr.Get("/api/student/results", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "student", s)) return;

        const vector<Result> list = ResultRepository().findByStudent(s.studentId, RESULT_FILE);
        string o = "[";
        for (size_t i = 0; i < list.size(); ++i)
            o += (i ? "," : "") + resultJson(list[i], titleOf(list[i].getExamId()));
        reply(res, 200, o + "]");
    });

    // ---------- student: take an exam ----------
    svr.Post(R"(/api/exams/(\d+)/start)", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "student", s)) return;

        const int examId = stoi(req.matches[1]);
        unique_ptr<Exam> exam = ExamRepository().findById(examId, EXAM_FILE);
        if (!exam) return fail(res, 404, "Exam not found.");
        if (ResultRepository().hasAttempted(s.studentId, examId, RESULT_FILE))
            return fail(res, 409, "You have already attempted this exam.");

        QuestionService service(QUESTION_FILE);
        vector<unique_ptr<Question> > questions = service.getQuestions(examId);
        if (questions.empty()) return fail(res, 400, "No questions have been added to this exam yet.");

        const pair<int, int> key(s.studentId, examId);
        if (!attempts.count(key)) attempts[key] = nowSec();     // a second call resumes the same attempt

        const long long left = exam->getDuration() * 60LL - (nowSec() - attempts[key]);
        if (left <= 0) {                                         // time ran out while away
            finish(s.studentId, *exam, questions, AnswerSheet());
            attempts.erase(key);
            return fail(res, 409, "Your time for this exam has ended. It was submitted with no answers.");
        }

        string o = "{\"exam\":{\"id\":" + to_string(examId) + ",\"title\":" + js(exam->getTitle()) +
                   ",\"duration\":" + to_string(exam->getDuration()) + "},\"secondsLeft\":" + to_string(left) +
                   ",\"questions\":[";
        for (size_t i = 0; i < questions.size(); ++i)
            o += (i ? "," : "") + questionJson(*questions[i]);
        reply(res, 200, o + "]}");
    });

    // One form field per answered question: a_<questionId>=<answer>   e.g. a_1=B&a_8=TRUE&a_11=free text
    svr.Post(R"(/api/exams/(\d+)/submit)", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "student", s)) return;

        const int examId = stoi(req.matches[1]);
        const pair<int, int> key(s.studentId, examId);
        unique_ptr<Exam> exam = ExamRepository().findById(examId, EXAM_FILE);
        if (!exam || !attempts.count(key)) return fail(res, 400, "This exam is not in progress.");

        QuestionService service(QUESTION_FILE);
        vector<unique_ptr<Question> > questions = service.getQuestions(examId);

        const bool late = nowSec() - attempts[key] > exam->getDuration() * 60LL + GRACE_SECONDS;
        AnswerSheet sheet;
        if (!late) {
            for (size_t i = 0; i < questions.size(); ++i) {
                const string key = "a_" + to_string(questions[i]->getId());
                if (!req.has_param(key)) continue;
                const string answer = questions[i]->normalizeAnswer(req.get_param_value(key));
                if (!answer.empty())
                    sheet.setAnswer(questions[i]->getId(), answer);
            }
        }

        const Result r = finish(s.studentId, *exam, questions, sheet);
        attempts.erase(key);

        string review = "[";
        for (size_t i = 0; i < questions.size(); ++i) {
            const Question& q = *questions[i];
            const string mine = sheet.getAnswer(q.getId());
            review += string(i ? "," : "") + "{\"question\":" + questionJson(q) + ",\"yourAnswer\":" + js(mine) +
                      ",\"correctAnswer\":" + js(q.getCorrectAnswer()) +
                      ",\"isCorrect\":" + (!mine.empty() && q.isCorrect(mine) ? "true" : "false") + "}";
        }
        reply(res, 200, "{\"late\":" + string(late ? "true" : "false") + ",\"result\":" +
                        resultJson(r, exam->getTitle()) + ",\"review\":" + review + "]}");
    });

    // ---------- admin: exams, questions, results ----------
    svr.Get("/api/admin/exams", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        const vector<Exam> exams = ExamRepository().loadAll(EXAM_FILE);
        const vector<Result> results = ResultRepository().loadAll(RESULT_FILE);
        QuestionService service(QUESTION_FILE);

        string o = "{\"students\":" + to_string(StudentRepository().loadAll(STUDENT_FILE).size()) +
                   ",\"attempts\":" + to_string(results.size()) + ",\"exams\":[";
        for (size_t i = 0; i < exams.size(); ++i) {
            const vector<unique_ptr<Question> > qs = service.getQuestions(exams[i].getExamId());
            int taken = 0;
            for (size_t j = 0; j < results.size(); ++j)
                if (results[j].getExamId() == exams[i].getExamId()) ++taken;
            o += string(i ? "," : "") + "{\"id\":" + to_string(exams[i].getExamId()) + ",\"title\":" + js(exams[i].getTitle()) +
                 ",\"duration\":" + to_string(exams[i].getDuration()) + ",\"planned\":" + to_string(exams[i].getTotalQuestions()) +
                 ",\"questions\":" + to_string(qs.size()) + ",\"totalMarks\":" + to_string(QuestionService::totalMarks(qs)) +
                 ",\"attempts\":" + to_string(taken) + "}";
        }
        reply(res, 200, o + "]}");
    });

    svr.Post("/api/admin/exams", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        const string title = req.get_param_value("title");
        const int duration = atoi(req.get_param_value("duration").c_str());
        const int planned = atoi(req.get_param_value("planned").c_str());
        if (!Validator::isValidText(title, 100)) return fail(res, 400, "Title must be 1-100 characters and cannot contain commas.");
        if (!Validator::isValidDuration(duration)) return fail(res, 400, "Duration must be between 1 and 600 minutes.");
        if (planned < 1 || planned > 200) return fail(res, 400, "Planned questions must be between 1 and 200.");

        ExamRepository repo;
        const vector<Exam> all = repo.loadAll(EXAM_FILE);
        int id = 1;
        for (size_t i = 0; i < all.size(); ++i)
            id = max(id, all[i].getExamId() + 1);
        if (!repo.saveExam(Exam(id, title, duration, planned), EXAM_FILE))
            return fail(res, 500, "The exam could not be saved.");
        reply(res, 200, "{\"id\":" + to_string(id) + "}");
    });

    svr.Get(R"(/api/admin/exams/(\d+)/questions)", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        QuestionService service(QUESTION_FILE);
        const vector<unique_ptr<Question> > qs = service.getQuestions(stoi(req.matches[1]));
        string o = "[";
        for (size_t i = 0; i < qs.size(); ++i)
            o += string(i ? "," : "") + "{\"question\":" + questionJson(*qs[i]) + ",\"correctAnswer\":" + js(qs[i]->getCorrectAnswer()) + "}";
        reply(res, 200, o + "]");
    });

    svr.Post("/api/admin/questions", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        const int examId = atoi(req.get_param_value("examId").c_str());
        if (!ExamRepository().findById(examId, EXAM_FILE)) return fail(res, 404, "Exam not found.");

        QuestionService service(QUESTION_FILE);
        const string type = req.get_param_value("type"), text = req.get_param_value("text"),
                     correct = StringUtils::toUpper(req.get_param_value("correct"));
        const int marks = atoi(req.get_param_value("marks").c_str());
        string error;
        bool ok = false;

        if (type == "MCQ") {
            vector<string> options;
            options.push_back(req.get_param_value("a")); options.push_back(req.get_param_value("b"));
            options.push_back(req.get_param_value("c")); options.push_back(req.get_param_value("d"));
            ok = service.addMCQ(examId, text, marks, options, correct.empty() ? 'X' : correct[0], error);
        } else if (type == "TF") {
            if (correct != "TRUE" && correct != "FALSE") return fail(res, 400, "Choose True or False as the correct answer.");
            ok = service.addTrueFalse(examId, text, marks, correct == "TRUE", error);
        } else if (type == "DESC") {
            ok = service.addDescriptive(examId, text, marks, error);
        } else {
            return fail(res, 400, "Unknown question type.");
        }
        if (!ok) return fail(res, 400, error);
        reply(res, 200, "{\"ok\":true}");
    });

    svr.Delete(R"(/api/admin/questions/(\d+))", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        string error;
        if (!QuestionService(QUESTION_FILE).removeQuestion(stoi(req.matches[1]), error)) return fail(res, 404, error);
        reply(res, 200, "{\"ok\":true}");
    });

    svr.Get(R"(/api/admin/exams/(\d+)/results)", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        const int examId = stoi(req.matches[1]);
        const vector<Result> list = ResultRepository().findByExam(examId, RESULT_FILE);
        const vector<Student> students = StudentRepository().loadAll(STUDENT_FILE);
        const string title = titleOf(examId);

        string o = "[";
        for (size_t i = 0; i < list.size(); ++i) {
            string name = "(unknown)";
            for (size_t j = 0; j < students.size(); ++j)
                if (students[j].getId() == list[i].getStudentId()) name = students[j].getName();
            o += string(i ? "," : "") + "{\"studentId\":" + to_string(list[i].getStudentId()) + ",\"studentName\":" + js(name) +
                 ",\"result\":" + resultJson(list[i], title) + "}";
        }
        reply(res, 200, o + "]");
    });

    // All attempts of all students, for the "Student scores" table.
    svr.Get("/api/admin/scores", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        const vector<Result> all = ResultRepository().loadAll(RESULT_FILE);
        const vector<Student> students = StudentRepository().loadAll(STUDENT_FILE);
        const vector<Exam> exams = ExamRepository().loadAll(EXAM_FILE);

        string o = "[";
        for (size_t i = 0; i < all.size(); ++i) {
            string name = "(unknown)", title = "(deleted exam)";
            for (size_t j = 0; j < students.size(); ++j)
                if (students[j].getId() == all[i].getStudentId()) name = students[j].getName();
            for (size_t j = 0; j < exams.size(); ++j)
                if (exams[j].getExamId() == all[i].getExamId()) title = exams[j].getTitle();
            o += string(i ? "," : "") + "{\"studentId\":" + to_string(all[i].getStudentId()) +
                 ",\"studentName\":" + js(name) + ",\"result\":" + resultJson(all[i], title) + "}";
        }
        reply(res, 200, o + "]");
    });

    // The written answers of one student for one exam (examId, studentId in the URL).
    svr.Get(R"(/api/admin/submissions/(\d+)/(\d+))", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        const int examId = stoi(req.matches[1]), studentId = stoi(req.matches[2]);
        const vector<DescriptiveAnswer> answers = AnswerRepository().findBySubmission(studentId, examId, ANSWER_FILE);
        const vector<unique_ptr<Question> > questions = QuestionService(QUESTION_FILE).getQuestions(examId);

        string o = "[";
        for (size_t i = 0; i < answers.size(); ++i) {
            string text = "(deleted question)";
            int maxMarks = 0;
            for (size_t j = 0; j < questions.size(); ++j)
                if (questions[j]->getId() == answers[i].questionId) {
                    text = questions[j]->getText();
                    maxMarks = questions[j]->getMarks();
                }
            o += string(i ? "," : "") + "{\"questionId\":" + to_string(answers[i].questionId) + ",\"question\":" + js(text) +
                 ",\"maxMarks\":" + to_string(maxMarks) + ",\"answer\":" + js(answers[i].text) +
                 ",\"awarded\":" + to_string(answers[i].awarded) + "}";
        }
        reply(res, 200, o + "]");
    });

    svr.Post("/api/admin/grade", [&](const Request& req, Response& res) {
        lock_guard<mutex> lock(mu);
        Session s;
        if (!requireRole(req, res, "admin", s)) return;

        const int examId = atoi(req.get_param_value("examId").c_str());
        if (req.get_param_value("marks").empty()) return fail(res, 400, "Enter the marks to give.");

        string error;
        Result updated(0, 0, 0, 0, 0, 0, 0, "");
        if (!GradingService(QUESTION_FILE, ANSWER_FILE, RESULT_FILE).grade(
                atoi(req.get_param_value("studentId").c_str()), examId,
                atoi(req.get_param_value("questionId").c_str()),
                atoi(req.get_param_value("marks").c_str()), error, &updated))
            return fail(res, 400, error);
        reply(res, 200, "{\"result\":" + resultJson(updated, titleOf(examId)) + "}");
    });

    // ---------- static frontend ----------
    if (!svr.set_mount_point("/", webRoot)) {
        cerr << "Could not find the '" << webRoot << "' folder. Run the server from the project root." << endl;
        return 1;
    }

    cout << "ExamEra server running at http://localhost:" << port << "  (Ctrl+C to stop)" << endl;
    if (!svr.listen("127.0.0.1", port)) {
        cerr << "Could not start on port " << port << " (is it already in use?)." << endl;
        return 1;
    }
    return 0;
}
