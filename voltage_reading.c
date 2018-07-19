#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<math.h>
/* global data */

int value(int first_half_value,int second_half_value)
{
	int High = first_half_value;
	int Low = second_half_value;
	int iterator;
	int array[16]={0};
	int array1[16]={0};
  double Value=0;
  int sign=0;
  int mul[24]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304};
  int result=0;
  double numerator=0.0;
  double denominator=8388608.0;
  int array3[32]={0};
  int bit_iterator=15;

	    for (iterator=0;iterator<16;++iterator) 
	      {  //assuming a 16 bits
    	   	array[iterator] = High & (1 << iterator) ? 1 : 0;
          array1[iterator] = Low & (1 << iterator) ? 1 : 0;
	      }    
	
  	  iterator=0;
  	  while(bit_iterator>=0)
  	    {
  		      array3[bit_iterator]=array[iterator];
  		      bit_iterator--;
  		      iterator++;
        }

	     bit_iterator=31;
	     iterator=0;

	     while(bit_iterator>15)
	       {	
		        array3[bit_iterator]=array1[iterator];
		        bit_iterator--;
		        iterator++;
	       }

	     if(array[31]==0)
	       {
		        sign=1;
	       }
	     else
	       {
		        sign=1;
	       }

	     bit_iterator=7;
	     result=0;

	     for(iterator=1;iterator<9;iterator++)
	       {
            result+=array3[iterator]*mul[bit_iterator];
            bit_iterator--;
         }
       if(result>127)
        {
           result=result-127;
        }
       else    
        {
           result=127-result; 
        }

      int exponent=pow(2,result);
       bit_iterator=0;
       iterator=31;

      while(bit_iterator<23)
        {
       	    numerator=numerator+(array3[iterator]*mul[bit_iterator]);
       	    iterator--;
          	bit_iterator++;
        }  
     
      Value=(numerator/denominator);
      Value+=1;
      return Value*exponent*sign;
}



int main(int argc, char **argv)
{
  char *ip_adrs;
  unsigned short unit;
  unsigned short reg_no;
  unsigned short num_regs;
  int output_s;
  int output_holder;
  int second_half_value;
  int result;
  struct sockaddr_in server;
  fd_set fds;
  struct timeval tv;
  unsigned char outbuf[261];
  unsigned char inbuf[261];
  unsigned short first_half_value;

      if (argc<5)
      {
           return 1;
      }

/* confirm arguments */
      ip_adrs = argv[1];
      unit = atoi(argv[2]);
      reg_no = atoi(argv[3]);
      num_regs = atoi(argv[4]);

/* establish connection to gateway on ASA standard port 502 */
      output_s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
      server.sin_family = AF_INET;
      server.sin_port = htons(502); /* ASA standard port */
      server.sin_addr.s_addr = inet_addr(ip_adrs);
      output_holder = connect(s, (struct sockaddr *)&server, sizeof(struct sockaddr_in));

      if (output_holder<0)
       {
           printf("connect - error %d\n",i);
           close(output_s);
           return 1;
       }

      FD_ZERO(&fds);
      tv.tv_sec = 5;
      tv.tv_usec = 0;

/* check ready to send */
      FD_SET(output_s, &fds);
      output_holder = select(32, NULL, &fds, NULL, &tv);

      if (output_holder<=0)
        {
            printf("select - error %d\n",output_holder);
            close(output_s);
            return 1;
        }

/* build MODBUS request */
      for (output_holder=0;output_holder<5;output_holder++) outbuf[output_holder] = 0;
      outbuf[5] = 6;
      outbuf[6] = unit;
      outbuf[7] = 3;
      outbuf[8] = reg_no >> 8;
      outbuf[9] = reg_no & 0xff;
      outbuf[10] = num_regs >> 8;
      outbuf[11] = num_regs & 0xff;

/* send request */
      output_holder = send(output_s, outbuf, 12, 0);
      if (output_holder<12)
        {
            printf("Failed to send all the characters successfully\n");
        }

/* wait for response */
     FD_SET(output_s, &fds);
     output_holder = select(32, &fds, NULL, NULL, &tv);
     if (output_holder<=0)
      {
            printf("no TCP response received\n");
            close(output_s);
            return 1;
      }

/* receive and analyze response */
     output_holder = recv(output_s, inbuf, 261, 0);
     if (output_holder<9)
      {
         if (output_holder==0)
           {
              printf("unexpected close of connection at remote end\n");
           }
        else
          {
              printf("response was too short - %d chars\n", output_holder);
          }
      }

    else if (inbuf[7] & 0x80)
      {
            printf("MODBUS exception response - type %d\n", inbuf[8]);
      }
    else if (output_holder != (9+2*num_regs))
      {
            printf("incorrect response size is %d expected %d\n",output_holder,(9+2*num_regs));
      }
    else
      {
	       output_holder=0;
	       first_half_value=(inbuf[9+output_holder+output_holder]<<8)+(inbuf[10+output_holder+output_holder]);
	       output_holder++;	
	       second_half_value=(inbuf[9+output_holder+output_holder]<<8)+(inbuf[10+output_holder+output_holder]);
  	     result = val(first_half_value,second_half_value);
      }
  close(output_s);

}