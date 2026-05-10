#include <iostream>
#include <string>
#include <functional>
#include "sqlite3.h"
#include<conio.h>
using namespace std;
static int callback(void* data, int argc, char** argv, char** azCoName) {
    bool* found = (bool*)data;
    *found = true;
    return 0;
}
string hashPassword(string pass) {
    return to_string(hash<string>{}(pass));
}
string inputpassword() {
    string pass = "";
    char ch;
    while (true) {
        ch = _getch();
        if (ch == 13)
            break;
        else if (ch == 8) {
            if (!pass.empty()) {
                pass.pop_back();
                cout << "\b \b";
            }
        }
        else {
            pass += ch;
            cout << "*";
        }

    }
    cout << endl;
    return pass;
}
int main() {
    sqlite3* db;
    int chance = 3;
    int rc = sqlite3_open("C:/Users/Sweekar/source/repos/MySQLTest/users.db", &db);
    if (rc) {
        cout << "Can't open database\n";
        return 0;
    }
    cout << "Database opened successfully\n";
    const char* createTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "username TEXT PRIMARY KEY,"
        "password TEXT);";
    char* errMsg = 0;
    rc = sqlite3_exec(db, createTable, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cout << "Table creation failed\n";
        cout << errMsg << endl;
        sqlite3_free(errMsg);
    }
    else {
        cout << "Table ready\n";
    }
    while (true) {
        int choice;
        cout << "\n===LOGIN/REGISTER===\n";
        cout << "1.register\n2.login\n3.exit\nchoice:";
        cin >> choice;
        if (choice == 1) {
            bool found = false;
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            password = inputpassword();
            password = hashPassword(password);
            sqlite3_stmt* insertStmt;
            string insertSql =
                "INSERT INTO users(username,password) VALUES(?,?);";
            sqlite3_prepare_v2(db, insertSql.c_str(), -1, &insertStmt, NULL);
            sqlite3_bind_text(insertStmt, 1, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 2, password.c_str(), -1, SQLITE_STATIC);
            int insertResult = sqlite3_step(insertStmt);
            if (insertResult == SQLITE_DONE) {
                cout << "\nRegistration successful\n";
            }
            else {
                cout << "\nRegistration failed\n";
                cout << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(insertStmt);
        }
        else if(choice==2){
            if (chance==0) {
                cout << "\nyou ran out of chances to login try again later";
                return 0;
            }
            bool found = false;
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            password = inputpassword();
            password = hashPassword(password);
            sqlite3_stmt* stmt;
            string sql = "SELECT * FROM users WHERE username = ? AND password= ?;";
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
            int result = sqlite3_step(stmt);
            if (result == SQLITE_ROW) {
                cout << "\nLOGIN SUCCESSFUL\n";
                chance = 3;
                sqlite3_finalize(stmt);
            }
            else {
                cout << "\nwrong username or password\t try again\n";
                chance--;
                cout << "\t\tchance left: " << chance;
                sqlite3_finalize(stmt);
                continue;
            }
        }
        else if(choice==3){
            cout << "\n===THANK YOU===\n";
            break;
        }
        else {
            cout << "\ninvaldi choice";
        }
    }
    sqlite3_close(db);
    return 0;
}