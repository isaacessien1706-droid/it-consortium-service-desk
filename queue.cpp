#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// Average time (in minutes) for an IT engineer to triage/resolve a ticket
const int AVG_RESOLUTION_MINS = 15;

// Node representing a single technical support ticket in heap memory
struct IncidentNode {
    string ticketId;        // e.g., API-101, DBA-102, NET-103
    string clientOrStaff;   // Reporter name or institution (e.g., EcoBank API Team)
    string issueSummary;    // Specific technical problem description
    string assignedSquad;   // Engineering unit assigned to the ticket
    IncidentNode* next;     // Pointer to the next ticket in the FIFO queue
};

class ItHelpdeskQueue {
private:
    IncidentNode* head;     // Front pointer (Dequeue point — oldest waiting ticket)
    IncidentNode* tail;     // Rear pointer (Enqueue point — newest logged ticket)
    int openTicketsCount;   // Current number of unresolved tickets waiting
    int nextTicketNumber;   // Auto-incrementing ticket sequence starting at 101
    int resolvedTodayCount; // Total incidents resolved in the current shift

public:
    // Constructor initializes an empty dynamic helpdesk queue
    ItHelpdeskQueue() {
        head = nullptr;
        tail = nullptr;
        openTicketsCount = 0;
        nextTicketNumber = 101;
        resolvedTodayCount = 0;
    }

    // Destructor deallocates all remaining heap nodes when program exits
    ~ItHelpdeskQueue() {
        while (!isEmpty()) {
            IncidentNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Check if the helpdesk queue is empty — O(1)
    bool isEmpty() const {
        return head == nullptr;
    }

    // ENQUEUE: Log a new technical ticket at the Tail of the linked list — O(1)
    void logTicket(const string& reporter, const string& issue, int category, bool silent = false) {
        string prefix, squad;

        switch (category) {
            case 1:
                prefix = "API-";
                squad = "Backend & API Integration Squad";
                break;
            case 2:
                prefix = "DBA-";
                squad = "Database & SQL Reliability Unit";
                break;
            case 3:
                prefix = "NET-";
                squad = "NOC Network & Cloud Security";
                break;
            default:
                prefix = "SYS-";
                squad = "IT Workstation & Hardware Desk";
                break;
        }

        // Dynamically allocate a new IncidentNode on the heap
        IncidentNode* newNode = new IncidentNode;
        newNode->ticketId = prefix + to_string(nextTicketNumber++);
        newNode->clientOrStaff = reporter;
        newNode->issueSummary = issue;
        newNode->assignedSquad = squad;
        newNode->next = nullptr;

        int estWaitMins = openTicketsCount * AVG_RESOLUTION_MINS;

        // Attach node to the Tail of the Linked List
        if (isEmpty()) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }

        openTicketsCount++;

        if (!silent) {
            cout << "\n+------------------------------------------------------------+\n";
            cout << "|       IT CONSORTIUM (ITC) - HELPDESK TICKET LOGGED         |\n";
            cout << "+------------------------------------------------------------+\n";
            cout << "  TICKET ID       : " << newNode->ticketId << "\n";
            cout << "  REPORTED BY     : " << reporter << "\n";
            cout << "  ISSUE SUMMARY   : " << issue << "\n";
            cout << "  ASSIGNED SQUAD  : " << squad << "\n";
            cout << "  QUEUE POSITION  : #" << openTicketsCount << " in FIFO buffer\n";
            cout << "  EST. SLA WAIT   : ~" << estWaitMins << " mins\n";
            cout << "+------------------------------------------------------------+\n";
        }
    }

    // DEQUEUE: Dispatch and remove the oldest ticket from the Head — O(1)
    void dispatchNextTicket() {
        if (isEmpty()) {
            cout << "\n[ALL CLEAR] Helpdesk queue is empty. Zero pending incidents!\n";
            return;
        }

        IncidentNode* dispatchedNode = head;

        cout << "\n==============================================================\n";
        cout << " [DISPATCHING TICKET] " << dispatchedNode->ticketId << " -> ASSIGNED TO ENGINEER\n";
        cout << "--------------------------------------------------------------\n";
        cout << " Reported By    : " << dispatchedNode->clientOrStaff << "\n";
        cout << " Technical Issue: " << dispatchedNode->issueSummary << "\n";
        cout << " Handled By     : " << dispatchedNode->assignedSquad << "\n";
        cout << "==============================================================\n";

        // Advance Head pointer to the next ticket in line
        head = head->next;
        if (head == nullptr) {
            tail = nullptr; // Queue is now completely empty
        }

        // Free heap memory for the resolved ticket
        delete dispatchedNode;

        openTicketsCount--;
        resolvedTodayCount++;

        cout << "Queue Update: " << openTicketsCount << " pending ticket(s) | "
             << resolvedTodayCount << " resolved today.\n";
    }

    // PEEK: Inspect the ticket at the Head without removing it — O(1)
    void peekNextTicket() const {
        if (isEmpty()) {
            cout << "\n[INFO] Queue is empty (HEAD == nullptr).\n";
            return;
        }

        cout << "\n[NEXT UP AT HEAD] Ticket " << head->ticketId
             << " | Reported by: " << head->clientOrStaff
             << "\n  Issue: \"" << head->issueSummary
             << "\" -> [" << head->assignedSquad << "]\n";
    }

    // DISPLAY: Traverse all waiting tickets from Head to Tail — O(n)
    void displayQueueBoard() const {
        if (isEmpty()) {
            cout << "\n[NOC MONITOR] No active support tickets in the queue.\n";
            return;
        }

        cout << "\n====================== IT CONSORTIUM LIVE HELPDESK QUEUE ======================\n";
        cout << left << setw(11) << "Order"
             << setw(10) << "Ticket"
             << setw(20) << "Reporter / Client"
             << setw(28) << "Technical Issue"
             << setw(10) << "Est. SLA"
             << "Assigned Squad\n";
        cout << string(105, '-') << "\n";

        IncidentNode* current = head;
        int pos = 1;

        while (current != nullptr) {
            string posTag = to_string(pos);
            if (current == head) posTag += " (HEAD)";
            else if (current == tail) posTag += " (TAIL)";

            int waitMins = (pos - 1) * AVG_RESOLUTION_MINS;
            string slaWait = to_string(waitMins) + "m";

            cout << left << setw(11) << posTag
                 << setw(10) << current->ticketId
                 << setw(20) << current->clientOrStaff.substr(0, 18)
                 << setw(28) << current->issueSummary.substr(0, 26)
                 << setw(10) << slaWait
                 << current->assignedSquad << "\n";

            current = current->next;
            pos++;
        }
        cout << string(105, '-') << "\n";
        cout << "Open Tickets (Heap Nodes): " << openTicketsCount
             << " | Resolved Today: " << resolvedTodayCount << "\n";
    }

    // DEMO LOADER: Populate 3 realistic IT firm tickets immediately
    void loadSampleIncidents() {
        logTicket("GCB Mobile App", "Payment Gateway 504 Timeout", 1, true);
        logTicket("Internal HR Portal", "MySQL Primary Key Deadlock", 2, true);
        logTicket("Remote Dev Team", "Cisco VPN Tunnel Drop", 3, true);
        cout << "\n[DEMO LOADED] 3 sample IT support incidents added to the queue!\n";
        displayQueueBoard();
    }
};

int main() {
    ItHelpdeskQueue itcServiceDesk;
    int choice, category;
    string reporter, issue;

    do {
        cout << "\n=== IT CONSORTIUM (ITC) HELPDESK QUEUE SYSTEM ===\n";
        cout << "1. Log New Support Ticket (Dynamic Enqueue at Tail)\n";
        cout << "2. Dispatch & Resolve Next Ticket (Dynamic Dequeue from Head)\n";
        cout << "3. Inspect Next Ticket in Line (Peek Head)\n";
        cout << "4. View Live NOC Helpdesk Queue Board\n";
        cout << "5. [Quick Demo] Auto-Load 3 Sample IT Incidents\n";
        cout << "0. Shut Down Helpdesk Terminal\n";
        cout << "Select operation: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\n[ERROR] Invalid input. Enter a number between 0 and 5.\n";
            continue;
        }
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter Client or Staff Name (e.g., CalBank API / Kwame - Dev): ";
                getline(cin, reporter);
                cout << "Describe Technical Issue (e.g., SSL Certificate Expired): ";
                getline(cin, issue);
                cout << "\nSelect Technical Category:\n";
                cout << "  1. API & Payment Gateway Integration\n";
                cout << "  2. Database / SQL Server Administration\n";
                cout << "  3. Network, Firewall & VPN Infrastructure\n";
                cout << "  4. Workstation OS & Hardware Support\n";
                cout << "Enter category (1-4): ";
                if (!(cin >> category)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    category = 4;
                }
                itcServiceDesk.logTicket(reporter, issue, category);
                break;

            case 2:
                itcServiceDesk.dispatchNextTicket();
                break;

            case 3:
                itcServiceDesk.peekNextTicket();
                break;

            case 4:
                itcServiceDesk.displayQueueBoard();
                break;

            case 5:
                itcServiceDesk.loadSampleIncidents();
                break;

            case 0:
                cout << "\nDeallocating dynamic heap nodes and closing ITC Helpdesk. Goodbye!\n";
                break;

            default:
                cout << "\n[ERROR] Choose a valid option between 0 and 5.\n";
        }
    } while (choice != 0);

    return 0;
}