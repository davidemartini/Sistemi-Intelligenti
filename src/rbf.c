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
	double k,i,j,z,maxrbf,cbest,gbest,perc,lim,lowbc,lowbg,uppbc,uppbg,incrementc,incrementg;
	char command[100],cost[20],gamma[20],vett[100],x,xvett[4];
	int res,head=0,flag=0,print=0,count,ciclo;
	fout=fopen("outputrbf.txt", "w");
	fclose(fout);
	ffinal=fopen("fout.txt", "w");
	fclose(ffinal);
	lowbc=-15;
	uppbc=16;
	lowbg=-16;
	uppbg=16;
	incrementc=0;
	incrementg=0;
	for(ciclo=0;ciclo<2;ciclo++) {
		for(i=lowbc;i<uppbc;i++) { //esponente -c (la prima iterazione inizializza i file) 
			for(j=lowbg;j<uppbg;j++) {
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
						j--;
					}
					else {
						z=pow(2,j);
					}
			   		strcpy(command,"./svm-train -s 0 -t 2 -c "); 
					snprintf(cost,10,"%f",k);
					strcat(command,cost);
					strcpy(vett," -g ");
					strcat(command,vett);
					snprintf(cost,10,"%f",z);
					strcat(command,cost);
					strcpy(vett," -v 10 ./svm_dataset/svm/spam_train_std.txt > outputrbf.txt\n");
					strcat(command,vett);
					system(command);
					printf("%s\n",command);
					strcpy(command,"");
					fout=fopen("outputrbf.txt","r");
					if(fout==NULL) {
						perror("Errore in apertura del file OUTPUT");
						exit(1);
					}
					fend=fopen("accuracyrbf.txt", "a");
					if(fend==NULL) {
						perror("Errore in apertura del file accuracyrbf");
						exit(1);
					}
					if(head==0) {
						fprintf(fend,"rbf KERNEL\n");
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
							if(perc>maxrbf) {
								maxrbf=perc;
								cbest=k;
								gbest=z;
								printf("c %f\n",cbest);
								printf("gamma %f\n",gbest);
							}
							fprintf(fend,"\nC=%f gamma=%f\n",k,z);
						}
						//**Fine filtro
						//Inserire al posto del filtro fprintf(fend,"%c",x);						
						res=fscanf(fout,"%c",&x);
					}
					fclose(fout);
					fclose(fend);
				
				}
				else {
					printf("rbf KERNEL\n");
					flag=1;
				}
			}
			if(incrementc!=0) i--;
		}
		uppbc=cbest*2;
		lowbc=cbest/2;
		incrementc=(uppbc-lowbc)/20;
		uppbg=gbest*2;
		lowbg=gbest/2;
		incrementg=(uppbg-lowbg)/20;
	}
	system("rm outputrbf.txt");
	strcpy(command,"./svm-train -s 0 -t 2 -c "); 
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
	fend=fopen("accuracyrbf.txt", "a");
	if(fend==NULL) {
		perror("Errore in apertura del file accuracyrbf");
		exit(1);
	}			
	res=fscanf(ffinal,"%c",&x);
	fprintf(fend,"\n\nC=%f gamma=%f\nPercentuale train=%f\n",cbest,gbest,maxrbf);
	while(res!=EOF) {
		fprintf(fend,"%c",x);
		res=fscanf(ffinal,"%c",&x);
	}
	fclose(fend);
	fclose(ffinal);
	system("rm output.txt");
	system("rm fout.txt");
	system("rm model.txt");
	printf("\n\nTERMINATO KERNEL RBF\n\n");
	system("shutdown -h now");
}
