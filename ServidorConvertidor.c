#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	//inicializacion
	//abrimos socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error al crear el socket");
	//Bindeamos el puerto
	
	memset(&serv_adr, 0, sizeof(serv_adr)); //inicializa a zero el serv_addr
	serv_adr.sin_family = AF_INET;
	
	//asocia el socket a cualqueira de las IP de la maquina
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	//escucharemos en el port 9050
	serv_adr.sin_port = htons(9000);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf("Error al bind");
	if (listen(sock_listen, 3) < 0)
		printf("Error al listen");
	
	int i;
	//atenderemos infinitas peticiones
	for (;;){
		printf("Escuchando\n");
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		int terminar = 0;
		while (terminar == 0) {
			//Ahora recibimos su nombre, que dejamos en buff
			ret = read(sock_conn, peticion, sizeof(peticion));
			printf("Recibido");
			//Tenemos que anadirle la marca de fin de string
			//para que no escriba lo que hay despues en el buffer
			peticion[ret]='\0';
			//escribimos nombre en la consola
			printf("Peticion: %s\n", peticion);
			//vamos a ver que quieren
			char *p = strtok(peticion, "/");
			int codigo = atoi(p);
			float temperatura;
			if (codigo != 0) {
				p = strtok(NULL, "/");
				temperatura = atof(p);
				printf("Temperatura: %i\n", temperatura);
			}
			if (codigo == 0) 
				terminar = 1;
			else if (codigo == 1) {//celsius a farenheit
				float farenheit;
				farenheit = (temperatura*9)/5 + 32;
				sprintf(respuesta, "%f", farenheit);
			}
			else if (codigo == 2){//farenheit a celsius
				float celsius;
				celsius = (temperatura - 32)*5 /9;
				sprintf(respuesta, "%f", celsius);
			}
			
			if (codigo != 0) {
				printf("Respuesta: %s\n", respuesta);
				//enviamos
				write(sock_conn, respuesta, strlen(respuesta));
			}
		}
		close(sock_conn);
	}
}

