#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
using namespace std;

struct Booking {
    string host;
    int roomNumber;
    string startTime;
    string endTime;
    int chairsBooked;
};

struct Room {
    int roomNumber;
    int capacity;
    string purpose;
    vector<string> equipment;
};

int timeToMinutes(const string& time) {
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}

string toLower(const string& str) {
    string result = str;
    for (size_t i = 0; i < result.length(); ++i)
        result[i] = tolower(result[i]);
    return result;
}

class RoomBookingSystem {
private:
    unordered_map<int, Room> roomCatalog;
    unordered_map<int, vector<Booking>> roomBookings;

public:
    void addRoom() {
        Room r;
        cout << "Enter Room Number: ";
        cin >> r.roomNumber;
        cout << "Enter Room Capacity: ";
        cin >> r.capacity;
        cout << "Enter Room Purpose (Meeting/Lecture/Exam/General/etc): ";
        cin >> ws;
        getline(cin, r.purpose);

        cout << "Enter Equipment (comma-separated, e.g., Projector,Whiteboard): ";
        string equipLine;
        getline(cin, equipLine);

        size_t pos = 0;
        while ((pos = equipLine.find(',')) != string::npos) {
            r.equipment.push_back(equipLine.substr(0, pos));
            equipLine.erase(0, pos + 1);
        }
        if (!equipLine.empty()) r.equipment.push_back(equipLine);

        roomCatalog[r.roomNumber] = r;
        cout << "\n Room " << r.roomNumber << " registered successfully.\n";
    }

    void bookRoom() {
        int requiredCapacity;
        string requiredPurpose;
        cout << "Enter required capacity: ";
        cin >> requiredCapacity;
        cout << "Enter purpose (Meeting/Lecture/Exam/etc): ";
        cin >> ws;
        getline(cin, requiredPurpose);

        vector<int> matchedRooms;
        for (unordered_map<int, Room>::iterator it = roomCatalog.begin(); it != roomCatalog.end(); ++it) {
            int roomNo = it->first;
            Room& room = it->second;
            if (room.capacity >= requiredCapacity &&
                (toLower(room.purpose) == toLower(requiredPurpose) || toLower(room.purpose) == "general")) {
                if (toLower(room.purpose) == "general") {
                    cout << " Note: Room " << roomNo << " is a general-purpose room.\n";
                }
                matchedRooms.push_back(roomNo);
            }
        }

        if (matchedRooms.empty()) {
            cout << " No rooms available matching your requirements.\n";
            return;
        }

        cout << "Available Rooms: ";
        for (size_t i = 0; i < matchedRooms.size(); ++i) cout << matchedRooms[i] << " ";
        cout << "\nSelect a room to book: ";
        int chosenRoom;
        cin >> chosenRoom;

        if (roomCatalog.find(chosenRoom) == roomCatalog.end()) {
            cout << " Invalid room selected.\n";
            return;
        }

        Booking b;
        b.roomNumber = chosenRoom;
        cout << "Enter Host Name: ";
        getline(cin >> ws, b.host);
        cout << "Enter Start Time (HH:MM): ";
        cin >> b.startTime;
        cout << "Enter End Time (HH:MM): ";
        cin >> b.endTime;
        cout << "Enter Number of Chairs: ";
        cin >> b.chairsBooked;

        int newStart = timeToMinutes(b.startTime);
        int newEnd = timeToMinutes(b.endTime);

        vector<Booking>& existingBookings = roomBookings[chosenRoom];
        for (size_t i = 0; i < existingBookings.size(); ++i) {
            int existStart = timeToMinutes(existingBookings[i].startTime);
            int existEnd = timeToMinutes(existingBookings[i].endTime);
            if (newStart < existEnd && newEnd > existStart) {
                cout << " Conflict! Room " << b.roomNumber << " already booked from "
                     << existingBookings[i].startTime << " to " << existingBookings[i].endTime << ".\n";
                return;
            }
        }

        roomBookings[chosenRoom].push_back(b);
        cout << " Room " << b.roomNumber << " booked successfully.\n";
    }

    void checkRoomStatus() {
        int room;
        cout << "Enter Room Number to check: ";
        cin >> room;

        if (roomBookings.find(room) == roomBookings.end() || roomBookings[room].empty()) {
            cout << " Room " << room << " is currently available.\n";
        } else {
            cout << " Bookings for Room " << room << ":\n";
            for (size_t i = 0; i < roomBookings[room].size(); ++i) {
                Booking& b = roomBookings[room][i];
                cout << b.startTime << " - " << b.endTime << " | Host: " << b.host << " | Chairs: " << b.chairsBooked << "\n";
            }
        }
    }

    void viewAllBookings() {
        if (roomBookings.empty()) {
            cout << " No bookings in the system.\n";
            return;
        }

        cout << " All Room Bookings:\n";
        for (unordered_map<int, vector<Booking> >::iterator it = roomBookings.begin(); it != roomBookings.end(); ++it) {
            int room = it->first;
            vector<Booking>& bookings = it->second;
            for (size_t i = 0; i < bookings.size(); ++i) {
                Booking& b = bookings[i];
                cout << "Room " << room << " | " << b.startTime << " - " << b.endTime
                     << " | Host: " << b.host << " | Chairs: " << b.chairsBooked << "\n";
            }
        }
    }

    void cancelOrRescheduleBooking() {
        int room;
        string host;
        cout << "Enter Room Number: ";
        cin >> room;
        cout << "Enter Host Name: ";
        cin >> ws;
        getline(cin, host);

        vector<Booking>& bookings = roomBookings[room];
        for (vector<Booking>::iterator it = bookings.begin(); it != bookings.end(); ++it) {
            if (toLower(it->host) == toLower(host)) {
                cout << "Booking Found: " << it->startTime << " - " << it->endTime << ", Chairs: " << it->chairsBooked << "\n";
                cout << "Do you want to (1) Cancel or (2) Reschedule? ";
                int option;
                cin >> option;

                if (option == 1) {
                    bookings.erase(it);
                    cout << " Booking cancelled.\n";
                    return;
                } else if (option == 2) {
                    string newStart, newEnd;
                    cout << "Enter new Start Time (HH:MM): ";
                    cin >> newStart;
                    cout << "Enter new End Time (HH:MM): ";
                    cin >> newEnd;

                    int newStartMin = timeToMinutes(newStart);
                    int newEndMin = timeToMinutes(newEnd);

                    for (size_t i = 0; i < bookings.size(); ++i) {
                        if (toLower(bookings[i].host) != toLower(host)) {
                            int existStart = timeToMinutes(bookings[i].startTime);
                            int existEnd = timeToMinutes(bookings[i].endTime);
                            if (newStartMin < existEnd && newEndMin > existStart) {
                                cout << " Conflict with another booking: " << bookings[i].startTime << "-" << bookings[i].endTime << "\n";
                                return;
                            }
                        }
                    }

                    it->startTime = newStart;
                    it->endTime = newEnd;
                    cout << " Booking rescheduled.\n";
                    return;
                }
            }
        }

        cout << " No booking found for Host: " << host << " in Room: " << room << "\n";
    }

    void searchBookingsByHost() {
        string host;
        cout << "Enter Host Name to search: ";
        cin >> ws;
        getline(cin, host);

        bool found = false;
        for (unordered_map<int, vector<Booking> >::iterator it = roomBookings.begin(); it != roomBookings.end(); ++it) {
            int room = it->first;
            vector<Booking>& bookings = it->second;
            for (size_t i = 0; i < bookings.size(); ++i) {
                if (toLower(bookings[i].host) == toLower(host)) {
                    cout << "Room " << room << " | " << bookings[i].startTime << " - " << bookings[i].endTime << " | Chairs: " << bookings[i].chairsBooked << "\n";
                    found = true;
                }
            }
        }

        if (!found)
            cout << " No bookings found for host: " << host << "\n";
    }

    void searchBookingsByTime() {
        string searchStart, searchEnd;
        cout << "Enter Start Time (HH:MM): ";
        cin >> searchStart;
        cout << "Enter End Time (HH:MM): ";
        cin >> searchEnd;

        int searchStartMin = timeToMinutes(searchStart);
        int searchEndMin = timeToMinutes(searchEnd);

        bool found = false;
        for (unordered_map<int, vector<Booking> >::iterator it = roomBookings.begin(); it != roomBookings.end(); ++it) {
            int room = it->first;
            vector<Booking>& bookings = it->second;
            for (size_t i = 0; i < bookings.size(); ++i) {
                int bStart = timeToMinutes(bookings[i].startTime);
                int bEnd = timeToMinutes(bookings[i].endTime);

                if (searchStartMin < bEnd && searchEndMin > bStart) {
                    cout << "Room " << room << " | " << bookings[i].startTime << " - " << bookings[i].endTime
                         << " | Host: " << bookings[i].host << " | Chairs: " << bookings[i].chairsBooked << "\n";
                    found = true;
                }
            }
        }

        if (!found)
            cout << " No bookings overlapping with that time.\n";
    }
};

int main() {
    RoomBookingSystem system;
    int choice;

    while (true) {
        cout << "\n===== Room Booking System =====\n";
        cout << "1. Add Room\n";
        cout << "2. Book a Room\n";
        cout << "3. Check Room Status\n";
        cout << "4. View All Bookings\n";
        cout << "5. Cancel/Reschedule Booking\n";
        cout << "6. Search Bookings by Host\n";
        cout << "7. Search Bookings by Time Slot\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: system.addRoom(); break;
            case 2: system.bookRoom(); break;
            case 3: system.checkRoomStatus(); break;
            case 4: system.viewAllBookings(); break;
            case 5: system.cancelOrRescheduleBooking(); break;
            case 6: system.searchBookingsByHost(); break;
            case 7: system.searchBookingsByTime(); break;
            case 8: cout << "Exiting...\n"; return 0;
            default: cout << "Invalid choice. Try again.\n";
        }
    }
}