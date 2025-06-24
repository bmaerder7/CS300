// ProjectTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Brooks Maerder
// CS300

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

const unsigned int DEFAULT_SIZE = 179;

//define course structure
struct Course {
    string courseId;
    string courseName;
    vector<string> prerequisites;
};

//Hash Table class definition
class HashTable {
private:
    //structure to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        //default constructor (empty)
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        //implement with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node() {
            course = aCourse;
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(const std::string& key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    std::vector<Course> SortCourses();
    void PrintAll();
    void PrintCourse();
    bool isEmpty();
};

//default hash table constructor
HashTable::HashTable() {
    nodes.resize(tableSize);
}

//constructor for specifying size of the table
HashTable::HashTable(unsigned int size) {
    this->tableSize = size;
    nodes.resize(size);
}

//destructor
HashTable::~HashTable() {
    for (Node& bucket : nodes) {
        Node* current = bucket.next;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
}

//calculate hash value for course
unsigned int HashTable::hash(const std::string& key) {
    unsigned int hashValue = 0;
    for (char c : key) {
        hashValue = (hashValue * 31) + c;
    }
    return hashValue % tableSize;
}

//insert a course
void HashTable::Insert(Course course) {
    unsigned int hashVal = hash(course.courseId);
    Node* bucket = &nodes[hashVal];

    if (bucket->key == UINT_MAX) {
        *bucket = Node(course, hashVal);
        return;
    }

    Node* current = bucket;
    while (true) {
        if (current->course.courseId == course.courseId) {
            cout << "Duplicate course detected: " << course.courseId << ". Bypassing insertion" << endl;
            return;
        }
        if (current->next == nullptr) {
            break;
        }
        current = current->next;
    }
    current->next = new Node(course, hashVal);
}

//vector sorting logic (used in other functions only)
void VectorSort(std::vector<Course>& courses) {
    for (int i = 0; i < courses.size(); ++i) {
        for (int j = i + 1; j < courses.size(); ++j) {
            if (courses[i].courseId > courses[j].courseId) {
                Course temp = courses[j];
                courses[j] = courses[i];
                courses[i] = temp;
            }
            }
    }
}

//populate a vector with course data for sorting(previous function)
std::vector<Course> HashTable::SortCourses() {
    vector<Course> courses;
    for (const Node& bucket : nodes) {
        const Node* current = &bucket;
        while (current != nullptr && current->key != UINT_MAX) {
            courses.push_back(current->course);
            current = current->next;
        }
    }
    VectorSort(courses);
    return courses;
}

//prints all courses(after sorting)
void HashTable::PrintAll() {
    std::vector<Course> courses = SortCourses();
    cout << endl << "Course List:" << endl;
    for (int i = 0; i < courses.size(); ++i) {
        cout  << courses[i].courseId << ", " << courses[i].courseName << endl;
    }
}

void HashTable::PrintCourse() {
    string userInput;
    cout << "Which course would you like to know about?" << endl;
    cin >> userInput;
    for (char& c : userInput) {
        c = toupper(c);
        }
    for (const Node& bucket : nodes) {
        const Node* current = &bucket;
        while (current != nullptr && current->key != UINT_MAX) {
            if (userInput == current->course.courseId) {
                cout << endl << "Course Details:" << endl << current->course.courseId << ", " << current->course.courseName << endl;
                if (!current->course.prerequisites.empty()) {
                    cout << "Prerequisites: ";
                    for (int i = 0; i < current->course.prerequisites.size(); ++i) {
                        if (i == current->course.prerequisites.size() - 1)
                            cout << current->course.prerequisites[i] << endl;
                        else {
                            cout << current->course.prerequisites[i] << ", ";
                        }
                    }
                }
                return;
            }
            current = current->next;
        }
    }
    cout << "Course " << userInput << " not found." << endl;
}

bool HashTable::isEmpty() {
    for (const Node& node : nodes) {
        if (node.key != UINT_MAX) {
            return false;
        }
    }
    return true;
}

void loadCourses(string fileName, HashTable* hashTable) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "ERROR File: '" << fileName << "' failed to open properly or was not found" << endl;
        return;
    }

    string line;
    //skip header line
    getline(file, line); 

    while (getline(file, line)) {
        stringstream ss(line);
        string courseId;
        string courseName;
        string prereq1;
        string prereq2;

        getline(ss, courseId, ',');
        getline(ss, courseName, ',');

        if (courseId.empty() || courseName.empty()) {
            cout << "Invalid line format: " << line << " was skipped" << endl;
            continue;
        }

        Course course;
        for (char& c : courseId) {
            c = toupper(c);
        }
        course.courseId = courseId;
        course.courseName = courseName;
        string prereq;
        while (getline(ss, prereq, ',')) {
            if (!prereq.empty()) {
                for (char& c : prereq) {
                    c = toupper(c);
                }
                course.prerequisites.push_back(prereq);
            }
        }
        hashTable->Insert(course);
    }
    file.close();
    cout << "Data from file: '" << fileName << "' opened successfully" << endl;
}

void displayMenu() {
    cout << "Welcome to the Course Planner" << endl;
    int choice = 0;
    string pathChoice = "";
    string fileName = "CS 300 ABCU_Advising_Program_Input.csv";
    HashTable* hashTable = new HashTable();
    while (choice != 9) {
        cout << endl;
        cout << "Menu Directory" << endl;
        cout << endl;
        cout << "1. Load Data Structure" << endl;
        cout << "2. Print Course List" << endl;
        cout << "3. Print Course" << endl;
        cout << "9. Exit" << endl;

        string input;
        cin >> input;
        if (all_of(input.begin(), input.end(), ::isdigit)) {
            choice = stoi(input);
        }
        else {
            cout << endl << "Invalid input type, please enter a number" << endl;
            continue;
        }
        switch (choice) {
        case 1:
            cout << "What file would like to use?" << endl;
            cout << "1. Default csv provided(used for testing)" << endl;
            cout << "2. Enter my own file path" << endl;
            cin >> pathChoice;
            if (pathChoice == "1") {
                loadCourses(fileName, hashTable);
                break;
            }
            if (pathChoice == "2") {
                cout << "Please copy and paste the path of your desired file" << endl;
                string desiredPath;
                cin >> desiredPath;
                loadCourses(desiredPath, hashTable);
                break;
            }
            else {
                cout << "Invalid selection: " << pathChoice << endl;
                break;
            }
        case 2:
            if (hashTable->isEmpty()) {
                cout << "No data has been loaded yet" << endl;
            }
            else {
                hashTable->PrintAll();
            }
            break;
        case 3:
            if (hashTable->isEmpty()) {
                cout << "No data has been loaded yet" << endl;
            }
            else {
                hashTable->PrintCourse();
            }
            break;
        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;
        default:
            cout << choice << " is not a valid option" << endl;
            break;
        }
    }
    delete hashTable;
}

int main()
{
    displayMenu();
    return 0;
}