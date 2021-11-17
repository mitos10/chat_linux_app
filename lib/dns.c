#include "dns.h"

void init_DNS_serv(){
    struct sockaddr_in aux;
    aux.sin_family = AF_INET;
    aux.sin_addr.s_addr = inet_addr("208.67.222.222");
    aux.sin_port = htons(53);
    add_socket(&aux, "DNS", UDP);
}

void dns_request(char *host_name){
    
    srand(time(NULL));
    dns_header header ={
        .ID = rand() % __UINT16_MAX__,
        .flags = 0x0001,
        .qdcount = 0x0100,
        .ancount = 0x0000,
        .nscount = 0x0000,
        .arcount = 0x0000
    };

    char footer[]= {0x00,0x01,0x00,0x01};

    char *question = _dns_parse_hn(host_name);

    char *data_packet = calloc( sizeof(header) + strlen(question) + 1 + sizeof(footer), sizeof(char));

    memcpy(data_packet, &header, sizeof(header));
    memcpy(data_packet+sizeof(header), question, strlen(question)+1);
    memcpy(data_packet+sizeof(header)+strlen(question)+1, footer, sizeof(footer));
    pack_node* dns_pack = (pack_node*)malloc(sizeof(pack_node));
    dns_pack->user = malloc(50);
    sprintf(dns_pack->user,"DNS");
    dns_pack->data = data_packet;
    dns_pack->size = sizeof(header) + strlen(question) + 1 + sizeof(footer);
    write_queue(dns_pack);
    printf("DNS packet:\n");
    for(int i = 0; i < (sizeof(header) + strlen(question) + 1 + sizeof(footer)); i++){
        printf("%.2hhX ", data_packet[i]);
    }
    printf("\n");

    printf("SIZE HEADER %d %d\n",sizeof(header), sizeof(footer));
    fflush(stdout);
}

char* _dns_parse_hn(char *host_name){
    
    char *token, *str;
    char *buffer_hn = (char*) malloc(strlen(host_name)), *buffer_out= (char*)calloc(strlen(host_name), sizeof(char));
    strcpy(buffer_hn,host_name);
    
    for( str = buffer_hn; ; str = NULL) {
        token = strtok(str, ".");
        if(token != NULL){
            char size[2] = {strlen(token), '\0'};
            strcat(buffer_out,size);
            strcat(buffer_out,token);
        }
        else break;
    }
    #ifdef DEBUG
    for(int i = 0; buffer_out[i] != '\0'; i++)
        printf("%.2X ",buffer_out[i]);
    printf("%.2X\n",'\0');
    printf("OUTPUT BUFFER \"%s\"\n",buffer_out);
    #endif
    free(buffer_hn);
    return buffer_out;
}

void _process_dns_packet(pack_node *DNS_pack){
    char IP_buffer[200];
    sprintf(IP_buffer,"%d.%d.%d.%d",((char*)DNS_pack->data)[ DNS_pack->size -4] &0xFF, (unsigned int)((unsigned char*)DNS_pack->data)[ DNS_pack->size -3] &0xFF, (unsigned int)((char*)DNS_pack->data)[ DNS_pack->size -2]&0xFF, (unsigned int)((char*)DNS_pack->data)[ DNS_pack->size -1]&0xFF);
    printf("IP DNS: %s\n", IP_buffer);
}