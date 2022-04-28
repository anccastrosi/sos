#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

/*Estructura para almacenar los datos ingresados por el usuario*/
struct ingresoType{
    int idOrigen, idDestino, hora;
};

/*Se presenta la interfaz al usuario, donde se leen los datos del usuario y se guardan en el archivo
temporal usado por la tubería para la comunicación con el proceso de búsqueda*/
int main(){
    struct ingresoType *entrada;
    entrada= malloc(sizeof(struct ingresoType));

    printf("PID INTERFAZ %d\n",getpid());

    printf("BÚSQUEDA DE VIAJE MEDIO\nIngrese el id origen:");
    scanf("%i", &entrada->idOrigen);
    printf("Ingrese el id destino:");
    scanf("%i", &entrada->idDestino);
    printf("Ingrese la hora:");
    scanf("%i", &entrada->hora);

    
    long inicio= clock();

    //ESCRITURA DE LA ENTRADA INGRESADA POR EL USUARIO
    int fd;

    mkfifo("/tmp/mi_fifo",0666);

    fd=open("/tmp/mi_fifo",O_WRONLY);
    
    
    write(fd,entrada,sizeof(struct ingresoType));
    
    close(fd);    

    //LECTURA DE LA MEDIA ESCRITA POR EL PROCESO DE BÚSQUEDA

    float media;
    fd=open("/tmp/mi_fifo",O_RDONLY);

    read(fd,&media,sizeof(float));
    
    if (media == -1)
    {
        printf("NA\n");
    }
    else
    {
        printf("Tiempo de viaje medio: %f\n", media);
    }
    free(entrada);
    close(fd);    
    long fin = clock();


    double tiempoTotal = (double)(fin-inicio)/CLOCKS_PER_SEC;
    printf("Tiempo total de ejecución:%lf Segundos\n",tiempoTotal);
}
