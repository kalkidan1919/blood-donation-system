#include <iostream>
#include <cstring>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include "sqlite3.h"

using namespace std;

struct Donor {
    int id;
    char name[50];
    int age;
    char bloodType[5];
    char donationDate[11];
    Donor* next;
};
Donor* head = NULL;
Donor* tail = NULL;
sqlite3* db;
char* errMsg = 0;
void login();
void mainMenu();
void recipientMenu();
void doctorMenu();
void addDonor();
void editDonor();
void displayDonorsByDate();
void displayDonors();
void searchByBloodType();
void deleteExpiredDonor();
void loadDonorsFromDatabase();
void saveDonorToDatabase(Donor* donor);
void deleteDonorFromDatabase(int id);
void pauseAndContinue();
void openDatabase();
void closeDatabase();
void createTable();
void printTitle(string title) {
    for (int i = 0; i < 20; i++) {
        system("cls");
        for (int j = 0; j < i; j++) {
            cout << " ";
        }
        cout << title << endl;
        Sleep(20);
    }
}

void pauseAndContinue() {
    cout << "\nPress any key to continue...";
    getch();
    system("cls");
}

void login() {
    char password[20];
    const char correctPassword[] = "admin123";

    cout << "Enter password to access the system: ";
    cin >> password;

    if (strcmp(password, correctPassword) == 0) {
        cout << "Login successful!\n";
    } else {
        cout << "Incorrect password. Exiting...\n";
        exit(0);
    }
}
bool isValidBloodType(const char* bloodType) {
    const char* validBloodTypes[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    for (int i = 0; i < 8; ++i) {
        if (strcmp(bloodType, validBloodTypes[i]) == 0) return true;
    }
    return false;
}
bool isValidDonationDate(const char* date) {
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') return false;
    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);

    if (year < 2025) return false;
    if (month < 1 || month > 12) return false;

    if (day < 1 || day > 31) return false;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;

    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            if (day > 29) return false;
        } else {
            if (day > 28) return false;
        }
    }

    return true;
}
void loadDonorsFromDatabase() {
    const char* sql = "SELECT * FROM donors;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Donor* newDonor = new Donor;
            newDonor->id = sqlite3_column_int(stmt, 0);
            strcpy(newDonor->name, (const char*)sqlite3_column_text(stmt, 1));
            newDonor->age = sqlite3_column_int(stmt, 2);
            strcpy(newDonor->bloodType, (const char*)sqlite3_column_text(stmt, 3));
            strcpy(newDonor->donationDate, (const char*)sqlite3_column_text(stmt, 4));
            newDonor->next = NULL;

            if (head == NULL) {
                head = newDonor;
                tail = newDonor;
            } else {
                tail->next = newDonor;
                tail = newDonor;
            }
        }
    }
    sqlite3_finalize(stmt);
}
void saveDonorToDatabase(Donor* donor) {
    char sql[256];
    sprintf(sql, "INSERT INTO donors (name, age, bloodType, donationDate) VALUES ('%s', %d, '%s', '%s');",
            donor->name, donor->age, donor->bloodType, donor->donationDate);

    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        cout << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}

void deleteDonorFromDatabase(int id) {
    char sql[256];
    sprintf(sql, "DELETE FROM donors WHERE id = %d;", id);

    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        cout << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}

void addDonor() {
    Donor* newDonor = new Donor;
    const char* sql = "SELECT seq FROM sqlite_sequence WHERE name='donors';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            newDonor->id = sqlite3_column_int(stmt, 0) + 1; 
        } else {
    
            newDonor->id = 1;
        }
        sqlite3_finalize(stmt);
    } else {
        cout << "Failed to fetch the last ID from the database.\n";
        return;
    }

    cout << "Enter donor name: ";
    cin.ignore();
    cin.getline(newDonor->name, 50);

    cout << "Enter donor age: ";
    cin >> newDonor->age;
    while (newDonor->age < 18 || newDonor->age > 45) {
        cout << "Age must be between 18 and 45 to donate.\nEnter a valid age: ";
        cin >> newDonor->age;
    }
    cin.ignore();

    cout << "Enter blood type (A+, A-, B+, B-, AB+, AB-, O+, O-): ";
    cin.getline(newDonor->bloodType, 5);

    while (!isValidBloodType(newDonor->bloodType)) {
        cout << "Invalid blood type. Enter again: ";
        cin.getline(newDonor->bloodType, 5);
    }

    cout << "Enter donation date (YYYY-MM-DD): ";
    cin.getline(newDonor->donationDate, 11);

    while (!isValidDonationDate(newDonor->donationDate)) {
        cout << "Invalid date format or values. Enter again (YYYY-MM-DD): ";
        cin.getline(newDonor->donationDate, 11);
    }

    newDonor->next = NULL;

    if (head == NULL) {
        head = newDonor;
        tail = newDonor;
    } else {
        tail->next = newDonor;
        tail = newDonor;
    }

    saveDonorToDatabase(newDonor); // Save donor to database
    cout << "Donor added successfully! ID: " << newDonor->id << "\n";
    pauseAndContinue();
}
void editDonor() {
    int id;
    cout << "Enter the ID of the donor to edit: ";
    cin >> id;

    Donor* temp = head;
    while (temp != NULL) {
        if (temp->id == id) {
            char newName[50];
            int newAge = -1; 
            char newBloodType[5];
            char newDonationDate[11];

            cout << "Enter new name (leave blank to keep current): ";
            cin.ignore();
            cin.getline(newName, 50);
            cout << "Enter new age (leave blank to keep current): ";
            string ageInput;
            getline(cin, ageInput); 
            if (!ageInput.empty()) {
                newAge = stoi(ageInput); 
                if (newAge < 18 || newAge > 45) {
                    cout << "Age must be between 18 and 45 to donate.\n";
                    cout << "Keeping current age: " << temp->age << "\n";
                    newAge = -1; 
                }
            }

            cout << "Enter new blood type (leave blank to keep current): ";
            cin.getline(newBloodType, 5);
            cout << "Enter new donation date (leave blank to keep current): ";
            cin.getline(newDonationDate, 11);
            if (strlen(newName) > 0) {
                strcpy(temp->name, newName);
            }
            if (newAge != -1) {
                temp->age = newAge;
            }
            if (strlen(newBloodType) > 0) {
                strcpy(temp->bloodType, newBloodType);
            }
            if (strlen(newDonationDate) > 0) {
                strcpy(temp->donationDate, newDonationDate);
            }

            char sql[256];
            sprintf(sql, "UPDATE donors SET name = '%s', age = %d, bloodType = '%s', donationDate = '%s' WHERE id = %d;",
                    strlen(newName) > 0 ? temp->name : temp->name,
                    newAge != -1 ? temp->age : temp->age,
                    strlen(newBloodType) > 0 ? temp->bloodType : temp->bloodType,
                    strlen(newDonationDate) > 0 ? temp->donationDate : temp->donationDate,
                    id);

            if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
                cout << "SQL error: " << errMsg << endl;
                sqlite3_free(errMsg);
            } else {
                cout << "Donor information updated successfully!\n";
            }
            pauseAndContinue();
            return;
        }
        temp = temp->next;
    }

    cout << "Donor not found.\n";
    pauseAndContinue();
}
void displayDonorsByDate() {
    if (head == NULL) {
        cout << "No donors to display.\n";
        pauseAndContinue();
        return;
    }

    cout << "Donors in donation date order:\n";
    Donor* temp = head;
    while (temp != NULL) {
        cout << "ID: " << temp->id << ", Name: " << temp->name << ", Blood Type: " << temp->bloodType
             << ", Donation Date: " << temp->donationDate << "\n";
        temp = temp->next;
    }
    pauseAndContinue();
}

void displayDonors() {
    if (head == NULL) {
        cout << "No donors to display.\n";
        return;
    }

    cout << "List of all donors:\n";
    Donor* temp = head;
    while (temp != NULL) {
        cout << "ID: " << temp->id << ", Name: " << temp->name << ", Age: " << temp->age
             << ", Blood Type: " << temp->bloodType << ", Donation Date: " << temp->donationDate << "\n";
        temp = temp->next;
    }
    pauseAndContinue();
}

void searchByBloodType() {
    char bloodType[5];
    cout << "Enter blood type to search: ";
    cin.ignore();
    cin.getline(bloodType, 5);

    if (!isValidBloodType(bloodType)) {
        cout << "Invalid blood type.\n";
        pauseAndContinue();
        return;
    }

    Donor* temp = head;
    bool found = false;
    while (temp != NULL) {
        if (strcmp(temp->bloodType, bloodType) == 0) {
            cout << "Donor found: ID: " << temp->id << ", Name: " << temp->name
                 << ", Donation Date: " << temp->donationDate << "\n";
            found = true;
        }
        temp = temp->next;
    }

    if (!found) {
        cout << "No donor found with the specified blood type.\n";
    }
    pauseAndContinue();
}
void deleteExpiredDonor() {
    int id;
    cout << "Enter the ID of the donor to delete: ";
    cin >> id;

    Donor* temp = head;
    Donor* prev = NULL;

    while (temp != NULL) {
        if (temp->id == id) {
            if (prev == NULL) {
                head = temp->next; 
            } else {
                prev->next = temp->next; 
            }

            if (temp == tail) {
                tail = prev; 
            }

            
            deleteDonorFromDatabase(id); 

            delete temp;
            cout << "Donor deleted successfully!\n";
            pauseAndContinue();
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    cout << "Donor not found.\n";
    pauseAndContinue();
}
void recipientMenu() {
    int choice;
    do {
        system("cls");
        cout << "\nRecipient Menu:\n";
        cout << "1. Add Donor\n";
        cout << "2. Edit Donor\n";
        cout << "3. Display Donors by Donation Date\n";
        cout << "4. Exit to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addDonor();
                break;
            case 2:
                editDonor();
                break;
            case 3:
                displayDonorsByDate();
                break;
            case 4:
                cout << "Returning to Main Menu...\n";
                return;
            default:
                cout << "Invalid choice. Try again.\n";
                pauseAndContinue();
        }
    } while (choice != 4);
}
void doctorMenu() {
    int choice;
    do {
        system("cls");
        cout << "\nDoctor Menu:\n";
        cout << "1. Display All Donors\n";
        cout << "2. Search Donor by Blood Type\n";
        cout << "3. Delete Donor\n";
        cout << "4. Exit to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayDonors();
                break;
            case 2:
                searchByBloodType();
                break;
            case 3:
                deleteExpiredDonor();
                break;
            case 4:
                cout << "Returning to Main Menu...\n";
                return;
            default:
                cout << "Invalid choice. Try again.\n";
                pauseAndContinue();
        }
    } while (choice != 4);
}

void mainMenu() {
    int userType;
    do {
        system("cls");
        cout << "\nWelcome to the Blood Donation Management System\n";
        cout << "1. Recipient Menu\n";
        cout << "2. Doctor Menu\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> userType;

        switch (userType) {
            case 1:
                recipientMenu();
                break;
            case 2:
                doctorMenu();
                break;
            case 3:
                cout << "Exiting the system. Goodbye!\n";
                return;
            default:
                cout << "Invalid choice. Try again.\n";
                pauseAndContinue();
        }
    } while (userType != 3);
}

void openDatabase() {
    if (sqlite3_open("new_donors.db", &db) != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }
}

void closeDatabase() {
    sqlite3_close(db);
}

void createTable() {
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS donors ("
                                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "name TEXT NOT NULL, "
                                  "age INTEGER NOT NULL, "
                                  "bloodType TEXT NOT NULL, "
                                  "donationDate TEXT NOT NULL);";
    if (sqlite3_exec(db, createTableSQL, 0, 0, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}

int main() {
    openDatabase();
    createTable();
    loadDonorsFromDatabase();

    string title = "BLOOD-DONATION-MANAGEMENT SYSTEM";
    string title1 = "WELCOME";
    printTitle(title);
    printTitle(title1);
    cout << "____    __    ____  _______  __        ______   ______   .___  ___.  _______ " << endl;
    cout << " \   \\  /  \\  /   / |   ____||  |      /      | /  __  \\  |   \\/   | |   ____|" << endl;
    cout << "  \\   \\/    \\/   /  |  |__   |  |     |  ,----'|  |  |  | |  \\  /  | |  |__   " << endl;
    cout << "   \\            /   |   __|  |  |     |  |     |  |  |  | |  |\\/|  | |   __|  " << endl;
    cout << "    \\    /\\    /    |  |____ |  `----.|  `----.|  `--'  | |  |  |  | |  |____ " << endl;
    cout << "     \\__/  \\__/     |_______||_______| \\______| \\______/  |__|  |__| |_______|" << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "is made by" << endl;
    cout << "   MEMBERS NAME               ID NUMBER" << endl;
    cout << "1. Kalkidan K/Mariam          DBU1501713" << endl;
    cout << "2. Tinbite Elias              DBU1501714" << endl;
    cout << "3. Bethelhem  Hiluf           DBU1501054" << endl;
    cout << endl;

    login(); 
    mainMenu(); 

    closeDatabase(); 
    return 0;
}


