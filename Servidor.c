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
			char nombre[20];
			if (codigo != 0) {
				p = strtok(NULL, "/");
				strcpy(nombre, p);
				printf("Codigo: %d, Nombre: %s\n", codigo, nombre);
			}
			if (codigo == 0) 
				terminar = 1;
			else if (codigo == 1) //longitud nombre
				sprintf(respuesta, "%d", strlen(nombre));
			else if (codigo == 2)//nombre bonito
				if ((nombre[0]=='L' || nombre[0] == 'R'))
					strcpy(respuesta, "SI,");
				else 
					strcpy(respuesta, "NO,");
			else if (codigo == 3){ //decir altura
				p = strtok(NULL, "/");
				float altura = atof(p);
				if (altura > 1.70)
					sprintf(respuesta, "%s: Eres alto", nombre);
				else 
					sprintf(respuesta, "%s: Eres bajito", nombre);
			}
			else if (codigo == 4) { //palindromo
				//pasamos nombre a minusculas
				int len = strlen(nombre);
				int palindromo = 1;
				char miniNombre[len+1];
				for (int i = 0; i < len;i++) {
					miniNombre[i] = tolower(nombre[i]);
				}
				miniNombre[len] = '\0';
				int i, j;
				//comparamos miniNombre con miniNombre pero al reves
				for (i = 0, j = len-1; i < j; ++i, --j) {
					if (miniNombre[i] != miniNombre[j]) palindromo = 0;
				}
				if ((palindromo == 1))
					strcpy(respuesta, "SI,");
				else 
					strcpy(respuesta, "NO,");
			}
			else if (codigo == 5) { //mayusculas
				int len = strlen(nombre);
				char maxiNombre[len+1];
				for (int i = 0; i < len;i++) {
					maxiNombre[i] = toupper(nombre[i]);
				}
				maxiNombre[len] = '\0';
				strcpy(respuesta, maxiNombre);
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

