#include <iostream>
#include <cstring>
#include <cstdlib>
#include <conio.h>
#include <windows.h>

using namespace std;

// Node for linked list
struct Donor {
	string title;
    char name[50];
    char bloodType[5];
    char donationDate[11];
    Donor* next;
};

// Global variables
Donor* head = NULL; // Head of the linked list
Donor* tail = NULL; // Tail for queue-like operations

// Function declarations
void login();
// 
void recipientMenu();
void doctorMenu();
void addDonor();
void editDonor();
void displayDonorsByDate();
void displayDonors();
void searchByBloodType();
void deleteExpiredDonor();

// Helper function to clear the terminal
/*void clearScreen() {
    for (int i = 0; i < 50; ++i) {
        cout << "\n";
    }
}*/

// Helper function for login
void printTitle(string title) {
    for (int i = 0; i < 20; i++) { // repeat 20 times
        system("cls"); // clear console
        for (int j = 0; j < i; j++) {
            cout << " "; // print spaces before title
        }
        cout << title << endl;
        Sleep(20); // wait for 100 milliseconds
    }
}
void printTitle1(string title1) {
    for (int i = 0; i < 20; i++) { // repeat 20 times
        system("cls"); // clear console
        for (int j = 0; j < i; j++) {
            cout << " "; // print spaces before title
        }
        cout << title1 << endl;
       Sleep(10); // wait for 100 milliseconds
    }
}
/*void printTitle2(string title2) {
    for (int i = 0; i < 20; i++) { // repeat 20 times
        system("cls"); // clear console
        for (int j = 0; j < i; j++) {
            cout << " "; // print spaces before title
        }
        cout << title2 << endl;
        Sleep(10); // wait for 100 milliseconds
    }
}
*/
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

// Function to validate blood type
bool isValidBloodType(const char* bloodType) {
    const char* validBloodTypes[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    for (int i = 0; i < 8; ++i) {
        if (strcmp(bloodType, validBloodTypes[i]) == 0) return true;
    }
    return false;
}

// Function to validate date format (YYYY-MM-DD)
bool isValidDate(const char* date) {
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(date[i])) return false;
    }
    return true;
}

// Function to add a new donor
void addDonor() {
    Donor* newDonor = new Donor;

    cout << "Enter donor name: ";
    cin.ignore();
    cin.getline(newDonor->name, 50);

    cout << "Enter blood type (A+, A-, B+, B-, AB+, AB-, O+, O-): ";
    cin.getline(newDonor->bloodType, 5);

    while (!isValidBloodType(newDonor->bloodType)) {
        cout << "Invalid blood type. Enter again: ";
        cin.getline(newDonor->bloodType, 5);
    }

    cout << "Enter donation date (YYYY-MM-DD): ";
    cin.getline(newDonor->donationDate, 11);

    while (!isValidDate(newDonor->donationDate)) {
        cout << "Invalid date format. Enter again (YYYY-MM-DD): ";
        cin.getline(newDonor->donationDate, 11);
    }

    newDonor->next = NULL;

    // Add to linked list (queue behavior)
    if (head == NULL) {
        head = newDonor;
        tail = newDonor;
    } else {
        tail->next = newDonor;
        tail = newDonor;
    }

    cout << "Donor added successfully!\n";
}

// Function to edit donor information
void editDonor() {
    char name[50];
    cout << "Enter the name of the donor to edit: ";
    cin.ignore();
    cin.getline(name, 50);

    Donor* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            cout << "Enter new blood type (A+, A-, B+, B-, AB+, AB-, O+, O-): ";
            cin.getline(temp->bloodType, 5);

            while (!isValidBloodType(temp->bloodType)) {
                cout << "Invalid blood type. Enter again: ";
                cin.getline(temp->bloodType, 5);
            }

            cout << "Enter new donation date (YYYY-MM-DD): ";
            cin.getline(temp->donationDate, 11);

            while (!isValidDate(temp->donationDate)) {
                cout << "Invalid date format. Enter again (YYYY-MM-DD): ";
                cin.getline(temp->donationDate, 11);
            }

            cout << "Donor information updated successfully!\n";
            return;
        }
        temp = temp->next;
    }

    cout << "Donor not found.\n";
}

// Function to display donors by donation date
void displayDonorsByDate() {
    if (head == NULL) {
        cout << "No donors to display.\n";
        return;
    }

    cout << "Donors in donation date order:\n";
    Donor* temp = head;
    while (temp != NULL) {
        cout << "Name: " << temp->name << ", Blood Type: " << temp->bloodType
             << ", Donation Date: " << temp->donationDate << "\n";
        temp = temp->next;
    }
    getch();
}

// Function to display all donors
void displayDonors() {
    if (head == NULL) {
        cout << "No donors to display.\n";
        return;
    }

    cout << "List of all donors:\n";
    Donor* temp = head;
    while (temp != NULL) {
        cout << "Name: " << temp->name << ", Blood Type: " << temp->bloodType
             << ", Donation Date: " << temp->donationDate << "\n";
        temp = temp->next;
    }
    getch();
}

// Function to search donors by blood type
void searchByBloodType() {
    char bloodType[5];
    cout << "Enter blood type to search: ";
    cin.ignore();
    cin.getline(bloodType, 5);

    if (!isValidBloodType(bloodType)) {
        cout << "Invalid blood type.\n";
        return;
    }

    Donor* temp = head;
    bool found = false;
    while (temp != NULL) {
        if (strcmp(temp->bloodType, bloodType) == 0) {
            cout << "Donor found: Name: " << temp->name << ", Donation Date: " << temp->donationDate << "\n";
            found = true;
        }
        temp = temp->next;
    }

    if (!found) {
        cout << "No donor found with the specified blood type.\n";
    }
}

// Function to delete expired or used donors
void deleteExpiredDonor() {
    char name[50];
    cout << "Enter the name of the donor to delete: ";
    cin.ignore();
    cin.getline(name, 50);

    Donor* temp = head;
    Donor* prev = NULL;

    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            if (prev == NULL) {
                head = temp->next;
            } else {
                prev->next = temp->next;
            }

            if (temp == tail) {
                tail = prev;
            }

            delete temp;
            cout << "Donor deleted successfully!\n";
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    cout << "Donor not found.\n";
}

// Recipient menu
void recipientMenu() {
    int choice;
    do {
        //clearScreen();
        system("cls");
        cout << "\nRecipient Menu:\n";
        cout << "1. Add Donor\n";
        cout << "2. Edit Donor\n";
        cout << "3. Display Donors by Donation Date\n";
        cout << "4. Exit\n";
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
                cout << "Exiting recipient menu...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 4);
}

// Doctor menu
void doctorMenu() {
	char ch = 'y';
    int choice;
    do {
        // clearScreen();
        system("cls");
        cout << "\nDoctor Menu:\n";
        cout << "1. Display All Donors\n";
        cout << "2. Search Donor by Blood Type\n";
        cout << "3. Delete Donor\n";
        cout << "4. Exit\n";
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
                cout << "Exiting doctor menu...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
        if (choice != 1 && ch == 'y') {
            cout << "Press any key to continue...\n";
            getch();
        }

    } while (choice != 4);
}

// Main function
int main() {
	
	string title = "BLOOD-DONATION-MANAGMENT SYSTEM";
	string title1 = "WELLCOME";
	//string title2 = "IS MADE BY ";
	 printTitle(title);
    printTitle1(title1);
    //printTitle2(title2);
    cout<<"____    __    ____  _______  __        ______   ______   .___  ___.  _______ "<<endl;
	cout<<" \   \  /  \  /   / |   ____||  |      /      | /  __  \  |   \/   | |   ____|"<<endl;
	cout<<"  \   \/    \/   /  |  |__   |  |     |  ,----'|  |  |  | |  \  /  | |  |__   "<<endl;
	cout<<"   \            /   |   __|  |  |     |  |     |  |  |  | |  |\/|  | |   __|  "<<endl;
	cout<<"    \    /\    /    |  |____ |  `----.|  `----.|  `--'  | |  |  |  | |  |____ "<<endl;
	cout<<"     \__/  \__/     |_______||_______| \______| \______/  |__|  |__| |_______|"<<endl;
    cout<<endl;
    cout<<endl;
   
    
    cout<<"is made by"<<endl;
    cout << "   MEMBERS NAME               ID NUMBER" << endl;
    cout << "1. Kalkidan K/Mariam          DBU1501713" << endl;
    cout << "2. Tinbite Elias              DBUXXXXX" << endl;
    cout << "3. Betelhem Hiluf             DBU1501054" << endl;

    login();

    int userType;
    cout << "\nWelcome to the Blood Donation Management System\n";
    cout << "1. Recipient\n";
    cout << "2. Doctor\n";
    cout << "Enter your user type: ";
    cin >> userType;

    if (userType == 1) {
        recipientMenu();
    } else if (userType == 2) {
        doctorMenu();
    } else {
        cout << "Invalid user type. Exiting...\n";
    }

    return 0;
}
