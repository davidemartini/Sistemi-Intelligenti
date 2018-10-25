#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double pow(double x,double y) {
	double k=x;
	int i;
	if(y>0) {
		for(i=1;i<y;i++) {
			k=k*x;
		}
		return k;
	}
	if(y<0) {
		y=y*-1;
		for(i=1;i<y;i++) {
			k=k*x;
		}
		return 1/k;
	}
	return 1;
}

//*****IMPORTANTE*****//
/*
dopo aver chiamato la funzione system su svm-train aprire il file output.txt, 
leggere l'intero contenuto, filtrarlo e salvarlo in un altro file.
dopo potrà essere richiamata un'altra iterazione del ciclo for, e quindi output sovrascritto.
il file di destinazione resterà aperto per tutto il tempo, il file output solo per il tempo necessario a leggerlo.

Il programma manda ad output solamente il comando che si darebbe da terminale.
*/
//*****IMPORTANTE*****//

int main() { //o 5 o 10 per k-fold cross validation
	FILE *fend,*fout,*ffinal;
	double k,i,j,z,d,a,maxpol,cbest,gbest,dbest,abest,perc,lim;
	double lowbd,uppbd,lowba,uppba,lowbc,lowbg,uppbc,uppbg,incrementc,incrementg,incrementd,incrementa;
	char command[100],cost[20],gamma[20],degree[20],alfa[20],vett[100],x,xvett[4];
	int res,head=0,flag=0,print=0,count,ciclo,first=0;
	fout=fopen("outpol.txt", "w");
	fclose(fout);
	ffinal=fopen("fout.txt", "w");
	fclose(ffinal);
	lowbc=-15;
	uppbc=16;
	lowbg=-16;
	uppbg=16;
	lowbd=2;
	uppbd=6;
	lowba=-6;
	uppba=6;
	incrementc=0;
	incrementg=0;
	incrementd=0;
	incrementa=0;
	for(ciclo=0;ciclo<2;ciclo++) {
		for(i=lowbc;i<uppbc;i++) { //esponente -c (la prima iterazione inizializza i file) 
			for(j=lowbg;j<uppbg;j++) {
				for(d=lowbd;d<uppbd;d++) {
					for(a=lowba;a<uppba;a++) {
						if(flag!=0) {
							if(incrementc!=0) {
								i=i+incrementc;
								k=i;
							}
							else {
								k=pow(2,i);
							}
							if(incrementg!=0) {
								j=j+incrementg;
								z=j;
							}
							else {
								z=pow(2,j);
							}
							if(incrementd!=0) {
								d=d+incrementd;
							}
							else {
								z=pow(2,j);
							}
							if(incrementa!=0) {
								a=a+incrementa;
							}
							else {
								z=pow(2,j);
							}
					   		strcpy(command,"./svm-train -s 0 -t 1 -c "); 
							snprintf(cost,10,"%f",k);
							strcat(command,cost);
							strcpy(vett," -g ");
							strcat(command,vett);
							snprintf(cost,10,"%f",z);
							strcat(command,cost);
							strcpy(vett," -d ");
							strcat(command,vett);
							snprintf(degree,10,"%f",d);
							strcat(command,degree);
							strcpy(vett," -r ");
							strcat(command,vett);
							snprintf(alfa,10,"%f",a);
							strcat(command,alfa);
							strcpy(vett," -v 10 ./svm_dataset/svm/spam_train_std.txt > outpol.txt\n");
							strcat(command,vett);
							system(command);
							printf("%s\n",command);
							strcpy(command,"");
							fout=fopen("outpol.txt","r");
							if(fout==NULL) {
								perror("Errore in apertura del file OUTPUT");
								exit(1);
							}
							fend=fopen("accuracypol.txt", "a");
							if(fend==NULL) {
								perror("Errore in apertura del file accuracypol");
								exit(1);
							}
							if(head==0) {
								fprintf(fend,"POL KERNEL\n");
								head++;
							}
							res=fscanf(fout,"%c",&x);
							while(res!=EOF) {
								//**Filtro file output
								if(x=='C') {
									for(count=0;count<27;count++) res=fscanf(fout,"%c",&x);
									count=0;
									while(count<5) {
										xvett[count]=x;
										fprintf(fend,"%c",x);
										res=fscanf(fout,"%c",&x);
										if(xvett[count]=='%') {
											xvett[count]='.';
											count++;
											xvett[count]='0';
											count=5;
										}
										count++;
									}
									//printf("%s\n%d\n",xvett,count);
									perc=atof(xvett);
									if(perc>maxpol) {
										maxpol=perc;
										cbest=k;
										gbest=z;
										dbest=d;
										abest=a;
										printf("c %f gamma %f degree %f alfa %f\n",cbest,gbest,dbest,abest);
									}
									fprintf(fend,"\nC=%f gamma=%f degree=%f alfa=%f\n",k,z,d,a);
								}
								//**Fine filtro
								//Inserire al posto del filtro fprintf(fend,"%c",x);						
								res=fscanf(fout,"%c",&x);
							}
							fclose(fout);
							fclose(fend);
				
						}
						else {
							printf("POL KERNEL\n");
							flag=1;
						}
						if(incrementa!=0) a--;	
						if(first==0) lowba=-5;	
						first=1;			
					}
					if(incrementd!=0) d--;
				}
				if(incrementg!=0) j--;
			}
			if(incrementc!=0) i--;
		}
		uppbc=cbest*2;
		lowbc=cbest/2;
		incrementc=(uppbc-lowbc)/20;
		uppbg=gbest*2;
		lowbg=gbest/2;
		incrementg=(uppbg-lowbg)/20;
		uppbd=dbest*2;
		lowbd=dbest/2;
		incrementd=(uppbd-lowbd)/20;
		uppba=abest*2;
		lowba=abest/2;
		incrementa=(uppba-lowba)/20;
	}
	system("rm outpol.txt");
	strcpy(command,"./svm-train -s 0 -t 1 -c "); 
	snprintf(cost,10,"%f",cbest);
	strcat(command,cost);
	strcpy(vett," ./svm_dataset/svm/spam_train_std.txt model.txt\n");
	strcat(command,vett);
	system(command);
	printf("%s\n",command);
	strcpy(command,"./svm-predict ./svm_dataset/svm/spam_test_std.txt model.txt output.txt > rbfoutput.txt\n");
	system(command);
	ffinal=fopen("fout.txt","r");
	if(ffinal==NULL) {
		perror("Errore in apertura del file FINALE");
		exit(1);
	}
	fend=fopen("accuracypol.txt", "a");
	if(fend==NULL) {
		perror("Errore in apertura del file accuracypol");
		exit(1);
	}			
	res=fscanf(ffinal,"%c",&x);
	fprintf(fend,"\n\nC=%f gamma=%f degree=%f alfa=%f\nPercentuale train=%f\n",cbest,gbest,dbest,abest,maxpol);
	while(res!=EOF) {
		fprintf(fend,"%c",x);
		res=fscanf(ffinal,"%c",&x);
	}
	fclose(fend);
	fclose(ffinal);
	system("rm output.txt");
	system("rm fout.txt");
	system("rm model.txt");
	printf("\n\nTERMINATO KERNEL POL\n\n");
	system("shutdown -h now");
}
