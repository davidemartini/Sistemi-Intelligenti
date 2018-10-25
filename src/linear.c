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
	double k,i,maxlinear,cbest,perc,lim,lowb,uppb,increment=0;
	char command[100],cost[20],vett[100],x,xvett[4];
	int res,head=0,flag=0,print=0,count,ciclo=0;
	fout=fopen("outputlinear.txt", "w");
	fclose(fout);
	ffinal=fopen("fout.txt", "w");
	fclose(ffinal);
	lowb=-16;
	uppb=16;
	increment=0;
	for(ciclo=0;ciclo<2;ciclo++) {
		for(i=lowb;i<uppb;i++) { //esponente -c (la prima iterazione inizializza i file) 
			if(flag!=0) {
				if(increment!=0) {
					i=i+increment;
					k=i;
					i--;
				}
				else {
					k=pow(2,i);
				}
		   		strcpy(command,"./svm-train -s 0 -t 0 -c "); 
				snprintf(cost,10,"%f",k);
				strcat(command,cost);
				strcpy(vett," -v 10 ./svm_dataset/svm/spam_train_std.txt > outputlinear.txt\n");
				strcat(command,vett);
				system(command);
				printf("%s\n",command);
				strcpy(command,"");
				fout=fopen("outputlinear.txt","r");
				if(fout==NULL) {
					perror("Errore in apertura del file OUTPUT");
					exit(1);
				}
				fend=fopen("accuracy.txt", "a");
				if(fend==NULL) {
					perror("Errore in apertura del file ACCURACY");
					exit(1);
				}
				if(head==0) {
					fprintf(fend,"LINEAR KERNEL\n");
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
						if(perc>maxlinear) {
							maxlinear=perc;
							cbest=k;
							printf("%f\n",cbest);
						}
						fprintf(fend,"\nC=%f\n",k);
					}
					//**Fine filtro
					//Inserire al posto del filtro fprintf(fend,"%c",x);						
					res=fscanf(fout,"%c",&x);
				}
				fclose(fout);
				fclose(fend);
				
			}
			else {
				printf("LINEAR KERNEL\n");
				flag=1;
			}
		}
		uppb=cbest*2;
		lowb=cbest/2;
		increment=(uppb-lowb)/20;
	}
	system("rm outputlinear.txt");
	strcpy(command,"./svm-train -s 0 -t 0 -c "); 
	snprintf(cost,10,"%f",cbest);
	strcat(command,cost);
	strcpy(vett," ./svm_dataset/svm/spam_train_std.txt model.txt\n");
	strcat(command,vett);
	system(command);
	printf("%s\n",command);
	strcpy(command,"./svm-predict ./svm_dataset/svm/spam_test_std.txt model.txt output.txt > linearoutput.txt\n");
	system(command);
	ffinal=fopen("fout.txt","r");
	if(ffinal==NULL) {
		perror("Errore in apertura del file FINALE");
		exit(1);
	}
	fend=fopen("accuracy.txt", "a");
	if(fend==NULL) {
		perror("Errore in apertura del file ACCURACY");
		exit(1);
	}			
	res=fscanf(ffinal,"%c",&x);
	fprintf(fend,"\n\nC=%f\nPercentuale train=%f\n",cbest,maxlinear);
	while(res!=EOF) {
		fprintf(fend,"%c",x);
		res=fscanf(ffinal,"%c",&x);
	}
	fclose(fend);
	fclose(ffinal);
	system("rm output.txt");
	system("rm fout.txt");
	system("rm model.txt");
	printf("\n\nTERMINATO KERNEL LINEARE\n\n");
	system("shutdown -h now");
}
