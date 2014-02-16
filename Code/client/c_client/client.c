#include <stdio.h>
#include <stdlib.h>

#include "box_api.h"
#include "client.h"

int main()
{
   int s;
   char hostname[BUFFSIZE];
   int port_number;

   printf("WHICH HOST COMPUTER ?:");
   scanf("%s",hostname);
   printf("WHICH port number ?:");
   scanf("%d", &port_number);

   box_connect(hostname,port_number);

   do_things(s);

   box_disconnect();

   return EXIT_SUCCESS;
}

void do_things() {
int temp, s;
        float values[2] =  {0.0,0.0};
		setPanTilt(2.7F,4.3F,values);
        printf("value 0 = %f value 1 = %f\n", values[0], values[1]);

        s = getRelays();
        printf("current relay settings = %d\n", s);
        if (setRelays(109)) printf("relays set o.k.\n");
                        else printf("relays NOT set\n");
		temp = 4;
                if (setRelayN(temp)) printf("relay %d set o.k.\n", temp);
                        else printf("relay %d NOT set\n", temp);
                s = getRelays();
                printf("current relay settings = %d\n", s);
		temp = 3;
                s = getRelayN(temp);
                printf("current relay %d settings = %d\n", temp, s);
                if (resetRelayN(5)) printf("relay N reset o.k.\n");
                        else printf("relay N NOT set\n");
		getInclinometer(values);
                printf("side-side= %f fore-aft = %f\n", values[0], values[1]);
		// box_disconnect();
		// box_shutdown();
}

