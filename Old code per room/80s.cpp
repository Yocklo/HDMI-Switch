//80s

#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pigpio.h>
#include "irslinger.h"

#define IR_PIN 18 // GPIO pin connected to the IR LED
#define FREQ 38000 // 38 kHz frequency for IR modulation
#define PULSE_HIGH 562 // 562 µs HIGH pulse
#define BIT_1_LOW 1687 // LOW duration for logical 1
#define BIT_0_LOW 562 // LOW duration for logical 0

void sendPulse(int gpio, int freq, int duration) {
    gpioHardwarePWM(gpio, freq, 500000); // 50% duty cycle
    gpioDelay(duration);
    gpioHardwarePWM(gpio, 0, 0); // Turn off PWM
    gpioDelay(PULSE_HIGH); // Delay for low
}

void sendBit(int gpio, bool bit) {
    sendPulse(gpio, FREQ, PULSE_HIGH); // Always start with a HIGH pulse
    if (bit) {
        gpioDelay(BIT_1_LOW);
    } else {
        gpioDelay(BIT_0_LOW);
    }
}

void sendNECCommand(int gpio, uint8_t address, uint8_t command) {
    // NEC Header
    sendPulse(gpio, FREQ, 9000); // 9 ms HIGH
    gpioDelay(4500); // 4.5 ms LOW

    // Encode and send data (32 bits)
    uint8_t address_inv = ~address;
    uint8_t command_inv = ~command;
    uint32_t frame = (address << 24) | (address_inv << 16) | (command << 8) | command_inv;

    for (int i = 31; i >= 0; i--) {
        sendBit(gpio, (frame >> i) & 1); // Send each bit
    }

    // End pulse
    sendPulse(gpio, FREQ, PULSE_HIGH);
}

void tcpConnectAndListen(const std::string& address, int port) {
    int sock;
    struct sockaddr_in serverAddr;
    uint32_t outPin = 18;            // The Broadcom pin number the signal will be sent on
    int frequency = 38000;           // The frequency of the IR signal in Hz
    double dutyCycle = 0.5;          // The duty cycle of the IR signal. 0.5 means for every cycle,
                                   // the LED will turn on for half the cycle time, and off the other half
    int leadingPulseDuration = 9000; // The duration of the beginning pulse in microseconds
    int leadingGapDuration = 4500;   // The duration of the gap in microseconds after the leading pulse
    int onePulse = 562;              // The duration of a pulse in microseconds when sending a logical 1
    int zeroPulse = 562;             // The duration of a pulse in microseconds when sending a logical 0
    int oneGap = 1688;               // The duration of the gap in microseconds when sending a logical 1
    int zeroGap = 562;               // The duration of the gap in microseconds when sending a logical 0
    int sendTrailingPulse = 1;       // 1 = Send a trailing pulse with duration equal to "onePulse"
                                   // 0 = Don't send a trailing pulse
    int result;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error." << std::endl;
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    // Convert address from text to binary form
    if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported." << std::endl;
        return;
    }
    
    back:
    // Connect to server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        goto back;
    }

    std::cout << "Connected to server at " << address << ":" << port << std::endl;


    char buf[10] = {'s', 'u', 'b', 's', 'c', 'r', 'i', 'b', 'e', '\n'};
    ssize_t bytesSent = write(sock, buf, strlen(buf));
    if (bytesSent < 0) {
        std::cerr << "Failed to send subscribe command." << std::endl;
        close(sock);
        return;
    } else {
        std::cout << "Subscribe command sent successfully." << std::endl;
    }

    // Buffer to store received data
    char buffer[1024] = {0};
    int valread;
    fd_set readfds;
    struct timeval tv;
    // Continuously read data from server
    while (true) {
        // Reset the file descriptor set
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        // Set timeout for select
        tv.tv_sec = 5; // 5 seconds timeout
        tv.tv_usec = 0;

        // Monitor the socket for readability
        int activity = select(sock + 1, &readfds, nullptr, nullptr, &tv);

        if (activity < 0 && errno != EINTR) {
            std::cerr << "Select error." << std::endl;
            break;
        }

        // If socket is readable, read data
        if (FD_ISSET(sock, &readfds)) {
            valread = read(sock, buffer, sizeof(buffer) - 1); // Read up to the buffer size

            if (valread < 0) {
                std::cerr << "Read error." << std::endl;
                break;
            } else if (valread == 0) {
                std::cerr << "Server closed connection." << std::endl;
                break;
            }

            // Null-terminate and process the buffer
            buffer[valread] = '\0';
            std::string receivedData(buffer);
            if (receivedData.find("event:json") == std::string::npos)
            {
                std::cout << "Received data: " << receivedData << std::endl;
                memset(buffer, 0, sizeof(buffer));
                
                //Karaoke
                if ((receivedData.find("chill:karaoke") != std::string::npos) || (receivedData.find("quiz:karaoke") != std::string::npos) || (receivedData.find("projection:karaoke") != std::string::npos) || (receivedData.find("club:karaoke") != std::string::npos)) 
                {
                    printf("Active karaoke\n");
                        result = irSling(
                        outPin,
                        frequency,
                        dutyCycle,
                        leadingPulseDuration,
                        leadingGapDuration,
                        onePulse,
                        zeroPulse,
                        oneGap,
                        zeroGap,
                        sendTrailingPulse,
                        "01000000101111110111101110000100");
  

                }

                //quiz
                if ((receivedData.find("chill:quiz") != std::string::npos) || (receivedData.find("karaoke:quiz") != std::string::npos) || (receivedData.find("projection:quiz") != std::string::npos) || (receivedData.find("club:quiz") != std::string::npos)) 
                {
                    printf("Active quiz\n");
                        result = irSling(
                        outPin,
                        frequency,
                        dutyCycle,
                        leadingPulseDuration,
                        leadingGapDuration,
                        onePulse,
                        zeroPulse,
                        oneGap,
                        zeroGap,
                        sendTrailingPulse,
                        "01000000101111110101101110100100");
  
                }

                //Blind test
                if ((receivedData.find("chill:projection") != std::string::npos) || (receivedData.find("karaoke:projection") != std::string::npos) || (receivedData.find("quiz:projection") != std::string::npos) || (receivedData.find("club:projection") != std::string::npos)) 
                {
                    printf("Active Blind test\n");
                        result = irSling(
                        outPin,
                        frequency,
                        dutyCycle,
                        leadingPulseDuration,
                        leadingGapDuration,
                        onePulse,
                        zeroPulse,
                        oneGap,
                        zeroGap,
                        sendTrailingPulse,
                        "01000000101111111101101100100100");
  
                }

                //Club
                if ((receivedData.find("chill:club") != std::string::npos) || (receivedData.find("karaoke:club") != std::string::npos) || (receivedData.find("quiz:club") != std::string::npos) || (receivedData.find("projection:club") != std::string::npos)) 
                {
                    printf("Active Club\n");
                }

            } else {
            // No data received; select timed out
            memset(buffer, 0, sizeof(buffer));
        }
    }
    }
    // Close socket after exit
    close(sock);
}


int main() {
    std::string address = "192.168.1.113";  //Adresse du raspberry de controle de la salle correspondante (Checker VNC)
    int port = 4444;

    // Initialize pigpio for GPIO control
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio." << std::endl;
        return -1;
    }

    // Set IR_PIN as output
    gpioSetMode(IR_PIN, PI_OUTPUT);

    tcpConnectAndListen(address, port);
    // Clean up pigpio library
    gpioTerminate();
    return 0;
}
