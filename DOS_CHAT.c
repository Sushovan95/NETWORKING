#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define SIZE 200

#define SW_INTR 0x60 //software interrupt that will be invoked

unsigned char recv_pkt[SIZE];
unsigned char send_pkt[SIZE];
unsigned char buffer[SIZE];

unsigned char type[] = "\xff\xff"; //2 bytes type after S & D MAC
unsigned char type_c[2];

unsigned char src_mac[6]; //my mac i.e source mac
unsigned char dest_mac[] = "\x08\x00\x27\x34\x81\xfe"; //destination mac address

int handle,pktlen,i,buflen,flag;

void getmymac(); //get my MAC address
void pktheader(); //prepare the packet header before sending
void pktmsg(unsigned char*,int); //stuffing the payload to packet
void sendpkt(); //for sending the packet
int access_type(); //access to the software interrupt
int release_type(); //release the access from that interrupt

void interrupt receiver(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags) //whenever ethernet driver will have
unsigned short bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags; //any data then receiver() fnc will be called
{ 
 // cprintf("INTERUUPT\n");
  if(ax == 0)
    { 
        if(cx>SIZE)
        { 
           es = 0;
           di = 0;     
        }
           
             es = FP_SEG(recv_pkt);
             di = FP_OFF(recv_pkt);
             pktlen = cx;
     }
  
     else
      {
        for(i=6;i<12;i++)
          { 
            cprintf("%02x:",recv_pkt[i]); //print to the console
          }
        cprintf(">");

        for(i=14;i<200;i++)
          {
            if(recv_pkt[i] == 0x00)
                { 
                  break;
                }
                cprintf("%c",recv_pkt[i]);
          
        }
       putch('\r');                   
      }
}

int main()
{
        
        getmymac();
        pktheader();
        access_type();
 
       while(1)
        {
                fgets(buffer,SIZE,stdin);
                buflen  = strlen(buffer);
                if(strncmp(buffer,"exit",4) == 0)
                  {     
                        break;
                  }
                else
                  {     
                        pktmsg(buffer,buflen);
                        buflen = 0;
                        sendpkt();
                   }
        } 
                       
        release_type();
        printf("EXITED...");
        return 0;
}        

void pktheader()
{
        memcpy(send_pkt,dest_mac,6); //first 6 bytes dest MAC
        memcpy(send_pkt+6,src_mac,6); //next 6 bytes source MAC
        memcpy(send_pkt+12,type,2); // next 2 bytes type
}

void pktmsg(unsigned char* buff,int slen)
{
        memcpy(send_pkt+14,buff,slen);
        for(i = slen+14;i<SIZE;i++)
                send_pkt[i] = 0;
                    
} 

void getmymac()
{
        union REGS in,out;
        struct SREGS segreg;
        segread(&segreg);
        segreg.es = FP_SEG(src_mac);
        in.x.di = FP_OFF(src_mac); 
        in.x.cx = 6; //length of the mac
        in.h.ah = 6;
        int86x(SW_INTR,&in,&out,&segreg);
        printf("MAC ADDRESS \n");
       
        for(i=0;i<6;i++)
        {
           cprintf("%02x:",src_mac[i]);
        }
       
}

void sendpkt()
{
        union REGS in,out;
        struct SREGS segreg;
        segread(&segreg);        
        in.h.ah = 4; // for send_pkt() AH register value = 4
        segreg.ds = FP_SEG(send_pkt);
        in.x.si = FP_OFF(send_pkt); 
        in.x.cx = SIZE;
        int86x(SW_INTR,&in,&out,&segreg);
}       

int access_type()
{
        union REGS in,out;
        struct SREGS segreg;        
        in.h.al = 1; //class of interface
        in.h.dl = 0; //if_number i.e. interface no.
        in.x.bx = -1;
        in.x.cx = 0;
        in.h.ah = 2; //interrupt number
        segreg.es = FP_SEG(receiver);
        in.x.di = FP_OFF(receiver);                 
        type_c[0] = 0xff;
        type_c[1] = 0xff;
        segreg.ds = FP_SEG(type_c);
        in.x.si = FP_OFF(type_c);
        int86x(SW_INTR,&in,&out,&segreg);
        flag = out.x.cflag; //carry flag access type
        printf("CARRY FLAG ACCESS TYPE = %x\n",flag);
        handle = out.x.ax;
        return out.x.cflag;
}

int release_type()
{
        union REGS in,out;
        struct SREGS segreg;
        in.h.ah = 3; //interrupt number
        in.x.bx = handle;
        int86x(SW_INTR,&in,&out,&segreg);       
        cprintf("RELEASE TYPE = %d\n",out.x.cflag);        
        return 0;
}



