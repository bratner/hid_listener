/* HID Listen, http://www.pjrc.com/teensy/hid_listen.html
 * Listens (and prints) all communication received from a USB HID device,
 * which is useful for view debug messages from the Teensy USB Board.
 * Copyright 2008, PJRC.COM, LLC
 *
 * You may redistribute this program and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rawhid.h"


static void delay_ms(unsigned int msec);
static int inpacket = 0;
/*
    b1 - packet start 
    00 - finishes the packet.
*/
void process_multiplo(unsigned char * buf, int * size){
    unsigned char tmp[64];
    int i,j;
//    printf("mult: processing %d bytes.\n", *size);
    
    j = 0;
    for(i=0; i< *size; i++){
       if(inpacket){ 
  //         printf("in packet index %d\n", i);
           if(buf[i] == 0x0){
               inpacket = 0;
               continue;
           }
           if(buf[i] == 0xB1){
               inpacket = 1;
               continue;              
           }
           tmp[j] = buf[i];
           //printf("%c", buf[i]);
           j++;
           
             
       } else if (buf[i] == 0xB1) {
    //       printf("start packet at index %d\n", i);
           inpacket = 1;
           continue;
       }
       //if(buf[i] == 0xB1 && buf[i+1]!=0 ){ //start packet
       //   printf("0x%x ",buf[i]);
       
       //}
    }
    //  printf("\n");
    *size = j;     
    memcpy(buf,tmp,j);
}
int main(void)
{
	char buf[64], *in, *out;
	rawhid_t *hid;
	int num, count;

	printf("Waiting for device:");
	fflush(stdout);
	while (1) {
		hid = rawhid_open_only1(0, 0, 0xFF31, 0x0074);
		if (hid == NULL) {
			printf(".");
			fflush(stdout);
			delay_ms(1000);
			continue;
		}
		printf("\nListening:\n");
		while (1) {
			num = rawhid_read(hid, buf, sizeof(buf), 200);
			if (num < 0) break;
			if (num == 0) continue;
			in = out = buf;
			/* filter for \0's - we disable it for multiplo
                          for (count=0; count<num; count++) {
				if (*in) {
					*out++ = *in;
				}
				in++;
			} */
			count = num; //out - buf;
                        process_multiplo(buf,&count);
			//printf("read %d bytes, %d actual\n", num, count);
			if (count) {
				num = fwrite(buf, 1, count, stdout);
				fflush(stdout);
			}
		}
		rawhid_close(hid);
		printf("\nDevice disconnected.\nWaiting for new device:");
	}
	return 0;
}






#if (defined(WIN32) || defined(WINDOWS) || defined(__WINDOWS__)) 
#include <windows.h>
static void delay_ms(unsigned int msec)
{
	Sleep(msec);
}
#else
#include <unistd.h>
static void delay_ms(unsigned int msec)
{
	usleep(msec * 1000);
}
#endif
