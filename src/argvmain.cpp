#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>
#include <sstream>
#include <cstdint>

#include "headers/audio_utilities.h"
#include "headers/dependency.h"
#include "headers/url_utilities.h"
#include "headers/file_manager.h"
#include "headers/song_manager.h"
#include "headers/metadata.h"
#include "headers/usage_helper.h"
#include "headers/queue_manager.h"
// #include "headers/audio_preview.h"

#define MAX_PRO_SEARCH_LIST_INT_SIZE 4

   #define RESET    "\033[0m"
   #define YELLOW  "\033[33m"

inline void showUserTheirOptions() {
    std::cout << "\n[!_LOG_REMEMBER_THE_INDEXES_!]: What do you wish to do with these indexes?" << "\n\n";
    std::cout << "[OPTIONS_LIST]" << "\n";
    std::cout << "┌   1. Remove a specific song" <<"\n";
    std::cout << "├   2. Remove multiple indexes" << "\n";
    std::cout << "├   3. Show metadata" << "\n";
    std::cout << "└   4. Transcode" << "\n";

    std::cout << "\n[FLAG_INT_PROMPT]: Enter option integer: " ;
}

using namespace std;

std::string execCommand(const std::string& cmd) {
    std::array<char, 4096> buffer;
    std::string result;

    using PCloseFn = int (*)(FILE*);
    std::unique_ptr<FILE, PCloseFn> pipe(popen(cmd.c_str(), "r"), pclose);

    if (!pipe) {
        throw std::runtime_error("[R-Error]: popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        parts.push_back(item);
    }

    return parts;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    string firstArg = argv[1];

    signed int commandType = (-1);
         if (firstArg == "-smlist")     { commandType = 0;  }
    else if (firstArg == "-rem")        { commandType = 1;  }
    else if (firstArg == "-remMul")     { commandType = 2;  }
    else if (firstArg == "-transcode")  { commandType = 3;  }
    else if (firstArg == "-metamsc")    { commandType = 4;  }
    else if (firstArg == "-help")       { commandType = 5;  }
    else if (firstArg == "-search")     { commandType = 6;  }
    else if (firstArg.find("LFI_") != std::string::npos)    
                                        { commandType = 7;  }
    else if (firstArg == "-prosearch")  { commandType = 8;  }
    else if (firstArg == "-queue")      { commandType = 9;  }
    else if (firstArg == "-batchfile")  { commandType = 10; }
    else if (firstArg == "-batchstdin") { commandType = 11; }
    else if (firstArg == "-preview")    { commandType = 12; }
    else if (firstArg == "-play")       { commandType = 13; }

    switch (commandType) {
        case 0: // -smlist
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);

            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            listDownloadedSongs(downloadPath);
            return EXIT_SUCCESS;
        }
        
        case 1: // -rem
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);
            
            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            removeSong(downloadPath);
            return EXIT_SUCCESS;
        }

        case 2: // -remMul
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);

            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            removeMultipleSongs(downloadPath);
            return EXIT_SUCCESS;
        }

        case 3: // -transcode
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);
            
            if (downloadPath.empty()) {
                std::cerr << "[FSYS_ACCESS_ERROR]: File access error" << std::endl;
                return EXIT_FAILURE;
            }

            transcodeSelectedFiles(downloadPath);
            return EXIT_SUCCESS;
        }
        
        case 4: // -metamsc
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);

            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            showSongMetadata(downloadPath);
            return EXIT_SUCCESS;
        }

        case 5: // -help
        {
            printUsage(argv[0]);
            return EXIT_SUCCESS;
        }

        case 6: // -search
        {
           string downloadPath = (argc >= 4) ? argv[3] : "downloads";
           searchForSong(downloadPath, static_cast<std::string>(argv[2]));

           return EXIT_SUCCESS;
        }

        case 7: // -LFI_t ... -LFI_[X]
        {
            string downloadPath = (argc >= 3) ? argv[2] : "downloads";

            bool detailed = (firstArg.find("t") != std::string::npos);
            listDirectoryContents(downloadPath, detailed);

            return EXIT_SUCCESS;
        }

        case 8: // -prosearch
        {
            std::cout << "[PROG_LOG]: Main junction command used: \n";
            std::cout << std::string(15, '=');
            std::cout << "\n\n";
            
            string downloadPath = (argc >= 4) ? argv[3] : "downloads";
            searchForSong(downloadPath, argv[2]);

            int32_t VOLITION_INT;

            showUserTheirOptions();

            std::cin >> VOLITION_INT;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (VOLITION_INT > MAX_PRO_SEARCH_LIST_INT_SIZE || VOLITION_INT < (MAX_PRO_SEARCH_LIST_INT_SIZE - 3)) {
                std::cerr << "\n[INPUT_LIST_INDEX_ERROR]: invalid integer in pro search listing options, retry recommended. \n";
                std::exit(EXIT_FAILURE);
            }

            switch (VOLITION_INT) {
                case 1: {
                    removeSong(downloadPath);
                    std::cout << "[LOG]: Completed, now exiting pro search.\n\n";
                    return EXIT_SUCCESS;
                }

                case 2: {
                    removeMultipleSongs(downloadPath);
                    std::cout << "[LOG]: Exiting pro search.\n\n";
                    return EXIT_SUCCESS;
                }

                case 3: {
                    showSongMetadata(downloadPath);
                    std::cout << "[LOG]: Now exiting pro search.\n\n";
                    return EXIT_SUCCESS;
                }

                case 4: {
                    transcodeSelectedFiles(downloadPath);
                    std::cout << "[LOG]: Exiting pro search.\n\n";
                    return EXIT_SUCCESS;
                }

                default: 
                    std::cerr << "[???]: Something went... terribly wrong... how..." << "\n\n";
                    std::exit(EXIT_FAILURE);
                    break;
            }
        }
        
        case 9: // -queue
        {
            std::cout << "\n[QUEUE_MANAGER]: Starting queue management interface\n\n";
            manageDownloadQueue();
            return EXIT_SUCCESS;
        }
        
        case 10: // -batchfile
        {
            if (argc < 3) {
                std::cerr << "[BATCH_ERROR]: File path required\n";
                std::cerr << "Usage: " << argv[0] << " -batchfile <urls_file> [format] [download_path]\n";
                return EXIT_FAILURE;
            }
            
            string urlsFile = argv[2];
            string format = (argc >= 4) ? argv[3] : "opus";
            string downloadPath = (argc >= 5) ? argv[4] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);
            
            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }
            
            std::cout << "\n[BATCH_MODE]: Loading URLs from file: " << urlsFile << "\n";
            
            DownloadQueue queue;
            queue.loadFromFile(urlsFile);
            queue.displayQueue();
            
            char confirm;
            std::cout << "\nStart batch download? (yY/nN): ";
            std::cin >> confirm;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if (confirm == 'y' || confirm == 'Y') {
                processBatchDownload(format, downloadPath);
            } else {
                std::cout << "[BATCH_CANCEL]: Batch download cancelled\n";
            }
            
            return EXIT_SUCCESS;
        }
        
        case 11: // -batchstdin
        {
            string format = (argc >= 3) ? argv[2] : "opus";
            string downloadPath = (argc >= 4) ? argv[3] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);
            
            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }
            
            DownloadQueue queue;
            queue.loadFromStdin();
            queue.displayQueue();
            
            char confirm;
            std::cout << "\nStart batch download? (yY/nN): ";
            std::cin >> confirm;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if (confirm == 'y' || confirm == 'Y') {
                processBatchDownload(format, downloadPath);
            } else {
                std::cout << "[BATCH_CANCEL]: Batch download cancelled\n";
            }
            
            return EXIT_SUCCESS;
        }
        
        case 12: // -preview
        {
            string downloadPath = (argc >= 3) ? argv[2] : "downloads";
            
            std::cout << "\n[PREVIEW_MODE]: Starting audio preview\n";
            std::cout << "[PREVIEW_INFO]: Detecting available audio players...\n\n";
            
            string player = detectAvailablePlayer();
            if (player.empty()) {
                std::cerr << "[PREVIEW_ERROR]: No audio player found!\n";
                std::cerr << "[PREVIEW_ERROR]: Please install one of: cmus, mpv, moc, mpg123\n";
                return EXIT_FAILURE;
            }
            
            std::cout << "[PREVIEW_DETECTED]: Using " << player << " for playback\n";
            
            previewDownloadedSongs(downloadPath);
            return EXIT_SUCCESS;
        }
        
        case 13: // -play
        {
            string downloadPath = (argc >= 3) ? argv[2] : "downloads";
            
            playSelectedSong(downloadPath);
            return EXIT_SUCCESS;
        }

        default:
        break;
    }

    if (argc < 3 || argc > 4) {
        std::cerr << "\n((argc < 3 || argc > 4) || INVALID COMMAND: " << argv[1] <<  ")\n\n";

        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const string url = argv[1];
    const string format = argv[2];
    const string customFolder = (argc == 4) ? argv[3] : "";

    if (url.length() > 0x200) {
        cout << "\n\n[URL_INT_ERROR]: too many characters in [URL], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (format.length() > 5) {
        cout << "\n\n[CODEC_INT_ERROR]: too many characters in [FORMAT], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (!isTheFormatSupported(format) || isTheFormatSupported(format) == false)  {
        cerr << "\n[CODEC_ERROR]: unsupported format [" << format << "]\n\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!checkDependenciesInstallation()) {
        cerr << "[Error]: dependencies not properly installed or not set properly in (path)\n";
        return EXIT_FAILURE;
    }

std::string metadataCmd = "yt-dlp --no-warnings --print \"%(title)s|%(artist,uploader,creator)s|%(duration)s\" \"" + url + "\"";
std::string metadataOutput = execCommand(metadataCmd);

std::istringstream metaStream(metadataOutput);
std::string metaLine;

std::getline(metaStream, metaLine);
std::vector<std::string> metaFields = split(metaLine, '|');

std::string title = "[UNKNOWN]";
std::string artist = "[UNKNOWN]";
std::string duration = "[UNKNOWN]";

if (metaFields.size() >= 3) {
    title = metaFields[0];
    artist = (!metaFields[1].empty()) ? metaFields[1] : "[ERROR_WHILE_FETCHING_MDT_FOR_ARTIST]";
    duration = metaFields[2];
} else {
    std::cerr << "[WARNING]: Could not extract complete metadata for URL.\n";
}

cout << "\n=== [TRACK METADATA] ===\n";
cout << "Title   : " << title << "\n";
cout << "Artist  : " << artist << "\n";
cout << "Duration: " << duration << " seconds\n";
cout << std::string(26, '=') << "\n";

    string downloadPath = createOrGetDownloadFolder(customFolder);
    if (downloadPath.empty()) {
        return EXIT_FAILURE;
    }

    isSongRedundant(downloadPath, title); 

    bool isPlaylist = isPlaylistURL(url);
    
    char actualCommand[0x800];
    if (isPlaylist) {
        snprintf(actualCommand, sizeof(actualCommand),
            "yt-dlp --extract-audio --audio-format %s --output \"%s/%%(playlist_index)s - %%(title)s.%%(ext)s\" \"%s\"",
            format.c_str(), downloadPath.c_str(), url.c_str());
        
        cout << "\n[PLAYLIST URL DETECTED]\n";
        cout << std::string(12, '=');
        cout << "Download URL: [" << url << "]\n";
        cout << "Audio format: [" << format << "]\n";
        cout << "Download folder: [" << downloadPath << "]\n";
        cout << "Files will be numbered by playlist order\n";
    } else {
        snprintf(actualCommand, sizeof(actualCommand),
            "yt-dlp --extract-audio --audio-format %s --output \"%s/%%(title)s.%%(ext)s\" \"%s\"",
            format.c_str(), downloadPath.c_str(), url.c_str()
        );
        
        cout << "\n[SINGLE TRACK MODE]\n";
        cout << std::string(12, '='); 
        cout << "Download URL: [" << url << "]\n";
        cout << "Audio format: [" << format << "]\n";
        cout << "Download folder: [" << downloadPath << "]\n";
    }

    cout << "\n[Running cmd]: [" << actualCommand << "]\n\n";

    int resultVal = system(actualCommand);
    if (resultVal == 0) {
        cout << "\n[SUCCESS]: Audio extraction completed in: " << downloadPath << "\n\n";
        
        // char previewChoice;
        // cout << "[PREVIEW_PROMPT]: Would you like to preview the downloaded audio? (yY/nN): ";
        // cin >> previewChoice;
        // cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        // if (previewChoice == 'y' || previewChoice == 'Y') {
        //     string player = detectAvailablePlayer();
        //     if (!player.empty()) {
        //         // Find the most recently downloaded file
        //         playSelectedSong(downloadPath);
        //     } else {
        //         cout << "[PREVIEW_INFO]: No audio player available. Install cmus, mpv, moc, or mpg123.\n";
        //     }
        // }
    } else {
        cout << "\n\n[Error]: Audio extraction failure | (exit code: " << resultVal << ")\n";
        cout << "Common issues:\n";
        cout << "- Invalid URL or private/restricted content | [DRM]\n";
        cout << "- Network connectivity issues\n";
        cout << "- Insufficient permissions for download folder\n\n";

        return 1;
    }

    return EXIT_SUCCESS;
}
