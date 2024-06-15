#include <sing.h>
void sing(){
	if(Music_Switch){
		if(flag==1){
			h=song[i];
			jiep=song[i+1];
			i=i+2;
			if(i>231)
				i=0;
			hz=G_fre[h];
			TH1=(hz)/256;
			TL1=(hz)%256;
			flag=0;
			TR2=1;
			TR1=1;
		}
	}
}

void Sing_Init(){
	i = 0;
	TR1 = 0;
	TR2 = 0;
}

void Sing_Off(){
	TR1 = 0;
	TR2 = 0;
}

void Sing_On(){
	TR1 = 1;
	TR2 = 1;
}