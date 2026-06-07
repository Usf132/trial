#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

// ========================
//       USER CLASS
// ========================
class User {
private:
    string username;
    string password;
    string phoneNumber;
    string status;
    string lastSeen;

public:
    User() : username(""), password(""), phoneNumber(""), status("Hey there! I am using WhatsApp."), lastSeen("") {
        updateLastSeen();
    }

    User(string uname, string pwd, string phone)
        : username(uname), password(pwd), phoneNumber(phone),
          status("Hey there! I am using WhatsApp.") {
        updateLastSeen();
    }

    string getUsername()    const { return username; }
    string getPhoneNumber() const { return phoneNumber; }
    string getStatus()      const { return status; }
    string getLastSeen()    const { return lastSeen; }

    void setStatus(string newStatus)    { status = newStatus; }
    void setPhoneNumber(string phone)   { phoneNumber = phone; }

    void updateLastSeen() {
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        lastSeen = string(buf);
    }

    bool checkPassword(string pwd) const { return password == pwd; }

    void changePassword(string newPwd) { password = newPwd; }
};

// ========================
//      MESSAGE CLASS
// ========================
class Message {
private:
    string sender;
    string content;
    string timestamp;
    string status;   // "sent", "delivered", "read"
    Message* replyTo;

public:
    Message() : sender(""), content(""), status("sent"), replyTo(nullptr) {
        updateTimestamp();
    }

    Message(string sndr, string cntnt)
        : sender(sndr), content(cntnt), status("sent"), replyTo(nullptr) {
        updateTimestamp();
    }

    string   getContent()   const { return content; }
    string   getSender()    const { return sender; }
    string   getTimestamp() const { return timestamp; }
    string   getStatus()    const { return status; }
    Message* getReplyTo()   const { return replyTo; }

    void setStatus(string newStatus) { status = newStatus; }
    void setReplyTo(Message* msg)    { replyTo = msg; }

    void updateTimestamp() {
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%H:%M", localtime(&now));
        timestamp = string(buf);
    }

    void addEmoji(string emojiCode) { content += " " + emojiCode; }

    void display() const {
        if (replyTo) {
            cout << "  | > [Reply to " << replyTo->getSender()
                 << "]: " << replyTo->getContent() << "\n";
        }
        cout << "  [" << timestamp << "] " << sender << ": " << content
             << "  (" << status << ")\n";
    }
};

// ========================
//       CHAT CLASS (BASE)
// ========================
class Chat {
protected:
    vector<string>  participants;
    vector<Message> messages;
    string          chatName;

public:
    Chat() : chatName("Unnamed Chat") {}

    Chat(vector<string> users, string name)
        : participants(users), chatName(name) {}

    string getChatName() const { return chatName; }

    const vector<string>& getParticipants() const { return participants; }

    void addMessage(const Message& msg) {
        messages.push_back(msg);
    }

    bool deleteMessage(int index, const string& username) {
        if (index < 0 || index >= (int)messages.size()) return false;
        if (messages[index].getSender() != username)     return false;
        messages.erase(messages.begin() + index);
        return true;
    }

    virtual void displayChat() const {
        cout << "\n=== " << chatName << " ===\n";
        if (messages.empty()) {
            cout << "  (No messages yet)\n";
        } else {
            for (const auto& m : messages) m.display();
        }
    }

    vector<Message> searchMessages(string keyword) const {
        vector<Message> results;
        string kw = keyword;
        transform(kw.begin(), kw.end(), kw.begin(), ::tolower);
        for (const auto& m : messages) {
            string content = m.getContent();
            transform(content.begin(), content.end(), content.begin(), ::tolower);
            if (content.find(kw) != string::npos) results.push_back(m);
        }
        return results;
    }

    void exportToFile(const string& filename) const {
        ofstream ofs(filename);
        if (!ofs) { cerr << "Cannot open file: " << filename << "\n"; return; }
        ofs << "Chat: " << chatName << "\n";
        ofs << string(40, '-') << "\n";
        for (const auto& m : messages) {
            ofs << "[" << m.getTimestamp() << "] "
                << m.getSender() << ": " << m.getContent() << "\n";
        }
        cout << "Chat exported to " << filename << "\n";
    }

    // Allow WhatsApp to access messages for in-chat interaction
    int messageCount() const { return (int)messages.size(); }

    Message* getMessageAt(int index) {
        if (index < 0 || index >= (int)messages.size()) return nullptr;
        return &messages[index];
    }
};

// ========================
//     PRIVATE CHAT CLASS
// ========================
class PrivateChat : public Chat {
private:
    string user1;
    string user2;

public:
    PrivateChat(string u1, string u2)
        : Chat({u1, u2}, u1 + " & " + u2), user1(u1), user2(u2) {}

    void displayChat() const override {
        cout << "\n=== Private Chat: " << chatName << " ===\n";
        if (messages.empty()) {
            cout << "  (No messages yet)\n";
        } else {
            for (const auto& m : messages) m.display();
        }
    }

    void showTypingIndicator(const string& username) const {
        cout << username << " is typing...\n";
    }

    bool hasUser(const string& uname) const {
        return user1 == uname || user2 == uname;
    }
};

// ========================
//      GROUP CHAT CLASS
// ========================
class GroupChat : public Chat {
private:
    vector<string> admins;
    string description;

public:
    GroupChat(vector<string> users, string name, string creator)
        : Chat(users, name), description("") {
        admins.push_back(creator);
        // Ensure creator is in participants
        if (find(participants.begin(), participants.end(), creator) == participants.end())
            participants.push_back(creator);
    }

    bool isAdmin(string username) const {
        return find(admins.begin(), admins.end(), username) != admins.end();
    }

    bool isParticipant(string username) const {
        return find(participants.begin(), participants.end(), username) != participants.end();
    }

    void addAdmin(string newAdmin) {
        if (!isAdmin(newAdmin) && isParticipant(newAdmin))
            admins.push_back(newAdmin);
    }

    bool removeParticipant(const string& admin, const string& userToRemove) {
        if (!isAdmin(admin)) { cout << "Only admins can remove participants.\n"; return false; }
        auto it = find(participants.begin(), participants.end(), userToRemove);
        if (it == participants.end()) { cout << "User not in group.\n"; return false; }
        participants.erase(it);
        // Also remove from admins if applicable
        auto ai = find(admins.begin(), admins.end(), userToRemove);
        if (ai != admins.end()) admins.erase(ai);
        cout << userToRemove << " removed from group.\n";
        return true;
    }

    void setDescription(string desc) { description = desc; }

    void sendJoinRequest(const string& username) {
        cout << username << " has requested to join \"" << chatName << "\".\n";
        cout << "(Admin approval required — feature stub)\n";
    }

    void displayChat() const override {
        cout << "\n=== Group: " << chatName << " ===\n";
        if (!description.empty()) cout << "  About: " << description << "\n";
        cout << "  Members (" << participants.size() << "): ";
        for (size_t i = 0; i < participants.size(); ++i)
            cout << participants[i] << (i + 1 < participants.size() ? ", " : "\n");
        if (messages.empty()) {
            cout << "  (No messages yet)\n";
        } else {
            for (const auto& m : messages) m.display();
        }
    }
};

// ========================
//    WHATSAPP APP CLASS
// ========================
class WhatsApp {
private:
    vector<User>  users;
    vector<Chat*> chats;
    int           currentUserIndex;

    int findUserIndex(const string& username) const {
        for (int i = 0; i < (int)users.size(); ++i)
            if (users[i].getUsername() == username) return i;
        return -1;
    }

    bool isLoggedIn() const { return currentUserIndex != -1; }

    string getCurrentUsername() const {
        if (!isLoggedIn()) return "";
        return users[currentUserIndex].getUsername();
    }

    // Find a private chat between two users
    Chat* findPrivateChat(const string& u1, const string& u2) {
        for (auto* c : chats) {
            auto* pc = dynamic_cast<PrivateChat*>(c);
            if (pc && pc->hasUser(u1) && pc->hasUser(u2)) return pc;
        }
        return nullptr;
    }

    // List chats the current user belongs to
    vector<Chat*> getUserChats() const {
        vector<Chat*> result;
        string me = getCurrentUsername();
        for (auto* c : chats) {
            const auto& parts = c->getParticipants();
            if (find(parts.begin(), parts.end(), me) != parts.end())
                result.push_back(c);
        }
        return result;
    }

    void inChatMenu(Chat* chat) {
        while (true) {
            cout << "\n--- " << chat->getChatName() << " ---\n";
            cout << "1. View messages\n2. Send message\n3. Reply to message\n";
            cout << "4. Delete my message\n5. Search messages\n6. Export chat\n7. Back\nChoice: ";
            int ch; cin >> ch; cin.ignore();

            if (ch == 1) {
                chat->displayChat();

            } else if (ch == 2) {
                cout << "Message: ";
                string text; getline(cin, text);
                Message msg(getCurrentUsername(), text);
                chat->addMessage(msg);
                cout << "Message sent.\n";

            } else if (ch == 3) {
                chat->displayChat();
                cout << "Reply to message number (1-based): ";
                int idx; cin >> idx; cin.ignore();
                Message* target = chat->getMessageAt(idx - 1);
                if (!target) { cout << "Invalid index.\n"; continue; }
                cout << "Your reply: ";
                string text; getline(cin, text);
                Message reply(getCurrentUsername(), text);
                reply.setReplyTo(target);
                chat->addMessage(reply);
                cout << "Reply sent.\n";

            } else if (ch == 4) {
                chat->displayChat();
                cout << "Delete message number (1-based): ";
                int idx; cin >> idx; cin.ignore();
                if (chat->deleteMessage(idx - 1, getCurrentUsername()))
                    cout << "Message deleted.\n";
                else
                    cout << "Cannot delete (wrong index or not your message).\n";

            } else if (ch == 5) {
                cout << "Search keyword: ";
                string kw; getline(cin, kw);
                auto found = chat->searchMessages(kw);
                if (found.empty()) cout << "No results found.\n";
                else for (const auto& m : found) m.display();

            } else if (ch == 6) {
                cout << "Filename (e.g. chat.txt): ";
                string fn; getline(cin, fn);
                chat->exportToFile(fn);

            } else if (ch == 7) {
                break;
            }
        }
    }

public:
    WhatsApp() : currentUserIndex(-1) {}

    ~WhatsApp() {
        for (auto* c : chats) delete c;
    }

    void signUp() {
        cout << "\n--- Sign Up ---\n";
        string uname, pwd, phone;
        cout << "Username: ";  cin >> uname;
        if (findUserIndex(uname) != -1) { cout << "Username already taken.\n"; return; }
        cout << "Password: ";  cin >> pwd;
        cout << "Phone:    ";  cin >> phone;
        users.emplace_back(uname, pwd, phone);
        cout << "Account created! Please log in.\n";
    }

    void login() {
        cout << "\n--- Login ---\n";
        string uname, pwd;
        cout << "Username: "; cin >> uname;
        cout << "Password: "; cin >> pwd;
        int idx = findUserIndex(uname);
        if (idx == -1 || !users[idx].checkPassword(pwd)) {
            cout << "Invalid credentials.\n"; return;
        }
        currentUserIndex = idx;
        users[currentUserIndex].updateLastSeen();
        cout << "Welcome, " << uname << "!\n";
    }

    void startPrivateChat() {
        cout << "\n--- Start Private Chat ---\n";
        cout << "Enter username to chat with: "; 
        string target; cin >> target;
        if (target == getCurrentUsername()) { cout << "You can't chat with yourself.\n"; return; }
        if (findUserIndex(target) == -1)    { cout << "User not found.\n"; return; }

        Chat* existing = findPrivateChat(getCurrentUsername(), target);
        if (!existing) {
            auto* pc = new PrivateChat(getCurrentUsername(), target);
            chats.push_back(pc);
            existing = pc;
            cout << "Chat created with " << target << ".\n";
        }
        inChatMenu(existing);
    }

    void createGroup() {
        cout << "\n--- Create Group ---\n";
        string name;
        cout << "Group name: "; cin >> name;
        int n;
        cout << "How many additional members (besides you)? "; cin >> n;
        vector<string> members = {getCurrentUsername()};
        for (int i = 0; i < n; ++i) {
            string m;
            cout << "  Member " << i + 1 << " username: "; cin >> m;
            if (findUserIndex(m) == -1) { cout << "User " << m << " not found, skipping.\n"; continue; }
            if (find(members.begin(), members.end(), m) == members.end())
                members.push_back(m);
        }
        auto* gc = new GroupChat(members, name, getCurrentUsername());
        chats.push_back(gc);
        cout << "Group \"" << name << "\" created with " << members.size() << " member(s).\n";
        inChatMenu(gc);
    }

    void viewChats() {
        auto myChats = getUserChats();
        if (myChats.empty()) { cout << "No chats yet.\n"; return; }
        cout << "\n--- Your Chats ---\n";
        for (size_t i = 0; i < myChats.size(); ++i)
            cout << "  " << i + 1 << ". " << myChats[i]->getChatName() << "\n";
        cout << "Select chat (0 to cancel): ";
        int sel; cin >> sel;
        if (sel < 1 || sel > (int)myChats.size()) return;
        inChatMenu(myChats[sel - 1]);
    }

    void updateProfile() {
        cout << "\n--- Profile ---\n";
        cout << "1. Change status\n2. Change phone\n3. Change password\n4. Back\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        if (ch == 1) {
            cout << "New status: "; string s; getline(cin, s);
            users[currentUserIndex].setStatus(s);
            cout << "Status updated.\n";
        } else if (ch == 2) {
            cout << "New phone: "; string p; getline(cin, p);
            users[currentUserIndex].setPhoneNumber(p);
            cout << "Phone updated.\n";
        } else if (ch == 3) {
            cout << "New password: "; string pwd; getline(cin, pwd);
            users[currentUserIndex].changePassword(pwd);
            cout << "Password changed.\n";
        }
    }

    void logout() {
        users[currentUserIndex].updateLastSeen();
        cout << "Goodbye, " << getCurrentUsername() << "!\n";
        currentUserIndex = -1;
    }

    void run() {
        cout << "=============================\n";
        cout << "   WhatsApp CLI Simulator\n";
        cout << "=============================\n";
        while (true) {
            if (!isLoggedIn()) {
                cout << "\n1. Login\n2. Sign Up\n3. Exit\nChoice: ";
                int choice; cin >> choice;
                if      (choice == 1) login();
                else if (choice == 2) signUp();
                else if (choice == 3) { cout << "Goodbye!\n"; break; }

            } else {
                cout << "\n1. Start Private Chat\n2. Create Group\n"
                        "3. View Chats\n4. Profile\n5. Logout\nChoice: ";
                int choice; cin >> choice;
                if      (choice == 1) startPrivateChat();
                else if (choice == 2) createGroup();
                else if (choice == 3) viewChats();
                else if (choice == 4) updateProfile();
                else if (choice == 5) logout();
            }
        }
    }
};

// ========================
//          MAIN
// ========================
int main() {
    WhatsApp whatsapp;
    whatsapp.run();
    return 0;
}
