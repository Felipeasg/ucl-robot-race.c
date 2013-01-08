#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char buf[80];
struct sockaddr_in s_addr;
int i, sock;

void initSocket() {
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    fprintf(stderr, "Failed to create socket1\n");
    exit(1);
  }
  
  s_addr.sin_family = AF_INET;
  s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  s_addr.sin_port = htons(55443);
  
  if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
    fprintf(stderr, "Failed to create socket2\n");
    exit(1);
  }
}

void nextCommand() {
  write(sock, buf, strlen(buf)); memset(buf, 0, 80); read(sock, buf, 80);
}

void moveAtVoltage(int voltage1, int voltage2) {
  sprintf(buf, "M LR %i %i\n", voltage1, voltage2);
  printf("M LR %i %i\n", voltage1, voltage2);
  nextCommand();
}

void moveStraightAtVoltage(int voltage) {
  moveAtVoltage(voltage, voltage);
}

void turnAtVoltage(int voltage1, int voltage2) {
  sprintf(buf, "M LR %i %i\n", voltage1, voltage2);
  printf("M LR %i %i\n", voltage1, voltage2);
  nextCommand();
}

void turnOnSpotAtVoltage(int voltage) {
  turnAtVoltage(voltage, -voltage);
  nextCommand();
}

void stopMovement() {
  sprintf(buf, "M LR 0 0\n");
  printf("M LR 0 0\n");
  nextCommand();
}