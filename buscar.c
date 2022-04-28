#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>


/*Estructura para almacenar los datos de los viajes*/
struct viajeType
{
    int idOrigen;
    int idDestino;
    int hora;
    float meanTravelTime;
    float stDevTravelTime;
    float geoMeanTravelTime;
    float geoStDesTravelTime;
    int nextIdOrigen;
};

/*Estructura para almacenar los datos en el archivo de índices*/
struct indiceType
{
    int posicion;
};

/*Estructura que almacena los datos que ingresa el usuario*/
struct ingresoType{
    int origen, destino, hora;
};

/*Abre el archivo indices.dat para buscar la posición en el archivo datosde la primera ocurrencia 
del id origen ingresado por el usuario*/
int buscarIndice(int origenId)
{
    FILE *archivo;
    archivo = fopen("indices.dat", "rb");
    int posicion = 0;
    if (archivo == NULL)
        exit(-1);
    fseek(archivo, sizeof(struct indiceType) * origenId, SEEK_SET);
    struct indiceType indice;
    fread(&indice, sizeof(struct indiceType), 1, archivo);
    fclose(archivo);
    return indice.posicion;
}

/*Abre el archivo datos.dat buscando el registro con los datos ingresados por el usuario*/
float busqueda(int origenId, int destinoId, int hora)
{
    int posicion = buscarIndice(origenId);
    float mediaViaje = -1;
    if(posicion==-1){
        return mediaViaje;
    }
    FILE *archivo;
    archivo = fopen("datos.dat", "rb");
    if (archivo == NULL)
        exit(-1);

    /*Se abre el archivo y se ubica en la posición encontrada con la función buscarIndice, luego
    se lee el dato ahí almacenado*/
    fseek(archivo, posicion * sizeof(struct viajeType), SEEK_SET);
    struct viajeType viaje;
    fread(&viaje, sizeof(struct viajeType), 1, archivo);

    fseek(archivo, posicion * sizeof(struct viajeType), SEEK_SET);

    /*Se busca en la tabla de datos saltando a través de los diferentes next hasta encontrar el dato
    solicitado o llegar al final de las coincidencias de id origen*/
    while (viaje.idDestino != destinoId || viaje.hora != hora)
    {
        posicion = viaje.nextIdOrigen;
        fseek(archivo, posicion * sizeof(struct viajeType), SEEK_SET);
        fread(&viaje, sizeof(struct viajeType), 1, archivo);
        if (viaje.nextIdOrigen == -1)
            break;
    }

    /*Verifica que el dato encontrado coincide con la solicitud del usuario*/
    if (viaje.idDestino == destinoId && viaje.hora == hora)
    {
        mediaViaje = viaje.meanTravelTime;
    }
    fclose(archivo);
    return mediaViaje;
}

/*Abre el archivo temporal usado por la tubería para leer el dato ingresado por el usuario, luego
realiza la búsqueda solicitada y finalmente escribe el resultado nuevamente en el archivo temporal*/
int main(){
    float media;
    struct ingresoType *entrada;
    entrada = malloc(sizeof(struct ingresoType));
    int fd;

    fd=open("/tmp/mi_fifo",O_RDONLY);

    read(fd,entrada,sizeof(struct ingresoType));

    close(fd);

    clock_t time1 = clock();

    media = busqueda(entrada->origen, entrada->destino, entrada->hora);


    printf(" PID BUSQUEDA %d\n",getpid());

    //ESCRITURA DEL RESULTADO

    fd=open("/tmp/mi_fifo",O_WRONLY);    
    
    write(fd,&media,sizeof(float));

    clock_t time2 = clock();

    double time = (double)(time2-time1)/CLOCKS_PER_SEC;
    printf("Tiempo de búsqueda:%lf Segundos\n",time);
    close(fd);
    free(entrada);
}
