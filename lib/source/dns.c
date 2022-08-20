#include "../header/dns.h"

enum { CMP_ID = 0, CMP_HOST_NAME = 1};

List _DNS_req_list = {
    .root = NULL,
    .size = 0,
    .cmpData = {[CMP_ID] = _cmp_id, [CMP_HOST_NAME] = _cmp_host_name, NULL},
    .printNode = NULL,
    .deleteNode = NULL,
    .sortNode = {NULL},
};

typedef struct DNS_req_node{
    uint16_t ID;
    uint16_t pack_size;
    uint8_t* IP;
    uint8_t* hostName;
} DNS_req_node;

typedef struct __attribute__((__packed__)){
    uint16_t ID;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
}dns_header;

const char* serverList[] = {
    "8.8.8.8",
    "8.8.4.4",
    "1.1.1.1",
    "1.0.0.1",
    "9.9.9.9",
};

void DNSinitServ(){
    struct sockaddr_in aux = {
        .sin_addr = {.s_addr = inet_addr(serverList[0])},
        .sin_family = AF_INET,
        .sin_port = htons(53), 
    };

    for(uint16_t i = 1; SUCCESS != sockAdd(&aux, "DNS", UDP) && i < sizeof(serverList); i++)
        aux.sin_addr.s_addr = inet_addr(serverList[i]);
}

uint8_t* DNSgetIP(uint16_t ID){
    DNS_req_node* data = listFind(&_DNS_req_list, NULL, &ID, 0);
    return NULL != data ? data->IP : NULL;
}

uint16_t DNSrequest(uint8_t *host_name){

    //Looks if request has already been made
    DNS_req_node *aux = listFind(&_DNS_req_list, NULL, host_name, CMP_HOST_NAME);

    //Return request ID if found
    if(aux != NULL)
        return aux->ID;

    //If request has not been made yet for the hostname, create request packet and send
    srand(time(NULL));
    dns_header header ={
        .ID = rand() % __UINT16_MAX__,
        .flags = 0x0001,
        .qdcount = 0x0100,
        .ancount = 0x0000,
        .nscount = 0x0000,
        .arcount = 0x0000
    };

    uint8_t *question = _DNSparseHn(host_name);

    uint8_t footer[]= {0x00,0x01,0x00,0x01};

    uint16_t data_packet_sz = sizeof(header) + strlen(question) + 1 + sizeof(footer);
    uint8_t *data_packet = calloc( data_packet_sz, sizeof(uint8_t));

    uint32_t offset = 0;
    memcpy( (data_packet + offset),    &header,    sizeof(header));
    offset += sizeof(header);
    memcpy( (data_packet + offset),    question,   strlen(question)+1);
    offset += strlen(question)+1;
    memcpy( (data_packet + offset),    footer,     sizeof(footer));
    offset += sizeof(footer);

    pack_node dns_pack = {
        .data = data_packet,
        .size = offset,
        .user = strcpy(calloc(10, sizeof(uint8_t)), "DNS"),
    };
    sockWriteQ(&dns_pack);

    DNS_req_node req_node = {
        .ID = header.ID,
        .hostName = strcpy(calloc(strlen(host_name), sizeof(uint8_t)), host_name),
        .IP = NULL,
        .pack_size = sizeof(header) + strlen(question) + 1 + sizeof(footer),
    };
    listInsert(&_DNS_req_list, &req_node, sizeof(DNS_req_node), LAST_POS);

    return header.ID;
}

uint8_t* _DNSparseHn(uint8_t *host_name){
    
    uint8_t *token, *str;
    uint8_t *buffer_hn = malloc(strlen(host_name));
    uint8_t *buffer_out = calloc(strlen(host_name), sizeof(uint8_t));

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
    
    free(buffer_hn);
    return buffer_out;
}

void _DNSprocessPacket(pack_node *DNS_pack){

    uint16_t* ID_ptr = DNS_pack->data;
    uint8_t* pack_ptr = DNS_pack->data;
    DNS_req_node* data = listFind(&_DNS_req_list, NULL, ID_ptr, CMP_ID);
    
    uint16_t pack_size = data->pack_size;
    if( pack_ptr[pack_size] == 0xC0 && pack_ptr[pack_size+1] == 0x0C ){
        sprintf( data->IP = calloc(10, sizeof(uint8_t)), "%d.%d.%d.%d", pack_ptr[ pack_size + 12], pack_ptr[ pack_size + 13], 
                                                                        pack_ptr[ pack_size + 14], pack_ptr[ pack_size + 15]);
        fprintf( stdout, "DNS Resp: %d.%d.%d.%d\n", pack_ptr[ pack_size + 12], pack_ptr[ pack_size + 13], 
                                                    pack_ptr[ pack_size + 14], pack_ptr[ pack_size + 15]);                                                                        
    }
}

static int16_t _cmp_id(void* s_data, void* data){
    return !((DNS_req_node*)data)->ID == *(uint16_t*)s_data;
}

static int16_t _cmp_host_name(void* s_data, void* data){
    return strcmp(s_data, ((DNS_req_node*)data)->hostName);
}