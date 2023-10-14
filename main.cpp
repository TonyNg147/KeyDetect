#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include "Key_defines.h"
enum KEY_EVENT
{
    PRESS = 0,
    RELEASE
};
int main()
{
    int fd = open("/dev/input/event1",O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr,"Unable to open input file");
        return 1;
    }
    printf("Come herere 2");
    int epollInstance = epoll_create1(EPOLL_CLOEXEC);
    if (epollInstance < 0)
    {
        close(fd);
        fprintf(stderr,"Unable to create epoll instance");
        return 1;
    }
    struct epoll_event epollEvent, epollEventReceived;
    epollEvent.events  = EPOLLIN;
    epollEvent.data.fd = fd;
    if (epoll_ctl(epollInstance, EPOLL_CTL_ADD, fd, &epollEvent) == -1)
    {
        close(fd);
        close(epollInstance);
        fprintf(stderr,"%s",strerror(errno));
        return 1;
    }
    printf("Come herere");
    int activeCount = 0;
    struct input_event ev[64];
    size_t eventSize = sizeof(struct input_event);
    int16_t type,code;
    int32_t value;
    while(1)
    {
        activeCount = epoll_wait(epollInstance, &epollEventReceived, 1, -1);
        if (activeCount > 0)
        {
            if (epollEventReceived.events & EPOLLIN)
            {
                int bytesRead = read(epollEventReceived.data.fd, ev, sizeof(ev));
                if (bytesRead < (int)sizeof(struct input_event))
                {
                    break;
                }
                for (size_t i=0;i<bytesRead/ eventSize; ++i)
                {
                    type = ev[i].type;
                    code = ev[i].code;
                    value = ev[i].value;
                    if (type == EV_KEY)
                    {
                        if (value == KEY_EVENT::PRESS)
                        {
                            printf("Key press %s\n",mapEvents[code]);
                        }
                        else if (value == KEY_EVENT::RELEASE)
                        {
                            printf("Key release %s\n",mapEvents[code]);
                        }
                    }
                }
            }
        }
    }
    
}
