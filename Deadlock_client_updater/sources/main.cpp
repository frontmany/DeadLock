#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

bool waitForProcessToExit(DWORD pid, DWORD timeoutMs = INFINITE) {
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process with PID " << pid << std::endl;
        return false;
    }

    DWORD waitResult = WaitForSingleObject(hProcess, timeoutMs);
    CloseHandle(hProcess);

    if (waitResult == WAIT_OBJECT_0) {
        std::cout << "Process " << pid << " has exited." << std::endl;
        return true;
    }
    else if (waitResult == WAIT_TIMEOUT) {
        std::cerr << "Timeout waiting for process " << pid << std::endl;
        return false;
    }
    else {
        std::cerr << "Wait failed for process " << pid << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: deadlock_updater.exe <PID_of_main_app>" << std::endl;
        return 1;
    }

    DWORD mainAppPid = 0;
    try {
        mainAppPid = std::stoul(argv[1]);
    }
    catch (...) {
        std::cerr << "Invalid PID argument." << std::endl;
        return 1;
    }

    constexpr DWORD waitTimeoutMs = 30000;
    if (!waitForProcessToExit(mainAppPid, waitTimeoutMs)) {
        std::cerr << "Main application did not exit within timeout." << std::endl;
        return 1;
    }

    const char* folderName = "updaterTemporary";
    fs::path versionsFilePath = fs::path(folderName) / "versions.txt";

    if (!fs::exists(versionsFilePath)) {
        std::cerr << "File " << versionsFilePath << " not found." << std::endl;
        return 1;
    }

    std::ifstream versionsFile(versionsFilePath);
    if (!versionsFile.is_open()) {
        std::cerr << "Failed to open file " << versionsFilePath << std::endl;
        return 1;
    }

    std::string line;
    std::string lastLine;
    while (std::getline(versionsFile, line)) {
        if (!line.empty()) {
            lastLine = line;
        }
    }
    versionsFile.close();

    if (lastLine.empty()) {
        std::cerr << "File " << versionsFilePath << " is empty." << std::endl;
        return 1;
    }

    std::string oldFileName = lastLine + ".exe";
    const std::string newFileName = lastLine + ".exe";

    try {
        fs::path sourcePath = fs::path(folderName) / oldFileName;
        fs::path destPath = fs::current_path() / newFileName;

        if (!fs::exists(sourcePath)) {
            std::cerr << "File " << sourcePath << " not found." << std::endl;
            return 1;
        }

        fs::copy_file(sourcePath, destPath, fs::copy_options::overwrite_existing);
        std::cout << "The file has been successfully replaced: " << destPath << std::endl;
        if (fs::exists(sourcePath)) {
            try {
                fs::remove(sourcePath);
                std::cout << "Deleted old update file: " << sourcePath << std::endl;
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Failed to delete old update file: " << e.what() << std::endl;
            }
        }

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };
        BOOL success = CreateProcessW(
            destPath.wstring().c_str(),
            NULL,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        );

        if (success) {
            std::cout << "Launched updated application." << std::endl;
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else {
            std::cerr << "Failed to launch updated application. Error: " << GetLastError() << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "File system error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
