#include "../header/dns.h"
#include "../header/list.h"

LIST(_DNS_req_list);

typedef struct DNS_req_node{
    uint16_t ID;
    unsigned int pack_size;
    char* IP;
} DNS_req_node;

typedef struct __attribute__((__packed__)){
    uint16_t ID;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
}dns_header;

char* _DNS_parse_hn(char *host_name);
static unsigned int _cmp_id(void* s_data, void* data);

void init_DNS_serv(){
    struct sockaddr_in aux;
    aux.sin_family = AF_INET;
    aux.sin_addr.s_addr = inet_addr("8.8.8.8");
    aux.sin_port = htons(53);
    add_socket(&aux, "DNS", UDP);
}

char* get_IP(uint16_t ID){
    unsigned int pos;
    find_node(&_DNS_req_list,&pos,_cmp_id,&ID);
    NODE *aux = get_node(&_DNS_req_list,pos);
    return aux != NULL ? ((DNS_req_node*)aux->data)->IP : NULL;
}

uint16_t DNS_request(char *host_name){
    
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

    char *question = _DNS_parse_hn(host_name);

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

    printf("SIZE HEADER %ld %ld\n",sizeof(header), sizeof(footer));
    fflush(stdout);

    DNS_req_node* req_node = (DNS_req_node*) malloc(sizeof(DNS_req_node));
    req_node->ID = header.ID;
    req_node->IP = NULL;
    req_node->pack_size = sizeof(header) + strlen(question) + 1 + sizeof(footer);
    insert_node(&_DNS_req_list,create_node (req_node),LAST_NODE);
    return header.ID;
}

char* _DNS_parse_hn(char *host_name){
    
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

    uint16_t ID = *((uint16_t*)DNS_pack->data);
    unsigned int pos;
    find_node(&_DNS_req_list,&pos,_cmp_id,&ID);
    NODE* aux = get_node(&_DNS_req_list,pos);
    unsigned int pack_size = ((DNS_req_node*)aux->data)->pack_size;

    if(  (((char*)DNS_pack->data)[pack_size] & 0xFF) == 0xC0 && (((char*)DNS_pack->data)[pack_size+1] & 0xFF) == 0x0C ){
        ((DNS_req_node*)aux->data)->IP = (char*) calloc(200, sizeof(char));
        sprintf( ((DNS_req_node*)aux->data)->IP,"%d.%d.%d.%d",  (unsigned int)((char*)DNS_pack->data)[ pack_size + 12] &0xFF, (unsigned int)((char*)DNS_pack->data)[ pack_size + 13] &0xFF, 
                                                                (unsigned int)((char*)DNS_pack->data)[ pack_size + 14] &0xFF, (unsigned int)((char*)DNS_pack->data)[ pack_size + 15] &0xFF);
    }
}

static unsigned int _cmp_id(void* s_data, void* data){
    return ((DNS_req_node*)data)->ID == *(uint16_t*)data;
}