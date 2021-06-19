#include<bits/stdc++.h>
#define pb push_back
using namespace std;
typedef long long ll;


// a vector storing the courses ordered
vector<string> ordered_courses;

// lvl:     the level of each course
// IN:      the number of prerequisites for each course
// courses: the courses themselves
map<string, int> lvl, IN, courses;

// par: a map storing all the prerequisites of a course
// adj: the graph is stored in an adjacency list 
map<string, vector<string>> par, adj;

// par["math"] = vector<>

//  a data type for the users
class User{
    string username;
    vector<string> completedCourses;

    int register_course(){
        string courseCode;
        int idx;
        cout << "Enter the code of the course you want to register in capital letters\n";
        cin >> courseCode;

        vector<string>::iterator codeIt = find(ordered_courses.begin(), ordered_courses.end(), courseCode);
        if(codeIt == ordered_courses.end()){
            cout << "Incorrect course code.\n";
            return -1;
        }
        else{
            idx = codeIt - ordered_courses.begin();
            for(int i=0; i<idx; i++){
                codeIt = find(completedCourses.begin(), completedCourses.end(), ordered_courses[i]);
                if(codeIt == completedCourses.end()){
                    cout << "Course " << ordered_courses[i] << " is a missing prerequisite\n";
                    cout << "Register " << ordered_courses[i] << " first.\n";
                    return 0;
                }
            }
            
            if(completedCourses[0] == "None") completedCourses[0] = ordered_courses[idx];
            else completedCourses.push_back(ordered_courses[idx]);
            cout << "Course registered successfully.\n\n";
        }

        return 1;
    }

    public:
    User(string username, vector<string> courses): username(username){
        for(auto course : courses){
            completedCourses.pb(course);
        }
    }

    void showInfo(){
        cout << "username: " << username << "\nCompleted Courses: "; 
        for(auto course : completedCourses) cout << course << " ";
        cout << "\n\n";
    }


    // login function
    int login(){
        string option;
        cout << "\nHi " << username << ", welcome back\n";

        // taking the option from the user and verify it
        while(1){
            cout
                << "\n1- Register a course\n"
                << "2- Look at all registered courses\n"
                << "3- Log out\n"
                << "4- Delete user\n\n";

            cin >> option;

            // checking if the input is a number
            bool b = false;
            if (! option.empty() && std::all_of(option.begin(), option.end(), [](char c){return std::isdigit(c);})) {
                b = true;
            }


            if(!b){
                cout << "\nInvalid option, please choose a number from 1 to 4\n";
                continue;
            }
            else if(stoi(option) < 1 || stoi(option) > 4){
                cout << "\nInvalid option, please choose a number from 1 to 4\n";
                continue;
            }
            else if(stoi(option) == 1){

                while(1){
                    if(register_course() != -1) break;
                    
                    int confirm;
                    cout << "\nTo cancel enter 0, to try again enter 1: ";
                    cin >> confirm;
                    if(!confirm) break;
                }
            }
            else if(stoi(option) == 2){
                cout << "\nRegistered courses:\n";
                for(auto course : completedCourses){
                    cout << course << "\n";
                }
            }
            else if(stoi(option) == 3){
                cout << "Logged out succesfuly\n\n";
                return 1;
            }
            else{
                string confirm;
                cout << "confirm deleting the user " << username << " (y or n): ";
                cin >> confirm;
                if(confirm[0] == 'y' || confirm[0] == 'Y') return -1;
            }
        }

        // returning 1 indicates that operations ended successfully
        return 1;
    }

    // username getter
    string getUsername(){
        return username;
    }

    // courses getter
    vector<string> getCourses(){
        return completedCourses;
    }

    // == operator overloading
    bool operator == (const User &user){
        return user.username == username;
    }
};

// a doubly linked list containing all the registered users
list<User> registeredUsers;

// topological sort using BFS
void topSort_BFS(){
    queue<string> q;

    // setting the level of the first course as 0
    int curLvl = 0;


    // pushing all the courses with no prerequisites in the queue
    for(auto u : courses){
        string course = u.first;
        if(!IN[course] && lvl[course]==-1){

            // pushing the course in the queue
            q.push(course); 

            // setting the prerequsites of the first course as none
            par[course].pb("None");
        }
    }

    while(!q.empty()){
        int n=q.size();
        for(int i=0; i<n; i++){
            string nd = q.front();
            ordered_courses.pb(nd);
            lvl[nd] = curLvl;
            q.pop();

            for(auto ch : adj[nd]){
                IN[ch]--;
                par[ch].pb(nd);

                if(!IN[ch]) q.push(ch);
            }
        }

        curLvl++;
    }
}

int read_courses(){
    ifstream courses_data("courses_data.txt");
    string line, course, prerequisite;

    if(!courses_data){
        cout << "Can't find courses data. \nThe program will close.\n\n";
        return 0;
    }

    while(getline(courses_data, line)){
        if(line[0] == '0') break;


        course = line.substr(0, line.find(' '));
        prerequisite = line.substr(line.find(' ')+1);

        if(prerequisite != "None"){
            adj[prerequisite].pb(course);


            IN[course]++;
            courses[course]++;
            courses[prerequisite]++;

            lvl[course] = lvl[prerequisite] = -1;
        }
    }

    courses_data.close();
    return 1;
}



// reading users data
void readUsers(){
    ifstream users_data("users.txt");
    string line, username, course;
    vector<string> courses;

    // checking if the users file is opened succesfully
    if(!users_data){
        cout << "Can't find users data. \nThe program will close.\n\n";
        return ;
    }

    // getting all the lines of the file
    while(getline(users_data, line)){
        // reading the username and removing it from the line
        username = line.substr(0, line.find(':'));
        line = line.substr(line.find(':')+1);

        // clearing the courses vector
        courses.clear();

        // getting the courses registered by the user 
        while(line.find(',') != string::npos){
            course = line.substr(0, line.find(','));
            courses.pb(course);
            line = line.substr(line.find(',')+1);
        }

        // storing the user in the registeredUsers vector
        registeredUsers.push_back(User(username, courses));
    }

    users_data.close();
}


// creating new users
void createUser(){
    string username;
    bool alreadyRegistered = false;

    cout << "Enter the username: ";
    cin >> username;

    auto checkUser = [](bool &alreadyRegistered, string &username){
        for(auto user : registeredUsers){
            if(user.getUsername() == username){
                alreadyRegistered = true;
                break;
            }
        }
    };

    checkUser(alreadyRegistered, username);

    while(alreadyRegistered){
        alreadyRegistered = false;
        cout << "This username is already registered\n";
        cout << "Enter the username again (To cancel enter 0): ";
        cin >> username;
        if(username[0] == '0') return;

        checkUser(alreadyRegistered, username);
    }

    registeredUsers.pb(User(username, {"None"}));
}

// saving users to users.txt file befor closing the program
void saveUsers(){
    ofstream users_data("users.txt");
    string line;

    for(auto user : registeredUsers){
        line = user.getUsername() + ":";

        for(auto course : user.getCourses()){
            line += course +  ',';
        }
        users_data << line << '\n';
    }
}

// login function 
void login(string username){
    for(auto &user : registeredUsers){
        if(user.getUsername() == username){
            if(user.login() == -1){
                cout << user.getUsername() << " is deleted successfully\n";
                registeredUsers.remove(user);
            }
            return;
        }
    }

    cout << "\nThis user in not registered\n";
}

// sign up function
bool signUp(string username){
    for(auto user : registeredUsers){
        if(user.getUsername() == username){
            cout << "This user in already registered\n";
            return 0;
        }
    }

    registeredUsers.pb(User(username, {"None"}));
    login(username);
    return 1;
}



int main(){
    int option;
    string username;

    // reading all the courses in ce plan
    if(!read_courses()) return 0;
    // reading previously registered users form users.txt
    readUsers();

    // calling topological sort
    topSort_BFS();


    // displaying program home page
    while(1){
        cout 
            <<"\n1- login\n"
            <<"2- sign up\n"
            <<"3- show the plan\n"
            <<"4- close the program\n\n";

        cin >> option;
        if(option > 4 || option < 1){
            cout << "\n\nInvalid input, choose a number from 1 to 4\n";
            continue;
        }
        else if(option == 1){
            cout << "\nEnter the username: ";
            cin >> username;
            login(username);
        }
        else if(option == 2){
            cout << "\nEnter the username: ";
            cin >> username;
            while(!signUp(username)){
                cout << "Enter another username or enter 0 to cancel: ";
                cin >> username;
                if(username[0] == '0') break;
            }
        }
        else if(option == 3){
            cout << "A file called \"sorted_courses.txt\" is created for you.\n";
            ofstream sorted_courses("sorted_courses.txt");

            int level = 0;
            sorted_courses << "Semester " << level + 1 << "\n";

            for(auto course : ordered_courses){
                if(lvl[course] != level){
                    level++;
                    sorted_courses << "\nSemester " << level+1 << "\n";
                }
                sorted_courses << course << "\n";
            }
            sorted_courses.close();
        }
        else if(option == 4){
            cout << "\nClosing the program\n\n";
            break;
        }

    }

    saveUsers();
    return 0;
}
