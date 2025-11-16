#include "queue_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <limits>
#include <cstring>
// #include <stdio.h>
// #include <stdlib.h>

using namespace std;

DownloadQueue::DownloadQueue(const string &queueFile)
    : queueFilePath(queueFile), currentIndex(0), isPaused(false) {
    loadQueue();
}

void DownloadQueue::addItem(const string &url, const string &format, int32_t priority) {
    QueueItem item;

    item.url = url;
    item.format = format;
    item.status = "pending";
    item.priority = priority;
    item.errorMsg = "";
    
    queue.push_back(item);

    saveQueue();
}

void DownloadQueue::loadFromFile(const string &filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "[QUEUE_ERROR]: F-Path resolution error: " << filePath << "\n";
        return;
    }
    
    string line;
    int count {0};
    
    cout << "\n[QUEUE_LOAD_LOG]: Reading the URLs from file\n";
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        
        stringstream ss(line);
        string url, format;
        int priority {0};
        
        ss >> url; ss >> format;
        if (format.empty()) { format = "opus"; }
        ss >> priority;
        
        addItem(url, format, priority);
        count += 1;
    }
    
    file.close();
    cout << "[QUEUE_LOAD_LOG]: " << "[" << static_cast<int>(count) << "] items appened to the queue." << "\n";
}

void DownloadQueue::loadFromStdin() {
    cout << "\n[QUEUE_INPUT]: Enter URLs (one per line, format: URL <format> <priority> || default = <0, opus>),\n";
    cout << "[QUEUE_INPUT]: Press Ctrl+D (linux) or Ctrl+Z (w*ndows) when done\n";
    cout << string(50, '-') << "\n";
    
    string line;
    int count {0};
    
    while (getline(cin, line)) {
        if (line.empty()) {continue;}
        
        stringstream ss(line);
        string url, format;
        int priority {0};
        
        ss >> url; ss >> format;
        if (format.empty()) { format = "opus"; }
        ss >> priority;
        
        addItem(url, format, priority);
        count += 1;
        cout << "[" << count << "] Added: " << url << "\n";
    }
    
    cout << "\n[QUEUE_INPUT]: Added " << count << " items to queue\n";
}

void DownloadQueue::saveQueue() {
    ofstream file(queueFilePath);
    if (!file.is_open()) {
        cerr << "[QUEUE_ERROR]: Queue save error in directory: " << queueFilePath << "\n";
        return;
    }
    
    for (const auto &item : queue) {
        file << item.url << "|" 
             << item.format << "|" 
             << item.status << "|" 
             << item.priority << "|" 
             << item.errorMsg << "\n";
    }
    
    file.close();
}

void DownloadQueue::loadQueue() {
    ifstream file(queueFilePath);
    if (!file.is_open()) {
        return; 
    }
    
    queue.clear();
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        QueueItem item;
        stringstream ss(line);
        string token;
        
        std::getline(ss, item.url, '|');
        std::getline(ss, item.format, '|');
        std::getline(ss, item.status, '|');
        
        std::getline(ss, token, '|');
        item.priority = stoi(token);
        
        std::getline(ss, item.errorMsg, '|');
        
        queue.push_back(item);
    }
    
    file.close();
}

void DownloadQueue::displayQueue() {
    cout << "\n=== DOWNLOAD QUEUE ===\n";
    cout << string(80, '=') << "\n";
    
    if (queue.empty()) {
        cout << "[QUEUE_EMPTY_LOG]: No items found in queue\n\n";
        return;
    }
    
    cout << setw(4) << "Idx" 
         << setw(10) << "Status" 
         << setw(8) << "Priority" 
         << setw(8) << "Format" 
         << " URL\n";
    cout << string(80, '-') << "\n";
    
    for (size_t inx {0}; inx < queue.size(); inx += 1) {
        const auto &item = queue[inx];
        
        std::string statusColor;
             if (item.status == "completed")   { statusColor = "\033[32m"; }
        else if (item.status == "failed")      { statusColor = "\033[31m"; }
        else if (item.status == "downloading") { statusColor = "\033[33m"; }
        else if (item.status == "paused")      { statusColor = "\033[35m"; }
        else statusColor = "\033[0m";
        
        cout << setw(4) << (inx + 1) 
             << statusColor << setw(10) << item.status << "\033[0m"
             << setw(8) << item.priority 
             << setw(8) << item.format 
             << ((item.title.empty()) ? item.url.substr(0, 50)      // files dont have titles at runtime during download
                                      : item.title.substr(0, 50));
            // << item.title.substr(0,50);
        
        if (item.url.length() > 50) {
             cout << "..."; 
        }

        cout << "\n";
        
        if (!item.errorMsg.empty()) {
            cout << "    └─ Error: " << item.errorMsg << "\n";
        }
    }
    
    cout << string(80, '=') << "\n";
    cout << "Total count: " << queue.size() << " items";
    if (isPaused) { cout << " [PAUSED]"; }
    cout << "\n\n";
}

void DownloadQueue::reorderItem(int fromIdx, int toIdx) {
    if (fromIdx < 1 || fromIdx > static_cast<int>(queue.size()) ||
        toIdx < 1 || toIdx > static_cast<int>(queue.size())) {
        cerr << "[QUEUE_ERROR]: Invalid indices parsed, cannot redorder\n";
        return;
    }
    
    (fromIdx)--;
    (toIdx)--;
    
    QueueItem item {queue[fromIdx]};
    queue.erase(queue.begin() + fromIdx);
    queue.insert(queue.begin() + toIdx, item);
    
    saveQueue();
    cout << "[QUEUE_UPDATE]: Reorder successful\n";
}

void DownloadQueue::removeItem(int idx) {
    if (idx < 1 || idx > static_cast<int>(queue.size())) {
        cerr << "[QUEUE_ERROR]: Invalid index\n";
        return;
    }
    
    queue.erase(queue.begin() + (idx - 1));
    saveQueue();

    cout << "[QUEUE_UPDATE]: Item removed\n";
}

void DownloadQueue::pauseQueue() {
    isPaused = true;
    cout << "[QUEUE_STATUS]: Queue paused\n";
}

void DownloadQueue::resumeQueue() {
    isPaused = false;
    cout << "[QUEUE_STATUS]: Queue resumed\n";
}

bool DownloadQueue::hasNext() {
    if (isPaused) {
        return false;
    }
    
    for (size_t inx {static_cast<size_t>(currentIndex)}; inx < queue.size(); inx += 1) {
        if (queue[inx].status == "pending" || queue[inx].status == "failed") {
            currentIndex = static_cast<int>(inx);
            return true;
        }
    }
    
    return false;
}

QueueItem DownloadQueue::getNext() {
    return queue[currentIndex];
}

void DownloadQueue::updateStatus(int idx, const string &status, const string &errorMsg) {
    if (idx >= 0 && idx < static_cast<int>(queue.size())) {
        queue[idx].status = status;
        queue[idx].errorMsg = errorMsg;
        saveQueue();
    }
}

void processBatchDownload(const string &format, const string &downloadPath) {
    DownloadQueue queue;

    cout << "\n[BATCH_MODE]: Starting batch download processing\n";

    while (queue.hasNext()) {
        if (queue.paused()) {
            cout << "[BATCH_PAUSED]: Queue is paused. Resume to continue.\n";
            break;
        }

        QueueItem item = queue.getNext();

        cout << "\n" << string(60, '=') << "\n";
        cout << "[BATCH_PROGRESS]: Processing item\n";
        cout << "URL: " << item.url << "\n";
        cout << "Format: " << item.format << "\n";
        cout << string(60, '=') << "\n\n";

        /* currentIndex already points to the item we are about to process */
        queue.updateStatus(queue.currentIndex, "downloading");

        char cmd[0x800];
        snprintf(
            cmd, sizeof(cmd),
            "yt-dlp --extract-audio --audio-format %s --output \"%s/%%(title)s.%%(ext)s\" \"%s\"",
            item.format.c_str(), downloadPath.c_str(), item.url.c_str()
        );

        int result {system(cmd)};

        if (result == 0) {
            queue.updateStatus(queue.currentIndex, "completed");

            /* grab the clean title */
            char titleCmd[0x800];
            snprintf(titleCmd, sizeof(titleCmd),
                    "yt-dlp --no-download --print filename "
                    "--output \"%%(title)s\" \"%s\"", item.url.c_str()
            );

            FILE *(fp) = popen(titleCmd, "r");
            char buff[512] = {0}; 
            if (fp != NULL || fp) {
                if (fgets(buff, sizeof(buff), /* *(fp */ fp)) {
                    buff[strcspn(buff, "\r\n")] = 0; // #{<cstring>}
                    queue.queue[queue.currentIndex].title = buff;
                } pclose(fp);
            }

            cout << "\n[BATCH_SUCCESS]: Download completed\n";
        } else {
            queue.updateStatus(queue.currentIndex, "failed", "Download failed with exit code: " + to_string(result));
            cout << "\n[BATCH_FAILED]: Download failed (continuing with next item)\n";
        }
    }

    cout << "\n[BATCH_COMPLETE]: All queued downloads processed - \n";
    queue.displayQueue();
}

void manageDownloadQueue() {
    DownloadQueue queue;
    
    while (true) {
        cout << "\n=== QUEUE MANAGER BOARD ===\n";
        cout << "1. Display queue\n";
        cout << "2. Add URL from stdin\n";
        cout << "3. Load URLs from file\n";
        cout << "4. Reorder item\n";
        cout << "5. Remove item\n";
        cout << "6. Pause queue\n";
        cout << "7. Resume queue\n";
        cout << "8. Start batch download\n";
        cout << "9. Exit\n";
        cout << "\nChoice <int_only + enter>: ";
        
        int choice;
        if (!(cin >> choice)) {            /* non-numeric input -> leave */
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR]: Invalid choice\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                queue.displayQueue();
                break;
                
            case 2:
                queue.loadFromStdin();
                break;
                
            case 3: {
                cout << "Enter file path: ";
                string filePath;
                getline(cin, filePath);
                queue.loadFromFile(filePath);
                break;
            }
            
            case 4: {
                queue.displayQueue();
                cout << "Move item from index: ";

                int from, to;
                cin >> from;
                cout << "To index: ";
                cin >> to;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                queue.reorderItem(from, to);
                break;
            }
            
            case 5: {
                queue.displayQueue();
                cout << "Remove item at index: ";
                int idx;
                cin >> idx;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                queue.removeItem(idx);
                break;
            }
            
            case 6:
                queue.pauseQueue();
                break;
                
            case 7:
                queue.resumeQueue();
                break;
                
            case 8: {
                cout << "Enter download path (or optionally press <Enter> for default): ";
                std::string path;
                std::getline(std::cin, path);
                if (path.empty()) { path = "downloads"; }
                processBatchDownload("opus", path);
                break;
            }
            
            case 9:
                cout << "[QUEUE_MANAGER]: Exiting\n";
                return;
                
            default:
                cout << "[ERROR]: Invalid choice\n";
                return;
        }
    }
}
