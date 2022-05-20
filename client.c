#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    char *logincookie=NULL;
    char *token=NULL;
    int sockfd;

    char command[100];
    while(1) {
        scanf("%s", command);
        if(strncmp(command, "exit", 4)==0) {
            break;
        }
        else {
            sockfd=open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            if(strncmp(command, "register", 8)==0) {
                char* username = malloc(100*sizeof(char));
                char* password = malloc(100*sizeof(char));
                printf("username=");
                scanf("%s", username);
                printf("password=");
                scanf("%s", password);
                JSON_Value *value = json_value_init_object();
                JSON_Object *object = json_value_get_object(value);
                json_object_set_string(object, "username", username);
                json_object_set_string(object, "password", password);
                message=compute_post_request("34.241.4.235", "/api/v1/tema/auth/register", "application/json", json_serialize_to_string_pretty(value), NULL, NULL, 0);
                send_to_server(sockfd, message);
                response=receive_from_server(sockfd);
                if(strstr(response, "taken")!=NULL) {
                    printf("400 -Bad Request- Utilizatorul deja exista!\n");
                }
                else printf("200 - OK - Utilizator inregistrat cu succes!\n");
                close_connection(sockfd);
                free(message);
                free(response);
                json_value_free(value);
            }
            if(strncmp(command, "login", 5)==0) {
                if(logincookie!=NULL) {
                    printf("Sunteti deja logat! Delogati-va inainte de a reincerca!\n");
                    continue;
                }
                char* username = malloc(100*sizeof(char));
                char* password = malloc(100*sizeof(char));
                printf("username=");
                scanf("%s", username);
                printf("password=");
                scanf("%s", password);
                JSON_Value *value = json_value_init_object();
                JSON_Object *object = json_value_get_object(value);
                json_object_set_string(object, "username", username);
                json_object_set_string(object, "password", password);
                message=compute_post_request("34.241.4.235", "/api/v1/tema/auth/login", "application/json", json_serialize_to_string_pretty(value), NULL, NULL, 0);
                send_to_server(sockfd, message);
                response=receive_from_server(sockfd);
                if(strstr(response, "No account with this username!")!=NULL) {
                    printf("400 - Bad Request - Nu exista acest utilizator!\n");
                }
                else if(strstr(response, "Credentials are no good!")!=NULL) {
                    printf("400 - Bad Request - Credentiale gresite!\n");
                }
                else {
                    printf("200 - OK - Bun venit!\n");
                    char *cookie=strstr(response, "Set-Cookie: ");
                    strtok(cookie, ";");
                    cookie=cookie+12;
                    logincookie=cookie;
                }
                close_connection(sockfd);
                free(message);
                free(response);
                json_value_free(value);
            }
            if(strncmp(command, "enter_library", 13)==0) {
                if(logincookie!=NULL) {
                    message=compute_get_request("34.241.4.235", "/api/v1/tema/library/access", NULL, &logincookie, 1);
                }
                message=compute_get_request("34.241.4.235", "/api/v1/tema/library/access", NULL, NULL, 0);
                send_to_server(sockfd, message);
                response=receive_from_server(sockfd);
                if(strstr(response, "You are not logged in!")!=NULL) {
                    printf("401 - Unauthorized - Nu sunteti autentificat!\n");
                    
                }
                else {
                    char *tokenptr = strstr(response, "token");
                    tokenptr=tokenptr+8;
                    strtok(tokenptr, "\"");
                    token=tokenptr;
                    printf("%s", token);
                }
                close_connection(sockfd);
                free(message);
                free(response);
            }
        }
    }

    return 0;
}
