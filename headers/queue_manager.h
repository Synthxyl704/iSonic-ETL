#ifndef HEADERS_QUEUE_MANAGER_H
#define HEADERS_QUEUE_MANAGER_H
//help

#include <string>
#include <vector>
#include <fstream>

using namespace std;

struct QueueItem {          // should i have put this in a class?
    std::string url;        // nvm too late now, the error resolved itself
    std::string format;
    std::string status;     // "pending", "downloading", "completed", "failed", "paused"
    int priority;
    std::string errorMsg;
    std::string title;
};

class DownloadQueue {
    // private:
        // std::vector<QueueItem> queue;
        // std::string queueFilePath;
        // int currentIndex;
        // bool isPaused;

        // this sucks more weewee than japanese corn stars 
    public:
        std::vector<QueueItem> queue;
        std::string queueFilePath;
        int currentIndex;
        bool isPaused;

        DownloadQueue(const std::string &queueFile = {"download_queue.txt"}); // a really bad idea 
        
        void addItem(const std::string &url, const std::string &format, int priority = 0); // im disappointed you cant put brace inits in here
        void loadFromFile(const std::string &filePath);
        void loadFromStdin();
        void saveQueue();
        void loadQueue();
        
        // BEATMANIA IIDX!!!!!!!!
        void displayQueue();
        void reorderItem(int fromIdx, int toIdx);
        void removeItem(int idx);
        void pauseQueue();
        void resumeQueue();
        
        bool hasNext();
        QueueItem getNext();

        // status, error
        void updateStatus(int idx, const std::string &status, const std::string &errorMsg = "");
        
        int size()   const  {return queue.size();} // :prayge:
        bool paused() const {return isPaused;}
};

void processBatchDownload(const std::string& format, const std::string& downloadPath);
void manageDownloadQueue();

#endif
