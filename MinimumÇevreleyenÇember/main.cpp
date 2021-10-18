#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define PI 3.14159265
typedef struct tkoordinats {
float x, y;   //int tanýmlýydý
}koordinats;
typedef struct tdgm{
	koordinats nokta;
	struct tdgm *ileri;
}dgm;
typedef struct tcember {
	float r;
	float x;
	float y;
	float alan;

}cember;
int cevredekiNoktaSayisi;

bool saatYonuTersiMi(const koordinats *p1,const koordinats *p2,const koordinats *p3){
	return ((p2->x - p1->x)*(p3->y - p1->y))>((p2->y - p1->y)*(p3->x - p1->x));
}

int karsilastir(const void *p1 ,const void *p2){// Noktalarýn x koordinatlarýný karþýlaþtýrýr.
	koordinats _p1;
	koordinats _p2;
	_p1=*((koordinats *)p1);
	_p2=*((koordinats *)p2);
	
	if(_p1.x > _p2.x)
		return 1;
	if(_p1.x < _p2.x)
		return -1;
	return 0;
}

void dugumSil(dgm *ptr){ // BAGLÝLÝSTEYÝ SÝLER
	if(ptr == NULL){
		return;
	}
	
	dugumSil(ptr->ileri);
	ptr -> ileri  =NULL;
	free(ptr);
}


dgm* dugumEkle(dgm *ptr, koordinats nokta){ // DUGUM EKLEME
	dgm *tmp = ptr;
	if(ptr == NULL){
		ptr = (dgm*)malloc(sizeof(dgm));
		ptr->nokta = nokta;
		ptr->ileri=NULL;
		return ptr;
	}	
	while(tmp->ileri!=NULL){
		tmp = tmp->ileri;
	}
	
	tmp->ileri = (dgm*)malloc(sizeof(dgm));
	tmp->ileri->nokta = nokta;
	tmp->ileri->ileri=NULL;
	return ptr;
}
dgm* dgmSondanSil(dgm *ptr) { // DUGUM SÝLME SONDAN
    dgm *tmp = ptr;
 
    if (ptr == NULL) {
        return NULL;
    }
    if (ptr->ileri == NULL) {
        free(ptr);
        return NULL;
    }
 
    while (tmp->ileri->ileri != NULL) {
        tmp = tmp->ileri;
    }
 
    free(tmp->ileri);
    tmp->ileri = NULL;
    return ptr;
}



void print(dgm *ptr) { //  CEVREDEKÝ NOKTALARI CÝZDÝRÝR
    printf("[");

    while (ptr != NULL) {
        printf(", (%.2f, %.2f)", ptr->nokta.x, ptr->nokta.y);
        setcolor(BLUE);
        circle((ptr->nokta.x * 3.45+300),(ptr->nokta.y * -3.45+250),3); 
        ptr = ptr->ileri;
    }
    printf("]");
}

dgm* cevrele(int len,koordinats p[]){ // JARVÝS ALGORÝTMASIYLA CONVEXHULL 
	dgm *liste =NULL;
	dgm *listePtr=NULL;
	int i;
	size_t listeLen = 0; 
	
	qsort(p,len, sizeof(koordinats),karsilastir);
	for (i = 0; i < len; ++i) {// alt çevreleme
        while (listeLen >= 2) {
            listePtr = liste;
            while (listePtr->ileri->ileri != NULL) {
                listePtr = listePtr->ileri;
            }
            if (saatYonuTersiMi(&listePtr->nokta, &listePtr->ileri->nokta, &p[i])) {
                break;
            }
 
            liste = dgmSondanSil(liste);
            listeLen--;
        }
 
        liste = dugumEkle(liste, p[i]);
        
        listeLen++;
        
    }
    
    
    for (i = len - 1; i >= 0; i--) { //üst çevreleme
        while (listeLen >= 2) {
            listePtr = liste;
            while (listePtr->ileri->ileri != NULL) {
                listePtr = listePtr->ileri;
            }
            if (saatYonuTersiMi(&listePtr->nokta, &listePtr->ileri->nokta, &p[i])) {
                break;
            }
 
            liste = dgmSondanSil(liste);
            listeLen--;
        }
 
        liste = dugumEkle(liste, p[i]);
        listeLen++;
    }
    cevredekiNoktaSayisi=listeLen-1;
    dgmSondanSil(liste);
    return liste;
	
}

// ----------------------- ÇEMBER UTÝLÝTY ----------------------------------//

float cemberAlanBul(float r){
	return 3.14159 * pow(r,2);
}

bool cemberIcindeMi(koordinats p , cember c){
	// (x-a)^2 + (y-b)^2 = r^2
	
	return ( pow((c.x - p.x),2) +  pow((c.y - p.y),2) ) <= ( pow(c.r , 2) );
}

cember merkezBul(const koordinats p1, const koordinats p2,const  koordinats p3 ){
	
	float x12 = p1.x - p2.x;
	float x13 = p1.x - p3.x;
	
	float y12 = p1.y - p2.y;
	float y13 = p1.y - p3.y;
	
	float y31 = p3.y - p1.y;
	float y21 = p2.y - p1.y;
	
	float x31 = p3.x - p1.x;
	float x21 = p2.x - p1.x;

	
	float ax13 = pow (p1.x,2) - pow(p3.x,2);
	float ay13 = pow (p1.y,2) - pow(p3.y,2);
	float ax21 = pow (p2.x,2) - pow(p1.x,2);
	float ay21 = pow (p2.y,2) - pow(p1.y,2);
	
	float f = ((ax13) * (x12) 
             + (ay13) * (x12) 
             + (ax21) * (x13) 
             + (ay21) * (x13)) 
            / (2 * ((y31) * (x12) - (y21) * (x13))); 
    
	float g =  ((ax13) * (y12) 
             + (ay13) * (y12) 
             + (ax21) * (y13) 
             + (ay21) * (y13)) 
            / (2 * ((x31) * (y12) - (x21) * (y13)));      
			
	float c = - pow(p1.x,2) - pow(p1.y,2) - (2 * g * p1.x) - (2 * f * p1.y);
	  
	float h = -g; 
    float k = -f; 
    float kok_r = h * h + k * k - c; 
    
    float _r;
	_r = sqrt(kok_r);
    cember c1;
    c1.x=h;
    c1.y=k;
    c1.r=_r;
    
    return c1;
}
// ---------------------------- en küçük çember --------------------------------------

cember enKucukCemberBul(dgm *cevrele, koordinats tumNoktalar[] ,int len , int tumNoktaSayisi){
	
	dgm *tmp = cevrele;


	koordinats noktalar[len];
	int sayac=0;
	while(tmp != NULL){
		noktalar[sayac].x = tmp->nokta.x;
		noktalar[sayac++].y = tmp->nokta.y;
		tmp = tmp->ileri;
	}
	// --------  TEST ------- //
		int w;
		/*
		printf(" \n ****TUM NOKTALAR**** \n");
		for(w=0; w<20; w++){
			printf(" , (%.1f,%.1f) ",tumNoktalar[w].x,tumNoktalar[w].y);
		}*/


	// --------  TEST ------- //
	//kombinasyon hesapla
	int komsayisi=0;
	koordinats noktalariki[500][3];
	
	sayac=0;
	int kontrol=0;

	int i,j,k,a=0,e;// len = 7
	 for(i=0; i<len-2; i++){  // 0 - 4
	 	for(j=i; j<len-1; j++){ // 1 - 5
	 		for(k=j; k<len; k++){ // 2 - 6
	 			if(i!=j && i!=k && j!=k){
	 				noktalariki[sayac][0] =noktalar[i];
	 				noktalariki[sayac][1] =noktalar[j];
	 				noktalariki[sayac++][2] =noktalar[k];	
				}
	 		}
	 	}
	 }
	 int m,p;
	 bool icindemi;
	 cember cList[sayac];
	 cember cemberList[sayac];//tüm noktalarý içeren çemberlerin listesi
	 int indeks=0;
	 for(m=0;m<sayac;m++){
	 	icindemi = true;
		cList[m] =merkezBul(noktalariki[m][0],noktalariki[m][1],noktalariki[m][2]);

		 for(p=0;p<tumNoktaSayisi;p++){
		 	if(!cemberIcindeMi(tumNoktalar[p],cList[m])){
		 		icindemi=false;
		 		break;
			 }
		 }
		 
		 if(icindemi){
		 	
		 	cemberList[indeks]=cList[m];// tüm noktalarý içeren çemberler listesi
		 	cemberList[indeks].alan=cemberAlanBul(cemberList[indeks].r);
		 	indeks++;
		 }	 

	 }

	cember enKucuk;	
	
	enKucuk=cemberList[0];
	
	for(e=0;e<indeks;e++){
		if(cemberList[e].alan < enKucuk.alan)
			enKucuk=cemberList[e];
	}
	
	return enKucuk;	
}

int main()
{
	//grafik ekranÄ±nÄ± olusturma

    int gd=DETECT,gm;
    initgraph(&gd,&gm,(char*)"");


	//test.txt dosyasÄ±ndan koordinat noktalarÄ±nÄ± alma
	
    FILE *dosya;
    dosya=fopen("test.txt", "r");
    int i;
    int noktaSayisi;
    printf("Lutfen nokta sayisini giriniz... \n");
    scanf("%d",&noktaSayisi);
    
    koordinats koordinat[noktaSayisi];
    for (i=0;i<noktaSayisi;i++) {
		
		
        fscanf(dosya,"%f%f",&koordinat[i].x,&koordinat[i].y);
        
		// OKUNAN NOKTALARI YAZDIRIR
        printf("x : %.2f , y : %.2f\n",koordinat[i].x,koordinat[i].y);
        
		//fprintf(dosya,"%.2f %.2f",&koordinat[i].x,&koordinat[i].y);  
        
    }

	

    //koordinat dÃ¼zlemini olusturma

    setcolor(WHITE);
    setlinestyle(SOLID_LINE, 0, THICK_WIDTH);
    line(300, 30, 300, 470); //Dik cizgi
    line(50, 250, 550, 250); //Yatay cizgi

    // CEVRELENMÝS NOKTALARI EKRANA CÝZDÝRME
    dgm *cevrelePtr = cevrele(noktaSayisi, koordinat);
    	
	//noktalarý oluþturma
	for (i=0;i<noktaSayisi;i++) {
		setcolor(RED);
        circle((koordinat[i].x*3.45+300),(koordinat[i].y*-3.45+250),1); 
        
    }

    

    

    
    printf("\n");
    // EN KUCUK CEMBERÝN EKRANA CÝZÝLMESÝ
 	cember enKucukCember = enKucukCemberBul(cevrelePtr,koordinat,cevredekiNoktaSayisi,noktaSayisi);
 	setcolor(RED);
	circle((int)enKucukCember.x*3.45+300,(int)enKucukCember.y*-3.45+250,(int)enKucukCember.r*3.60);

    printf("\n MINIMUM CEVRELEYEN CEMBERIN MERKEZI : (%2.f , %2.f)\n MINIMUM CEVRELEYEN CEMBERIN YARICAPI : %f \n",enKucukCember.x,enKucukCember.y,enKucukCember.r);

	printf("\nCEVRELEME FONKSIYONUN BULDUGU NOKTA SAYISI : %d\n",cevredekiNoktaSayisi);
	
	
	
    //koordinat dÃ¼zleminin noktalarÄ±nÄ± belirleme

    setcolor(0010);
     for(i=55;i<550;i++){
        circle(i,250,2);
        i=i+34.5;
     }
     for(i=40;i<480;i++){
        circle(300,i,2);
        i=i+34.5;
     }

    // koordinat dÃ¼zlemine deger atama

    settextstyle(0,0,1);
    int deger=0;
    char sayi[30];
    for(i=305;i<550;i++){
        sprintf(sayi,"%d",deger);
        outtextxy(i,255,sayi);
        deger=deger+10;
        i=i+32;
    }
    deger=10;
    for(i=205;i>20;i++){
        sprintf(sayi,"%d",deger);
        outtextxy(310,i,sayi);
        deger=deger+10;
        i=i-35;
    }
    deger=-10;
    for(i=280;i<550;i++){
        sprintf(sayi,"%d",deger);
        outtextxy(310,i,sayi);
        deger=deger-10;
        i=i+33;
    }
    deger=-10;
    for(i=250;i>40;i++){
        sprintf(sayi,"%d",deger);
        outtextxy(i,255,sayi);
        deger=deger-10;
        i=i-35;
    }
	    print(cevrelePtr);
	        double koor_x,koor_y,z;
	int g;
	
	qsort(koordinat,noktaSayisi, sizeof(koordinats),karsilastir);
	for(g=0;g<noktaSayisi;g=g+3){
		for(z=0.0;z<=1.0;z=z+0.001){
	       koor_x=pow(1-z,3)*(koordinat[g].x*3.45+300)+3*z*pow(1-z,2)*(koordinat[g+1].x*3.45+300)+3*z*z*(1-z)*(koordinat[g+2].x*3.45+300)+pow(z,3)*(koordinat[g+3].x*3.45+300);
	       koor_y=pow(1-z,3)*(koordinat[g].y-3.45+250)+3*z*pow(1-z,2)*(koordinat[g+1].y-3.45+250)+3*z*z*(1-z)*(koordinat[g+2].y-3.45+250)+pow(z,3)*(koordinat[g+3].y-3.45+250);
	       putpixel(koor_x,koor_y,BLUE);
    }
		//setcolor(WHITE);
	    //setlinestyle(SOLID_LINE, 2, NORM_WIDTH);
	    //line(koordinat[g].x*3.45+300,koordinat[g].y*-3.45+250,koordinat[g+1].x*3.45+300,koordinat[g+1].y*-3.45+250) ;
	    //line(koordinat[g+1].x*3.45+300,koordinat[g+1].y*-3.45+250,koordinat[g+2].x*3.45+300,koordinat[g+2].y*-3.45+250) ;
	    //line(koordinat[g+2].x*3.45+300,koordinat[g+2].y*-3.45+250,koordinat[g+3].x*3.45+300,koordinat[g+3].y*-3.45+250) ;
	
	
	}
    dugumSil(cevrelePtr);
    cevrelePtr = NULL;

    getch();
    fclose(dosya);
    closegraph();

    return 0;
}

