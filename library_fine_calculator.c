/* library_fine_calculator_autoDue.c
   Library Fine Calculator with Auto 14-Day Due Date and Manual Return Date
   Includes AI-inspired fine prediction feature
   Author: Student
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 500
#define MAX_MEMBERS 200
#define MAX_TRANS 1000
#define FINE_PER_DAY 2
#define LOAN_PERIOD_DAYS 14

typedef struct {
    int day, month, year;
} Date;

typedef struct {
    int id;
    char title[100];
    char author[60];
    int totalCopies;
    int available;
} Book;

typedef struct {
    int id;
    char name[60];
    char phone[20];
    char email[60];
} Member;

typedef struct {
    int id;
    int bookId;
    int memberId;
    Date issueDate;
    Date dueDate;
    Date returnDate;
    int fine;
    int returned;
} Transaction;

Book books[MAX_BOOKS];
Member members[MAX_MEMBERS];
Transaction trans[MAX_TRANS];
int bookCount = 0, memberCount = 0, transCount = 0;
int nextBookId = 1, nextMemberId = 1, nextTransId = 1;

const char *BOOKS_FILE = "books.dat";
const char *MEMBERS_FILE = "members.dat";
const char *TRANS_FILE = "trans.dat";

/* --- Utility Functions --- */
int readInt() {
    int x;
    if (scanf("%d", &x) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    return x;
}

int isLeap(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

int daysInMonth(int month, int year) {
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && isLeap(year)) return 29;
    return days[month - 1];
}

/* --- Date Handling --- */
void inputDate(const char *msg, Date *d) {
    printf("%s (dd mm yyyy): ", msg);
    scanf("%d %d %d", &d->day, &d->month, &d->year);
}

void printDate(Date d) {
    printf("%02d/%02d/%04d", d.day, d.month, d.year);
}

int countDays(Date d) {
    int days = d.day;
    for (int i = 1; i < d.month; i++) days += daysInMonth(i, d.year);
    days += d.year * 365 + (d.year / 4 - d.year / 100 + d.year / 400);
    return days;
}

int daysBetween(Date d1, Date d2) {
    return countDays(d2) - countDays(d1);
}

Date addDays(Date d, int daysToAdd) {
    Date result = d;
    result.day += daysToAdd;
    while (result.day > daysInMonth(result.month, result.year)) {
        result.day -= daysInMonth(result.month, result.year);
        result.month++;
        if (result.month > 12) {
            result.month = 1;
            result.year++;
        }
    }
    return result;
}

/* --- File Handling --- */
void loadData() {
    FILE *f;
    f = fopen(BOOKS_FILE, "rb");
    if (f) { fread(&bookCount, sizeof(int), 1, f); fread(&nextBookId, sizeof(int), 1, f);
             fread(books, sizeof(Book), bookCount, f); fclose(f); }

    f = fopen(MEMBERS_FILE, "rb");
    if (f) { fread(&memberCount, sizeof(int), 1, f); fread(&nextMemberId, sizeof(int), 1, f);
             fread(members, sizeof(Member), memberCount, f); fclose(f); }

    f = fopen(TRANS_FILE, "rb");
    if (f) { fread(&transCount, sizeof(int), 1, f); fread(&nextTransId, sizeof(int), 1, f);
             fread(trans, sizeof(Transaction), transCount, f); fclose(f); }
}

void saveData() {
    FILE *f;
    f = fopen(BOOKS_FILE, "wb");
    if (f) { fwrite(&bookCount, sizeof(int), 1, f); fwrite(&nextBookId, sizeof(int), 1, f);
             fwrite(books, sizeof(Book), bookCount, f); fclose(f); }

    f = fopen(MEMBERS_FILE, "wb");
    if (f) { fwrite(&memberCount, sizeof(int), 1, f); fwrite(&nextMemberId, sizeof(int), 1, f);
             fwrite(members, sizeof(Member), memberCount, f); fclose(f); }

    f = fopen(TRANS_FILE, "wb");
    if (f) { fwrite(&transCount, sizeof(int), 1, f); fwrite(&nextTransId, sizeof(int), 1, f);
             fwrite(trans, sizeof(Transaction), transCount, f); fclose(f); }
}

/* --- CRUD Operations --- */
void addBook() {
    if (bookCount >= MAX_BOOKS) { printf("Book limit reached.\n"); return; }
    Book b;
    b.id = nextBookId++;
    getchar();
    printf("Enter title: "); fgets(b.title, sizeof(b.title), stdin); b.title[strcspn(b.title, "\n")] = 0;
    printf("Enter author: "); fgets(b.author, sizeof(b.author), stdin); b.author[strcspn(b.author, "\n")] = 0;
    printf("Enter total copies: "); b.totalCopies = readInt();
    b.available = b.totalCopies;
    books[bookCount++] = b;
    printf("Book added successfully with ID %d\n", b.id);
    saveData();
}

void viewBooks() {
    printf("\nID | Title | Author | Total | Available\n");
    for (int i = 0; i < bookCount; i++)
        printf("%d | %s | %s | %d | %d\n", books[i].id, books[i].title, books[i].author, books[i].totalCopies, books[i].available);
}

void addMember() {
    if (memberCount >= MAX_MEMBERS) { printf("Member limit reached.\n"); return; }
    Member m;
    m.id = nextMemberId++;
    getchar();
    printf("Enter name: "); fgets(m.name, sizeof(m.name), stdin); m.name[strcspn(m.name, "\n")] = 0;
    printf("Enter phone: "); fgets(m.phone, sizeof(m.phone), stdin); m.phone[strcspn(m.phone, "\n")] = 0;
    printf("Enter email: "); fgets(m.email, sizeof(m.email), stdin); m.email[strcspn(m.email, "\n")] = 0;
    members[memberCount++] = m;
    printf("Member added successfully with ID %d\n", m.id);
    saveData();
}

void viewMembers() {
    printf("\nID | Name | Phone | Email\n");
    for (int i = 0; i < memberCount; i++)
        printf("%d | %s | %s | %s\n", members[i].id, members[i].name, members[i].phone, members[i].email);
}

int findBookIndexById(int id) {
    for (int i = 0; i < bookCount; i++) if (books[i].id == id) return i;
    return -1;
}
int findMemberIndexById(int id) {
    for (int i = 0; i < memberCount; i++) if (members[i].id == id) return i;
    return -1;
}
int findActiveTransaction(int bookId, int memberId) {
    for (int i = 0; i < transCount; i++)
        if (trans[i].bookId == bookId && trans[i].memberId == memberId && trans[i].returned == 0)
            return i;
    return -1;
}

/* --- Issue Book --- */
void issueBook() {
    printf("Enter Book ID: "); int bid = readInt();
    int bi = findBookIndexById(bid);
    if (bi < 0) { printf("Book not found.\n"); return; }
    if (books[bi].available <= 0) { printf("No copies available.\n"); return; }

    printf("Enter Member ID: "); int mid = readInt();
    int mi = findMemberIndexById(mid);
    if (mi < 0) { printf("Member not found.\n"); return; }

    Transaction t;
    t.id = nextTransId++;
    t.bookId = bid;
    t.memberId = mid;

    inputDate("Enter Issue Date", &t.issueDate);
    t.dueDate = addDays(t.issueDate, LOAN_PERIOD_DAYS);  // Auto set due date
    t.returned = 0;
    t.fine = 0;

    trans[transCount++] = t;
    books[bi].available--;
    printf("\nBook issued successfully!\nTransaction ID: %d\n", t.id);
    printf("Due Date (14 days later): ");
    printDate(t.dueDate);
    printf("\n");
    saveData();
}

/* --- Return Book --- */
void returnBook() {
    printf("Enter Book ID: "); int bid = readInt();
    printf("Enter Member ID: "); int mid = readInt();
    int idx = findActiveTransaction(bid, mid);
    if (idx < 0) { printf("No active transaction found.\n"); return; }

    inputDate("Enter Return Date", &trans[idx].returnDate);
    int diff = daysBetween(trans[idx].dueDate, trans[idx].returnDate);
    trans[idx].fine = (diff > 0) ? diff * FINE_PER_DAY : 0;
    trans[idx].returned = 1;

    int bi = findBookIndexById(bid);
    if (bi >= 0) books[bi].available++;

    printf("\n--- Return Summary ---\n");
    printf("Book ID: %d | Member ID: %d\nDue Date: ", bid, mid);
    printDate(trans[idx].dueDate);
    printf(" | Return Date: "); printDate(trans[idx].returnDate);
    printf("\nDays Late: %d | Fine: %d\n", (diff > 0 ? diff : 0), trans[idx].fine);
    saveData();
}

/* --- View Transactions --- */
void viewTransactions() {
    printf("\nTID | BookID | MemberID | Issue | Due | Return | Fine\n");
    for (int i = 0; i < transCount; i++) {
        printf("%d | %d | %d | ", trans[i].id, trans[i].bookId, trans[i].memberId);
        printDate(trans[i].issueDate); printf(" | ");
        printDate(trans[i].dueDate); printf(" | ");
        if (trans[i].returned) printDate(trans[i].returnDate); else printf("Not Returned");
        printf(" | %d\n", trans[i].fine);
    }
}

/* --- AI Feature --- */
void predictLateReturn() {
    if (memberCount == 0 || transCount == 0) {
        printf("No data available for prediction.\n");
        return;
    }

    printf("\n--- Smart Fine Prediction ---\n");
    printf("Enter Member ID: ");
    int mid = readInt();

    int totalFine = 0, totalTrans = 0;
    for (int i = 0; i < transCount; i++) {
        if (trans[i].memberId == mid) {
            totalFine += trans[i].fine;
            totalTrans++;
        }
    }

    if (totalTrans == 0) {
        printf("Member has no fine history. Prediction: On-time return ✅\n");
        return;
    }

    float avgFine = (float)totalFine / totalTrans;
    printf("Average Fine: %.2f\n", avgFine);
    if (avgFine > 5)
        printf("Prediction: LIKELY late return ⚠️\n");
    else
        printf("Prediction: LIKELY on-time return ✅\n");
}

/* --- Menu --- */
void menu() {
    int choice;
    while (1) {
        printf("\n--- Library Fine Calculator ---\n");
        printf("1. Add Book\n2. View Books\n3. Add Member\n4. View Members\n");
        printf("5. Issue Book (Auto 14-day Due Date)\n6. Return Book\n7. View Transactions\n");
        printf("8. Predict Late Return (AI Feature)\n9. Exit\n");
        printf("Enter choice: ");
        choice = readInt();

        switch (choice) {
            case 1: addBook(); break;
            case 2: viewBooks(); break;
            case 3: addMember(); break;
            case 4: viewMembers(); break;
            case 5: issueBook(); break;
            case 6: returnBook(); break;
            case 7: viewTransactions(); break;
            case 8: predictLateReturn(); break;
            case 9: printf("Exiting...\n"); saveData(); exit(0);
            default: printf("Invalid choice.\n");
        }
    }
}

int main() {
    loadData();
    menu();
    return 0;
}
