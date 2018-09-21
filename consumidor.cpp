#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <semaphore.h>
#include <time.h>
#include <ctime>
//El tamaño de la memoria compartida
#define MAX_BUFFER 1024
#define DATOS_A_PRODUCIR 10000
using namespace std;


struct Ordenes {
	int numeroCliente;
	int numeroOrden;
	float cantidadPagar;
	char fecha[30];
	char tipoPago[10];
};

struct Segmento {
	Ordenes buffer[MAX_BUFFER];
	sem_t sem_productor;
	sem_t sem_consumidor;
	sem_t mutex_pantalla;
	int pos_productor;
	int pos_consumidor ;
	int num;


};



//VARIABLES GLOBALES	
Segmento segmento;
Ordenes orden;
Segmento *shm;

void imprimirOrden(Ordenes orden){
	cout<<"Numero Cliente: "<<orden.numeroCliente<<endl;
	cout<<"Numero Orden: "<<orden.numeroOrden<<endl;
	cout<<"Cantidad a pagar: "<<orden.cantidadPagar<<endl;
	cout<<"Fecha: "<<orden.fecha;
	cout<<"Tipo de pago: "<<orden.tipoPago<<endl;
}

void consumir_entrada(Ordenes dato){
	sem_wait(&shm->mutex_pantalla);
	cout<<"\nDato consumido " <<endl;
	imprimirOrden(dato); 
	sem_post(&shm->mutex_pantalla);
}

int main(int argc, char const *argv[])

{
	int shmid;
	key_t key;
	key = 9876;

	shmid = (int)shmget(key , sizeof(Ordenes)*1024 ,  0666);
	if(shmid < 0){
		perror("shmget");
		exit(1);
	}

	shm  = (Segmento*)shmat(shmid , NULL , 0);


	if(shm == (Segmento *)-1){
		perror("shmat");
		exit(1);
	}
	//sem_init(&shm->semaphore , 1 , 1);
	
	//sem_init(&shm->sem_productor , 1 , MAX_BUFFER);
	//sem_init(&shm->sem_consumidor , 1 , 0);
	//sem_init(&shm->mutex_pantalla , 1 , 1);

	for (int i = 0; i < DATOS_A_PRODUCIR; ++i)
	{
		sem_wait(&shm->sem_consumidor);
		Ordenes orden = shm->buffer[shm->pos_consumidor% MAX_BUFFER];
		shm->pos_consumidor++;	
		sem_post(&shm->sem_productor);
		consumir_entrada(orden);
		sleep(1);
	}	


	/* code */
	return 0;
}