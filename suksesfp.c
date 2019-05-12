
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>

char filename[200]="/home/zaky/Final_Project/crontab.data";

void *MembacaCrontab(void *arg)
{
	char nampung[200];
	sprintf(nampung, "%s", (char*)arg);

	char *memisah = strtok(nampung, " ");	 	//strtok adalah membagi string menjadi beberapa bagian yang dibatasi oleh karakter yang telah ditentukan
	int bulan=-1, hari=-1, tgl=-1, jam=-1, menit=-1;
	int flag_menit=1, flag_jam=1, flag_tgl=1, flag_bulan=1, flag_hari=1;

	if(strcmp(memisah, "*")!=0)
	{
		menit=atoi(memisah);		//lalu mengubah string, angka menjadi sebuah  bilangan numerik integer
		if((strcmp(memisah, "0")!=0) && menit==0 || (menit>59 || menit<0)) return NULL;
	}

	memisah = strtok(NULL, " "); 
	if(strcmp(memisah, "*")!=0)
	{
		jam=atoi(memisah);
		if((strcmp(memisah, "0")!=0) && jam==0 || (jam>23 || jam<0)) return NULL;
	}

	memisah = strtok(NULL, " "); 
	if(strcmp(memisah, "*")!=0)
	{
		tgl=atoi(memisah);
		if(tgl==0 && (strcmp(memisah, "0")!=0)) return NULL;
	}

	memisah = strtok(NULL, " "); 
	if(strcmp(memisah, "*")!=0)
	{
		bulan=atoi(memisah);
		if((strcmp(memisah, "0")!=0) && bulan==0 || (bulan>12 || bulan<1)) return NULL;
	}

	memisah = strtok(NULL, " "); 
	if(strcmp(memisah, "*")!=0)
	{
		hari=atoi(memisah);
		if((strcmp(memisah, "0")!=0) && hari==0 || (hari>6 || hari<0)) return NULL;
	}

	char crontab[200];
	char *hasil[200];
	char *dirpath;
	memisah = strtok(NULL, " ");
	sprintf(crontab, "%s", memisah);
	char crontab_path[200];
	int h=0;
	sprintf(crontab_path, "%s", crontab);
	dirpath=strrchr(crontab_path, '/');

	for(int i=0; i<strlen(dirpath); i++)
	{
		dirpath[i]=dirpath[i+1];
	}

	hasil[h]=dirpath;
	h+=1;

	while (memisah != NULL)
	{
		if(h!=1)
		{
			hasil[h-1]=memisah;
		}
		h+=1;
		memisah = strtok(NULL, " ");
	}

	h-=1;

	char menjalankan[500];
	sprintf(menjalankan, "%s %s\n", hasil[0], hasil[1]);
	for(int j=1; j<h; j++)
	{
		sprintf(menjalankan, "%s %s", menjalankan, hasil[j]);
	}

	while(1)
	{
		time_t now;
		time(&now);
		struct tm *local = localtime(&now);

		if(menit != -1)
		{
			if(menit==local->tm_min) flag_menit=-1;
			else flag_menit=0;
		}

		if(jam != -1)
		{
			if(jam==local->tm_hour) flag_jam=-1;
			else flag_jam=0;
		}

		if(tgl != -1)
		{
			if(tgl==local->tm_mday) flag_tgl=-1;
			else flag_tgl=0;
		}

		if(bulan != -1)
		{
			if(bulan==local->tm_mon+1) flag_bulan=-1;
			else flag_bulan=0;
		}

		if(hari != -1)
		{
			if(hari==local->tm_wday) flag_hari=-1;
			else flag_hari=0;
		}
		printf("%s\n",menjalankan);
		if(flag_bulan!=0 && flag_hari!=0 && flag_tgl!=0 && flag_jam!=0 && flag_menit!=0 && local->tm_sec==0)
		{
			system(menjalankan);
			sleep(3);
		}
	}
	return NULL;
}

pthread_t tid[200];

void make_t(char *file, int n)
{
	printf("%s\n",file);
	pthread_create(&(tid[n]), NULL, &MembacaCrontab, file);
}

void membaca_file()
{
	FILE *filenya;
	char f;
	filenya = fopen(filename, "r");
	f = fgetc(filenya);
	char cost_time[200];
	int number=0;
	while (f != EOF)
	{
		cost_time[number]=f;
		number+=1;
		f = fgetc(filenya);
	}
	fclose(filenya);

	cost_time[number]='\0';
	char *save[200];
	int next=0;
	char *memisah = strtok(cost_time, "\n");
	int size = strlen(cost_time);

	while(memisah != NULL)
	{
		printf("%s\n",memisah);
		save[next]=strdup(memisah);
	}
	
	for (int i=0; i<next; i++)
	{
		printf("%s\n",save[i]);
		make_t(save[i], i);
	}
}


int main()
{
	struct stat sb;
	long int get_t;
	stat(filename, &sb);
	get_t=sb.st_mtime;

	membaca_file();
	
	while(1)
	{

		stat(filename, &sb);
    
		if(get_t<sb.st_mtime)
		{
			for(int n=0; n<0; n++)
			{
				 pthread_cancel(tid[n]);
			}
			
			membaca_file();
		}
		sleep(3);
	}
}
